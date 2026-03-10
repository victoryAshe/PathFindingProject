#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Util.h"

#include <cstring>

namespace Wanted
{
	Renderer::Frame::Frame(int bufferCount)
	{
		// 배열 생성 및 초기화.
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, 0, sizeof(int) * bufferCount);
	}

	Renderer::Frame::~Frame()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}

	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		// 2차원 배열로 다루는 1차원 배열을 순회하면서
		// 빈 문자(' ')를 설정!
		const int width = screenSize.x;
		const int height = screenSize.y;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				// 배열 index 구하기.
				const int index = (y * width) + x;

				// 글자 값 및 속성 설정.
				CHAR_INFO& info = charInfoArray[index];
				info.Char.UnicodeChar = ' ';
				info.Attributes = 0;

				// sortingOrder 초기화.
				sortingOrderArray[index] = -1;
			}
		}
	}

	// ==================== Frame ============================

	// 정적 변수 초기화.
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		instance = this;

		// Create instance of Frame struct
		const int bufferCount = screenSize.x * screenSize.y;
		frame = new Frame(bufferCount);

		// Initialize frame.
		frame->Clear(screenSize);

		// Create instance of Double Buffer
		screenBuffers[0] = new ScreenBuffer(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = new ScreenBuffer(screenSize);
		screenBuffers[1]->Clear();

		// UI <=> Woarld Rect 경계 그려줄 문자.
		borderChar[0] = '#';
	}

	Renderer::~Renderer()
	{
		SafeDelete(frame);

		for (ScreenBuffer*& buffer : screenBuffers)
		{
			if (buffer)
			{
				SafeDelete(buffer);
			}
		}
	}

	Renderer& Renderer::Get()
	{
		if (!instance)
		{
			MessageBoxA(
				nullptr,
				"Renderer::Get() - instance is null.",
				"Error",
				MB_OK
			);

			__debugbreak();
		}

		return *instance;
	}

	void Renderer::Clear()
	{
		// Clear Screen.
		// 1. Frmae(2차원 배열 데이터) 지우기.
		frame->Clear(screenSize);

		// 콘솔 버퍼 지우기.
		GetCurrentBuffer()->Clear();
	}

	void Renderer::Draw()
	{
		// Clear Screen.
		Clear();

		const IntRect screenRect = GetScreenRect();

		for (const RenderCommand& command : renderQueue)
		{
			if (!command.text || command.width <= 0 || command.height <= 0) // [변경]
			{
				continue;
			}

			IntRect renderRect = command.renderRect; 
			if (!renderRect.IsValid())              
			{
				renderRect = screenRect;
			}

			const int drawStartX = renderRect.x + command.localPosition.x;           
			const int drawStartY = renderRect.y + command.localPosition.y;    
			const int drawEndX = drawStartX + command.width - 1;              
			const int drawEndY = drawStartY + command.height - 1;     

			const int clipMinX = (renderRect.GetLeft() > screenRect.GetLeft())     
				? renderRect.GetLeft() : screenRect.GetLeft();
			const int clipMinY = (renderRect.GetTop() > screenRect.GetTop())
				? renderRect.GetTop() : screenRect.GetTop();
			const int clipMaxX = (renderRect.GetRight() < screenRect.GetRight())
				? renderRect.GetRight() : screenRect.GetRight();
			const int clipMaxY = (renderRect.GetBottom() < screenRect.GetBottom())
				? renderRect.GetBottom() : screenRect.GetBottom();

			if (drawEndY < clipMinY || drawStartY > clipMaxY)  continue;

			if (drawEndX < clipMinX || drawStartX > clipMaxX) continue;
			

			const int visibleStartY = (drawStartY < clipMinY) ? clipMinY : drawStartY;
			const int visibleEndY = (drawEndY > clipMaxY) ? clipMaxY : drawEndY;      
			const int visibleStartX = (drawStartX < clipMinX) ? clipMinX : drawStartX;
			const int visibleEndX = (drawEndX > clipMaxX) ? clipMaxX : drawEndX;     

			for (int y = visibleStartY; y <= visibleEndY; ++y)
			{
				const int sourceY = y - drawStartY;

				for (int x = visibleStartX; x <= visibleEndX; ++x)
				{
					const int sourceX = x - drawStartX; 

					const int sourceIndex = (sourceY * command.width) + sourceX;
					const char c = command.text[sourceIndex];
					

					if (command.spaceTransparent && c == ' ') 
					{
						continue;
					}

					const int destIndex = (y * screenSize.x) + x;

					if (frame->sortingOrderArray[destIndex] > command.sortingOrder)
					{
						continue;
					}

					frame->charInfoArray[destIndex].Char.UnicodeChar = c;
					frame->charInfoArray[destIndex].Attributes = (WORD)command.color;
					frame->sortingOrderArray[destIndex] = command.sortingOrder;
				}
			}
		}
		// Draw.
		GetCurrentBuffer()->Draw(frame->charInfoArray);

		// Buffer 교환.
		Present();

		// Clear RenderQueue.
		renderQueue.clear();
	}

	void Renderer::Submit(
		const char* text,
		const Vector2& position,
		Color color,
		int sortingOrder
	)
	{
		// Create Render Data => Add to Queue
		RenderCommand command = {};
		command.text = text;
		command.width = (text) ? static_cast<int>(strlen(text)) : 0;
		command.height = 1;
		command.localPosition = position;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.renderRect = GetScreenRect();
		command.spaceTransparent = false;

		renderQueue.emplace_back(command);
	}

	void Renderer::Submit(
		const char* text, 
		const Vector2& localPosition, 
		const IntRect& renderRect,
		Color color, 
		int sortingOrder)
	{
		RenderCommand command = {};
		command.text = text;
		command.width = (text) ? static_cast<int>(strlen(text)) : 0;
		command.height = 1;
		command.localPosition = localPosition;
		command.renderRect = renderRect;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.spaceTransparent = false;

		renderQueue.emplace_back(command);
	}

	//void Renderer::Submit(
	//	const wchar_t* text, 
	//	const Vector2& localPosition, 
	//	const IntRect& renderRect, 
	//	Color color, 
	//	int sortingOrder)
	//{
	//	RenderCommand command = {};
	//	command.wtext = text;
	//	command.width = (text) ? static_cast<int>(wcslen(text)) : 0;
	//	command.height = 1;
	//	command.localPosition = localPosition;
	//	command.renderRect = renderRect;
	//	command.color = color;
	//	command.sortingOrder = sortingOrder;
	//	command.spaceTransparent = false;
	//
	//	renderQueue.emplace_back(command);
	//}

	void Renderer::Submit(
		const char* image, 
		int width, int height, 
		const Vector2& localPosition, 
		const IntRect& renderRect, 
		Color color, 
		int sortingOrder, 
		bool spaceTransparent)
	{
		RenderCommand command = {};
		command.text = image;
		command.width = width;
		command.height = height;
		command.localPosition = localPosition;
		command.renderRect = renderRect;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.spaceTransparent = spaceTransparent;

		renderQueue.emplace_back(command);
	}

	void Renderer::Submit(
		const char* image, 
		int width, int height, 
		const Vector2& position, 
		Color color, int sortingOrder, 
		bool spaceTransparent)
	{
		RenderCommand command = {};
		command.text = image;
		command.width = width;
		command.height = height;
		command.localPosition = position;
		command.renderRect = GetScreenRect();
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.spaceTransparent = spaceTransparent;

		renderQueue.emplace_back(command);
	}

	void Renderer::Submit(
		std::shared_ptr<const AsciiArt> art, 
		const Vector2& position, 
		Color color, 
		int sortingOrder, 
		bool spaceTransparent)
	{
		RenderCommand command = {};
		command.artOwner = art;
		if (!art || !art->isValid()) return;

		command.text = art->pixels.data();
		command.width = art->width;
		command.height = art->height;
		command.renderRect = GetScreenRect();
		command.localPosition = position;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.spaceTransparent = spaceTransparent;

		renderQueue.emplace_back(command);
	}

	void Renderer::Submit(
		std::shared_ptr<const AsciiArt> art,
		const Vector2& localPosition, 
		const IntRect& renderRect, 
		Color color, 
		int sortingOrder, 
		bool spaceTransparent)
	{
		if (!art || !art->isValid())
		{
			return;
		}

		RenderCommand command = {};
		command.artOwner = art;
		command.text = art->pixels.data();
		command.width = art->width;
		command.height = art->height;
		command.localPosition = localPosition;
		command.renderRect = renderRect;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.spaceTransparent = spaceTransparent;

		renderQueue.emplace_back(command);
	}

	void Renderer::SubmitRectOutline(
		const IntRect& rect, 
		Color color, 
		int sortingOrder)
	{
		if (!rect.IsValid())
		{
			return;
		}

		for (int x = 0; x < rect.width; ++x)
		{
			Submit(borderChar, Vector2(rect.x + x, rect.y), color, sortingOrder);
			Submit(borderChar, Vector2(rect.x + x, rect.y + rect.height - 1), color, sortingOrder);
		}

		for (int y = 0; y < rect.height; ++y)
		{
			Submit(borderChar, Vector2(rect.x, rect.y + y), color, sortingOrder);
			Submit(borderChar, Vector2(rect.x + rect.width - 1, rect.y + y), color, sortingOrder);
		}
	}

	void Renderer::PresentImmediately()
	{
		Draw();
		GetCurrentBuffer()->Clear();
	}

	void Renderer::Present()
	{
		// Buffer 교환.
		SetConsoleActiveScreenBuffer(
			GetCurrentBuffer()->GetBuffer()
		);

		// index 교체.
		currentBufferIndex = 1 - currentBufferIndex;
	}

	ScreenBuffer* Renderer::GetCurrentBuffer()
	{
		return screenBuffers[currentBufferIndex];
	}
	IntRect Renderer::GetScreenRect() const
	{
		return IntRect(0, 0, screenSize.x, screenSize.y);
	}
}