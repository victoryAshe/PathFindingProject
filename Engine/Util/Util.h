#pragma once

#include "Math/Vector2.h"
#include "Math/Color.h"

#include <time.h>

using namespace Wanted;

// Helper 기능 제공.
namespace Util 
{
	// Console cursor 위치 이동(설정) 하는 함수.
	inline void SetConsolePosition(const Vector2& position)
	{
		SetConsoleCursorPosition(
			GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<COORD>(position)
		);
	}

	// Console Text Color 설정 함수.
	inline void SetConsoleTextColor(Color color)
	{
		SetConsoleTextAttribute(
			GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<unsigned short>(color)
		);
	}

	// 커서 끄기.
	inline void TurnOffCursor()
	{
		CONSOLE_CURSOR_INFO info = {};
		GetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);

		info.bVisible = false;
		SetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);
	}

	// 커서 켜기.
	inline void TurnOnCursor()
	{
		CONSOLE_CURSOR_INFO info = {};
		GetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);

		info.bVisible = true;
		SetConsoleCursorInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&info
		);
	}

	inline void SetRandomSeed()
	{
		// 시간 값을 랜덤 종자값으로 설정.
		srand(static_cast<unsigned int>(time(nullptr)));
	}

	// 정수 난수 함수.
	inline int Random(int min, int max)
	{
		int diff = (max - min) + 1;
		return ((diff * rand()) / (RAND_MAX + 1)) + min;
	}

	// 부동소수점 난수 함수.
	inline float RandomRange(float min, float max)
	{
		// 0~1사이의 float 기반 랜덤 값.
		float random
			= static_cast<float>(rand())
			/ static_cast<float>(RAND_MAX);

		float diff = (max - min);
		return (random * diff) + min;
	}

	// 어떤 값을 두 수 사이로 고정할 때 사용하는 함수.
	template<typename T>
	T Clamp(T value, T min, T max)
	{
		if (value < min)
		{
			value = min;
		}
		else if (value > max)
		{
			value = max;
		}

		return value;
	}
}

// Memory 정리 함수.
template<typename T>
void SafeDelete(T*& t)
{
	if (t)
	{
		delete t;
		t = nullptr;
	}
}

// 배열 매모리 정리.
template<typename T>
void SafeDeleteArray(T*& t)
{
	if (t)
	{
		delete[] t;
		t = nullptr;
	}
}