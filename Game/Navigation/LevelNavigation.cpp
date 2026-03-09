#include "LevelNavigation.h"

#include "Level/IngameLevel.h"
#include "Actor/Actor.h"
#include "Engine/GameEngine.h"

#include <cassert>
#include <algorithm>

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
		const Vector2& targetPosition,
		const int atkDist 
	) const
	{
		// 8방향.
		static const Vector2 directions[8] =
		{
			// 상하좌우.
			Vector2(0, atkDist),
			Vector2(0, -atkDist),
			Vector2(atkDist, 0),
			Vector2(-atkDist, 0),

			// 대각선.
			Vector2(atkDist, atkDist),
			Vector2(atkDist, -atkDist),
			Vector2(-atkDist, atkDist),
			Vector2(-atkDist, -atkDist)
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

	std::vector<Vector2> LevelNavigation::SortApproachPositionsByHeuristic(
		const Vector2& startPosition,
		const std::vector<Vector2>& approachCandidates
	) const
	{
		std::vector<Vector2> sortedApproachCandidates = approachCandidates;

		std::sort(
			sortedApproachCandidates.begin(),
			sortedApproachCandidates.end(),
			[&startPosition](const Vector2& candidateA, const Vector2& candidateB)
			{
				const int distanceToCandidateASq =
					(candidateA.x - startPosition.x) * (candidateA.x - startPosition.x) +
					(candidateA.y - startPosition.y) * (candidateA.y - startPosition.y);

				const int distanceToCandidateBSq =
					(candidateB.x - startPosition.x) * (candidateB.x - startPosition.x) +
					(candidateB.y - startPosition.y) * (candidateB.y - startPosition.y);

				// startPosition 기준으로 더 가까운 접근 칸을 우선
				if (distanceToCandidateASq != distanceToCandidateBSq)
				{
					return distanceToCandidateASq < distanceToCandidateBSq;
				}

				// tie breaker
				if (candidateA.y != candidateB.y)
				{
					return candidateA.y < candidateB.y;
				}

				return candidateA.x < candidateB.x;
			}
		);

		return sortedApproachCandidates;
	}

	bool LevelNavigation::IsWithinAttackRange(
		const Vector2& currentPosition, 
		const Vector2& targetPosition, 
		int attackRange) const
	{
		const int deltaX = std::abs(currentPosition.x - targetPosition.x);
		const int deltaY = std::abs(currentPosition.y - targetPosition.y);

		// 현재 A*가 대각 이동을 허용하므로 체비쇼프 거리 기준이 가장 자연스럽다.
		return max(deltaX, deltaY) <= attackRange;
	}

}