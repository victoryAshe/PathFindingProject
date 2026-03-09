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

	// ==================== Frame ============================ //

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
	}

	Renderer::~Renderer()
	{
		SafeDelete(frame);
		for (ScreenBuffer*& buffer : screenBuffers)
		{
			if(buffer) SafeDeleteArray(buffer);
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

		// 전제 조건: Level의 모든 Actor가 Renderer에 Submit을 완료해야 함!
		// RenderQueue를 순회하면서 Frame 채우기.
		for (const RenderCommand& command : renderQueue)
		{
			// 화면에 그릴 text가 없으면 건너뜀.
			if (!command.text)
			{
				continue;
			}

			Vector2 cPos = command.position;
			// 세로 클리핑: 세로 기준 화면 벗어났는지 확인.
			const int startY = cPos.y;
			const int endY = cPos.y + command.height - 1;
			if (endY < 0 || startY >= screenSize.y) continue;

			const int visibleStartY = (startY < 0) ? 0 : startY;
			const int visibleEndY = (endY >= screenSize.y) ? (screenSize.y - 1) : endY;

			for (int y = visibleStartY; y <= visibleEndY; ++y)
			{
				// 가져올 char 좌표.
				const int sourceY = y - startY;

				// 가로 클리핑: 가로 기준 화면 벗어났는지 확인.
				const int startX = cPos.x;
				const int endX = cPos.x + command.width - 1;
				if (endX < 0 || startX >= screenSize.x) continue;

				const int visibleStartX = (startX < 0) ? 0 : startX;
				const int visibleEndX = (endX >= screenSize.x) ? (screenSize.x - 1) : endX;

				for (int x = visibleStartX; x <= visibleEndX; ++x)
				{
					// 가져올 char 좌표.
					const int sourceX = x - startX;

					const int sourceIndex = (sourceY * command.width) + sourceX;
					const char c = command.text[sourceIndex];

					// 공백은 투명 처리해서 기존 pixel 유지.
					if (command.spaceeTransparent && c == ' ') continue;

					const int destIndex = (y * screenSize.x) + x;

					// sortingOrder 기반 합성
					// 현재 text의 sortingOrder가 더 크면(:앞 layer면) 덮어쓰기 x.
					if (frame->sortingOrderArray[destIndex] > command.sortingOrder)
						continue;

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
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		command.spaceeTransparent = false;

		renderQueue.emplace_back(command);
	}

	void Renderer::Submit(
		const char* image, 
		int width, int height, 
		const Vector2& position, 
		Color color, 
		int sortingOrder, 
		bool spaceTransparent)
	{
		RenderCommand command = {};
		command.text = image;
		command.width = width;
		command.height = height;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.spaceeTransparent = spaceTransparent;

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

		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;
		command.spaceeTransparent = spaceTransparent;

		renderQueue.emplace_back(command);
	}

	void Renderer::PresentImmediately()
	{
		Draw();
		GetCurrentBuffer()->Draw(frame->charInfoArray);
		Present();
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
}