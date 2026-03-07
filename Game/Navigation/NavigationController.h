#pragma once

#include "Math/Vector2.h"
#include "Navigation/AStar.h"
#include <vector>

using namespace Wanted;

namespace Navigation
{

	class NavigationController
	{
	public:
		NavigationController() = default;
		~NavigationController() = default;

		// IngameLevel에서 받아올 grid.
		std::vector<Vector2> FindPath(
			const Vector2& start,
			const Vector2& goal,
			const std::vector<std::vector<int>>& navGrid
		);

	private:
		// AStar를 member로 가짐.
		// TODO: 다른 길찾기 알고리즘을 추가해, 설정에 따라 변경 적용.
		AStar aStar;
	};


}