#pragma once

#include "Math/Vector2.h"
#include "Math/Color.h"
#include "Assets/AsciiArt.h"

#include <vector>
#include <string>
#include <memory>

namespace Wanted
{
	// forward declaration: Console Buffer를 관리하는 class
	class ScreenBuffer;

	// Dubble buffering을 지원하는 Renderer class.
	class WANTED_API Renderer
	{
		// Frame Struct - 2차원 글자 배열의 항목이 될 struct.
		struct Frame
		{
			Frame(int bufferCount);
			~Frame();

			// Clear Screen.
			void Clear(const Vector2& screenSize);

			// CHAR_INFO: 글자 값과 글자의 색상을 갖는 type.
			CHAR_INFO* charInfoArray = nullptr;

			// 그리기 우선순위 배열.
			int* sortingOrderArray = nullptr;
		};

		// Rendereing할 Data.
		struct RenderCommand
		{
			// text가 가리키는 buffer의 수명을 잡음
			std::shared_ptr<const AsciiArt> artOwner;

			// 화면에 보여줄 문자열 값.
			const char* text = nullptr;
			const wchar_t* wtext = nullptr;

			int width = 0;
			int height = 0;

			// 좌표.
			Vector2 position;

			// Color.
			Color color = Color::White;

			// Render priority.
			int sortingOrder = 0;

			bool spaceeTransparent = true;
		};

	public:
		Renderer(const Vector2& screenSize);
		~Renderer();

		// Render.
		void Draw();

		// One-Line-Actor용 Submit.
		void Submit(
			const char* text,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0
		);

		// AsciiArt용 Submit
		void Submit(
			const char* image,
			int width,
			int height,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0,
			bool spaceTransparent = true
		);

		// AsciiArt의 shared_ptr 제출용 submit.
		void Submit(
			std::shared_ptr<const AsciiArt> art,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0,
			bool spaceTransparent = true
		);

		// 즉시 화면에 표시할 때 사용.
		void PresentImmediately();

		// Singleton 접근 변수.
		static Renderer& Get();

	private:

		// Clear Scrren.
		void Clear();


		// Double Buffering을 활용해 Activated Buffer를 교환하는 함수.
		void Present();

		// Getter: 현재 사용할 Buffer를 return.
		ScreenBuffer* GetCurrentBuffer();

	private:
		// ScreenSize.
		Vector2 screenSize;

		// Frame struct to manage.
		Frame* frame = nullptr;

		// DoubleBuffer array: 2개라서 double-buffer라고 함.
		ScreenBuffer* screenBuffers[2] = {}; 

		// Currently activated buffer index.
		int currentBufferIndex = 0;

		// RenderQueue (Level의 모든 RenderCommand를 모아두는 배열).
		std::vector<RenderCommand> renderQueue;

		// Singleton 구현을 위한 static 변수.
		static Renderer* instance;
	};
}