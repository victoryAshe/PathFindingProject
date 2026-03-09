
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
		
		// grid를 Level에서 받아오므로, start만 예외 처리
		// start는 자기 자신이므로, 이동 가능으로 처리.
		grid[start.y][start.x] = 0;
		return aStar.FindPath(start, goal, grid);
	}

	std::vector<Vector2> NavigationController::FindShortestPathToAnyGoal(
		const Vector2& start, 
		const std::vector<Vector2>& candidateGoals, 
		const std::vector<std::vector<int>>& navGrid, 
		const Vector2* preferredGoal, 
		Vector2* outSelectedGoal)
	{
		if (outSelectedGoal)
		{
			*outSelectedGoal = Vector2(-1, -1);
		}

		if (candidateGoals.empty())
		{
			return {};
		}

		// 1. 이전 프레임에서 쓰던 접근 목표가 아직 후보에 남아 있으면 우선 유지.
		if (preferredGoal)
		{
			for (const Vector2& candidateGoal : candidateGoals)
			{
				if (candidateGoal == *preferredGoal)
				{
					std::vector<Vector2> preferredPath =
						FindPath(start, candidateGoal, navGrid);

					if (!preferredPath.empty())
					{
						if (outSelectedGoal)
						{
							*outSelectedGoal = candidateGoal;
						}

						return preferredPath;
					}

					break;
				}
			}
		}

		// 2. 유효한 후보들 중 실제 path 길이가 가장 짧은 경로를 선택.
		// 동률이면 candidateGoals의 순서를 유지
		std::vector<Vector2> bestPath;
		Vector2 bestGoal(-1, -1);

		for (const Vector2& candidateGoal : candidateGoals)
		{
			std::vector<Vector2> candidatePath =
				FindPath(start, candidateGoal, navGrid);

			if (candidatePath.empty())
			{
				continue;
			}

			if (bestPath.empty() || candidatePath.size() < bestPath.size())
			{
				bestPath = std::move(candidatePath);
				bestGoal = candidateGoal;
			}
		}

		if (outSelectedGoal)
		{
			*outSelectedGoal = bestGoal;
		}

		return bestPath;
	
	}
}
