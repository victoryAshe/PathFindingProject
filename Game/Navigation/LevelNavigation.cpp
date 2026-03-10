#include "LevelNavigation.h"

#include "Level/IngameLevel.h"
#include "Actor/Actor.h"
#include "Engine/GameEngine.h"
#include "Actor/Wall.h"

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

		// 전체 screenSize가 아니라 실제 플레이 월드 크기 사용.
		const Vector2 playableWorldSize = level->GetPlayableWorldSize();

		std::vector<std::vector<int>> navGrid(
			playableWorldSize.y,
			std::vector<int>(playableWorldSize.x, 0)
		);

		// --------------------------------------------------
		//				Actor 기반 장애물 처리
		// --------------------------------------------------

		const std::vector<Actor*>& actors = level->GetActors();

		for (Actor* const actor : actors)
		{
			if (!actor || actor->DestroyRequested())
			{
				continue;
			}

			const Vector2 actorPosition = actor->GetPosition();

			// UI Rect를 제외한 world local 범위만 nav grid에 반영.
			if (!level->IsInsideWorldBounds(actorPosition))
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

	bool LevelNavigation::IsWallAtPosition(const Vector2& tilePosition) const
	{
		if (!level)
		{
			return false;
		}

		const std::vector<Actor*>& actors = level->GetActors();

		for (Actor* const actor : actors)
		{
			if (!actor || actor->DestroyRequested())
			{
				continue;
			}

			if (actor->GetPosition() != tilePosition)
			{
				continue;
			}

			if (actor->GetSortingOrder() == wallSortingOrder)
			{
				return true;
			}
		}

		return false;
	}

	bool LevelNavigation::HasClearLineToTarget(const Vector2& attackPosition, const Vector2& targetPosition) const
	{
		const int deltaX = targetPosition.x - attackPosition.x;
		const int deltaY = targetPosition.y - attackPosition.y;

		const int stepX = (deltaX == 0) ? 0 : (deltaX > 0 ? 1 : -1);
		const int stepY = (deltaY == 0) ? 0 : (deltaY > 0 ? 1 : -1);

		// attackPosition과 targetPosition 사이의 중간 칸들만 검사
		Vector2 current = attackPosition + Vector2(stepX, stepY);

		while (current != targetPosition)
		{
			// world local 범위를 벗어나면 line check 실패 처리.
			if (!level->IsInsideWorldBounds(current))
			{
				return false;
			}

			if (IsWallAtPosition(current))
			{
				return false;
			}

			current = current + Vector2(stepX, stepY);
		}

		return true;
	}

	std::vector<Vector2> LevelNavigation::FindApproachPositions(
		const std::vector<std::vector<int>>& navGrid,
		const Vector2& targetPosition,
		const int atkRange
	) const
	{
		// 8방향.
		static const Vector2 directions[8] =
		{
			// 상하좌우.
			Vector2(0, atkRange),
			Vector2(0, -atkRange),
			Vector2(atkRange, 0),
			Vector2(-atkRange, 0),

			// 대각선.
			Vector2(atkRange, atkRange),
			Vector2(atkRange, -atkRange),
			Vector2(-atkRange, atkRange),
			Vector2(-atkRange, -atkRange)
		};

		std::vector<Vector2> candidatePositions;

		for (const Vector2& direction : directions)
		{
			const Vector2 candidatePosition = targetPosition + direction;

			if (!IsTileWalkable(navGrid, candidatePosition))
			{
				continue;
			}

			if (!HasClearLineToTarget(candidatePosition, targetPosition))
			{
				continue;
			}

			candidatePositions.emplace_back(candidatePosition);
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


	bool LevelNavigation::CanAttackFromPosition(
		const Vector2& attackPosition,
		const Vector2& targetPosition,
		int attackRange
	) const
	{
		// Target이 AttackRange 안에 없으면 false.
		if (!IsWithinAttackRange(attackPosition, targetPosition, attackRange))
		{
			return false;
		}

		// AttackActor <=> TargetActor 사이에 벽이 있는지 검사.
		return HasClearLineToTarget(attackPosition, targetPosition);
	}
}