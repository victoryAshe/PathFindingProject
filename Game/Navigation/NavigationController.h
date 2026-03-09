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

		std::vector<Vector2> FindPath(
			const Vector2& start,
			const Vector2& goal,
			const std::vector<std::vector<int>>& navGrid
		);

		// 여러 목표 칸 중 실제 path 길이가 가장 짧은 경로를 반환
		// preferredGoal이 아직 유효하면 그 목표를 우선 유지해 시각적 오류 줄임.
		std::vector<Vector2> FindShortestPathToAnyGoal(
			const Vector2& start,
			const std::vector<Vector2>& candidateGoals,
			const std::vector<std::vector<int>>& navGrid,
			const Vector2* preferredGoal = nullptr,
			Vector2* outSelectedGoal = nullptr
		);


	private:
		// AStar를 member로 가짐.
		// TODO: 다른 길찾기 알고리즘을 추가해, 설정에 따라 변경 적용.
		AStar aStar;
	};


}