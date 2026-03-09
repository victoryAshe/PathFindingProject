#pragma once

#include "Math/Vector2.h"

#include <Windows.h>

namespace Wanted
{
	// Double Buffering에 사용할 Console Output Handle을 관리하는 클래스.
	class ScreenBuffer
	{
	public:
		ScreenBuffer(const Vector2& screenSize);
		~ScreenBuffer();

		// Clear Console Buffer.
		void Clear();

		// Console에 2차원 글자 배열을 그릴 때 사용하는 함수.
		void Draw(CHAR_INFO* charInfo);

		// Buffer 반환 Getter.
		inline HANDLE GetBuffer() const { return buffer; }

	private:
		// Console Output Handle.
		HANDLE buffer = nullptr;

		// Screen Size.
		Vector2 screenSize;
	};
}


