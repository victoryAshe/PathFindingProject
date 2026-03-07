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
			const Vector2& targetPosition
		) const;

		Vector2 SelectBestApproachPosition(
			const Vector2& start,
			const std::vector<Vector2>& candidatePositions
		) const;


	private:
		bool IsTileWalkable(
			const std::vector<std::vector<int>>& navGrid,
			const Vector2& tilePosition
		) const;

	private:
		const IngameLevel* level = nullptr;
	};

}
