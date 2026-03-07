
#include "NavigationController.h"

namespace Navigation
{
	std::vector<Vector2> NavigationController::FindPath(
		const Vector2& start,
		const Vector2& goal,
		const std::vector<std::vector<int>>& navGrid
	)
	{
		std::vector<std::vector<int>> grid = navGrid;

		// Exception Handling.
		// 1. Grid is empty.
		if (grid.empty() || grid[0].empty()) return {};

		// 2. Start is out of range.
		if (start.y < 0 || start.y >= static_cast<int>(grid.size())
			|| start.x < 0 || start.x >= static_cast<int>(grid[0].size()))
		{
			return {};
		}
		
		// grid를 Level에서 받아오므로, start만 예외처리
		// start는 자기 자신이므로, 장애물에서 빼줌.
		grid[start.y][start.x] = 0;
		return aStar.FindPath(start, goal, grid);
	}
}
