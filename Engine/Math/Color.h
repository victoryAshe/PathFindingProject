#pragma once

#include "Common/Common.h"
#include <Windows.h>

//SetConsoleTextAttribute()

namespace Wanted 
{
	 // Console에 Text Color 등을 지정할 때 사용할
	// enum Color.
	enum class WANTED_API Color : unsigned short
	{
		Black = 0,
		Blue = FOREGROUND_BLUE,
		Green = FOREGROUND_GREEN,
		Red = FOREGROUND_RED,
		White = Blue | Green | Red, // 덧셈한 결과와 같음.
	};
}