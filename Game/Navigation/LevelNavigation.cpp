#include "LevelNavigation.h"

#include "Level/IngameLevel.h"
#include "Actor/Actor.h"
#include "Engine/GameEngine.h"
#include <cassert>

namespace Navigation
{
	LevelNavigation::LevelNavigation(const IngameLevel* level)
		: level(level)
	{
	}

	std::vector<std::vector<int>> LevelNavigation::BuildNavGrid() const
	{
		// Exception Handling.
		assert(level != nullptr);

		const Vector2 screenSize = GameEngine::Get().GetScreenSize();

		std::vector<std::vector<int>> navGrid(
			screenSize.y,
			std::vector<int>(screenSize.x, 0)
		);

		const std::vector<Actor*>& actors = level->GetActors();

		for (Actor* const actor : actors)
		{
			if (!actor || actor->DestroyRequested())
			{
				continue;
			}

			const Vector2 actorPosition = actor->GetPosition();

			if (actorPosition.x < 0 || actorPosition.x >= screenSize.x ||
				actorPosition.y < 0 || actorPosition.y >= screenSize.y)
			{
				continue;
			}

			// 현재 프로젝트 규칙:
			// sortingOrder > 0 인 Actor가 있으면 점유 타일로 취급.
			if (actor->GetSortingOrder() > 0)
			{
				navGrid[actorPosition.y][actorPosition.x] = 1;
			}
		}

		return navGrid;
	}

	bool LevelNavigation::IsTileWalkable(
		const std::vector<std::vector<int>>& navGrid,
		const Vector2& tilePosition
	) const
	{
		// Exception Handling.
		if (navGrid.empty() || navGrid[0].empty())
		{
			return false;
		}

		if (tilePosition.y < 0 || tilePosition.y >= static_cast<int>(navGrid.size()) ||
			tilePosition.x < 0 || tilePosition.x >= static_cast<int>(navGrid[0].size()))
		{
			return false;
		}

		return navGrid[tilePosition.y][tilePosition.x] == 0;
	}

	std::vector<Vector2> LevelNavigation::FindApproachPositions(
		const std::vector<std::vector<int>>& navGrid,
		const Vector2& targetPosition
	) const
	{
		// 8방향.
		static const Vector2 directions[8] =
		{
			// 상하좌우.
			Vector2(0, 1),
			Vector2(0, -1),
			Vector2(1, 0),
			Vector2(-1, 0),

			// 대각선.
			Vector2(1, 1),
			Vector2(1, -1),
			Vector2(-1, 1),
			Vector2(-1, -1)
		};

		std::vector<Vector2> candidatePositions;

		for (const Vector2& direction : directions)
		{
			const Vector2 candidatePosition = targetPosition + direction;

			if (IsTileWalkable(navGrid, candidatePosition))
			{
				candidatePositions.emplace_back(candidatePosition);
			}
		}

		return candidatePositions;
	}

	Vector2 LevelNavigation::SelectBestApproachPosition(
		const Vector2& start,
		const std::vector<Vector2>& candidatePositions
	) const
	{
		if (candidatePositions.empty())
		{
			return Vector2(-1, -1);
		}

		Vector2 bestPosition = candidatePositions[0];
		float bestDistanceSq =
			static_cast<float>(
				(bestPosition.x - start.x) * (bestPosition.x - start.x) +
				(bestPosition.y - start.y) * (bestPosition.y - start.y));

		for (size_t i = 1; i < candidatePositions.size(); ++i)
		{
			const Vector2& currentPosition = candidatePositions[i];

			const float currentDistanceSq =
				static_cast<float>(
					(currentPosition.x - start.x) * (currentPosition.x - start.x) +
					(currentPosition.y - start.y) * (currentPosition.y - start.y));

			if (currentDistanceSq < bestDistanceSq)
			{
				bestPosition = currentPosition;
				bestDistanceSq = currentDistanceSq;
			}
		}

		return bestPosition;
	}
}