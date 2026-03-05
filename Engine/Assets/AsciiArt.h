#pragma once

#include <vector>

// File에서 Load한 Ascii Art를 width*height 형태로 보관
// => Actor에 바로 넘겨서 생성할 수 있도록 함.

namespace Wanted
{
	struct AsciiArt
	{
		int width = 0;
		int height = 0;
		std::vector<char> pixels; // size = width * height;

		bool isValid() const 
		{ 
			return (width > 0) && (height > 0) 
				&& (static_cast<int>(pixels.size()) == width * height); 
		}
	};
}