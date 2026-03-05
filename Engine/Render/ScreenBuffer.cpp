#include "ScreenBuffer.h"

#include <iostream>

namespace Wanted
{
	ScreenBuffer::ScreenBuffer(const Vector2& screenSize)
		:screenSize(screenSize)
	{
		// Create Console Output.
		buffer = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,		// Access Mode
			FILE_SHARE_READ | FILE_SHARE_WRITE, // Share Mode
			nullptr,							// Security Attributes
			CONSOLE_TEXTMODE_BUFFER,			// Draw Flag
			nullptr								// Screen Buffer Data.
		);
		// => if fails, return INVALID_HANDLE_VALUE

		// Exception Handling.
		if (buffer == INVALID_HANDLE_VALUE)
		{
			// pop-up 띄울 때 사용하는 함수! 
			MessageBoxA(nullptr,							// nulptr을 넣으면 새 팝업 생성.
				"ScreenBuffer - Failed to create buffer.",	// 내용
				"Buffer creation error",					// 제목
				MB_OK										// MessageBoxType. 여러 개가 있으니 써보자!
			);

			__debugbreak();
		}

		// Set Size of Console Window
		SMALL_RECT rect;
		rect.Left = 0;
		rect.Top = 0;
		rect.Right = static_cast<short>(screenSize.x - 1);
		rect.Bottom = static_cast<short>(screenSize.y - 1);

		// Null-check.
		if (!SetConsoleWindowInfo(buffer, true, &rect))
		{
			//DWORD errorCode = GetLastError();	// 마지막 에러 코드를 받을 수 있다!
			std::cerr << "Failed to set console window info\n";
			__debugbreak();
		}

		// After Create: Set Buffer Size
		// +Null - check
		if (!SetConsoleScreenBufferSize(buffer, screenSize))
		{
			std::cerr << "Failed to set console buffer size.\n";
			__debugbreak();
		}

		// Turn Off cursor.
		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(buffer, &info);

		info.bVisible = false;
		SetConsoleCursorInfo(buffer, &info);
	}

	ScreenBuffer::~ScreenBuffer()
	{
		// Free Buffer.
		if (&buffer)
		{
			CloseHandle(&buffer);
		}
	}

	void ScreenBuffer::Clear()
	{
		// Console Buffer에 있는 Screen Clear.
		// Graphics -> Clear -> 한 색상(또는 값)으로 덮어쓰기!

		// 실제로 화면을 지우고 난 뒤에, 
		// 몇 글자를 섰는지 반환받는 데 사용.
		DWORD writtenCount = 0;

		FillConsoleOutputCharacterA(
			buffer,							// Handle ConsoleOutput
			' ',							// 덮어쓸 값
			screenSize.x * screenSize.y,	// 덮어쓸 영역
			Vector2::Zero,					// 시작 포인트
			&writtenCount					// 얼마나 덮어씌웠는지 저장받을 값.
		);
	}

	void ScreenBuffer::Draw(CHAR_INFO* charInfo)
	{
		// 설정할 버퍼의 크기.
		SMALL_RECT writeRegion = {};
		writeRegion.Left = 0;
		writeRegion.Top = 0;
		writeRegion.Right = static_cast<short>(screenSize.x - 1);
		writeRegion.Bottom = static_cast<short>(screenSize.y - 1);

		// Buffer에 전달 받은 글자 배열 설정.
		WriteConsoleOutputA(
			buffer,
			charInfo,
			screenSize,
			Vector2::Zero,			// 시작 포인트.
			&writeRegion
		);
	}
}
