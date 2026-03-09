#pragma once

#include "Math/Vector2.h"

#include <vector>

class IngameLevel;

using namespace Wanted;

namespace Navigation
{
	class LevelNavigation
	{
	public:
		explicit LevelNavigation(const IngameLevel* level);
		~LevelNavigation() = default;

	public:
		std::vector<std::vector<int>> BuildNavGrid() const;


		std::vector<Vector2> FindApproachPositions(
			const std::vector<std::vector<int>>& navGrid,
			const Vector2& targetPosition,
			const int atkRange = 1
		) const;

		// 후보 정렬 전용.
		// 최종 path 선택은 NavigationController가 담당.
		std::vector<Vector2> SortApproachPositionsByHeuristic(
			const Vector2& start,
			const std::vector<Vector2>& candidatePositions
		) const;

		bool IsWithinAttackRange(
			const Vector2& currentPosition,
			const Vector2& targetPosition,
			int attackRange
		) const;

		bool CanAttackFromPosition(
			const Vector2& attackPosition,
			const Vector2& targetPosition,
			int attackRange
		) const;

	private:
		bool IsTileWalkable(
			const std::vector<std::vector<int>>& navGrid,
			const Vector2& tilePosition
		) const;

		bool IsWallAtPosition(const Vector2& tilePosition) const;

		bool HasClearLineToTarget(
			const Vector2& attackPosition,
			const Vector2& targetPosition
		) const;


	private:
		const IngameLevel* level = nullptr;
	};

}
