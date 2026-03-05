#pragma once

#include "Math/Vector2.h"

#include <Windows.h>

namespace Wanted
{
	/*
	* ~회사마다 있는 주석 틀~
	* 작성자	: ㅇㅇㅇ
	* 날짜	: 2026.02.03
	* 내용	: Double Buffering에 사용할 Console Output Handle을 관리하는 클래스. 
	*/
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


