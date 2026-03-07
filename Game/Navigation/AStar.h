#pragma once

#include "Node.h"
#include "Util/Util.h"

#include <vector>
#include <queue>

namespace Navigation
{
	class AStar
	{
		// struct for process direction.
		struct Direction
		{
			int x = 0;
			int y = 0;

			// cost to move.
			float cost = 0.0f;
		};

		// 탐색할 node의 struct.
		struct OpenNode
		{
			Vector2 position;
			float gCost = 0.0f;
			float hCost = 0.0f;
			float fCost = 0.0f;
		};

		struct CompareOpenNode
		{
			bool operator()(const OpenNode& a, const OpenNode& b) const
			{
				if (a.fCost == b.fCost)
				{
					return a.hCost > b.hCost;
				}

				return a.fCost > b.fCost;
			}
		};


	public:
		AStar();
		~AStar();

		// PathFinding.
		std::vector<Vector2> FindPath
		(
			const Vector2& start,
			const Vector2& goal,
			const std::vector<std::vector<int>>& grid		// 2D array to search.
		);

	private:

		// 탐색을 완료한 후에, 최적 경로를 반환하는 함수.
		// parentNode를 따라가면서 역추적.
		std::vector<Vector2> ConstructPath(
			const Vector2& start,
			const Vector2& goal,
			const std::vector<std::vector<Vector2>>& parent
		) const;

		// 휴리스틱(hCost) 계산 함수.
		float CalculateHeuristic(const Vector2& current, const Vector2& goal) const;

		// 탐색하려는 위치(Node)가 grid 범위 안에 있는지 확인.
		// 유효성 검증.
		bool IsInRange(int x, int y, const std::vector<std::vector<int>>& grid) const;

		bool IsBlocked(int x, int y, const std::vector<std::vector<int>>& grid) const;

		// 대각선 이동이 가능한지 질의 (대각선의 가로세로가 막혀있는지 질의.)
		bool CanMoveDiagonal(
			const Vector2& current,
			int nextX,
			int nextY,
			const std::vector<std::vector<int>>& grid
		) const;

	};


}