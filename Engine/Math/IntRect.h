#pragma once

namespace Wanted
{
	struct IntRect
	{
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		IntRect() = default;

		IntRect(int x, int y, int width, int height)
			: x(x), y(y), width(width), height(height)
		{
		}

		int GetLeft() const { return x; }
		int GetTop() const { return y; }
		int GetRight() const { return x + width - 1; }
		int GetBottom() const { return y + height - 1; }

		bool IsValid() const
		{
			return width > 0 && height > 0;
		}

		bool Contains(int px, int py) const
		{
			return px >= GetLeft() && px <= GetRight()
				&& py >= GetTop() && py <= GetBottom();
		}
	};
} 