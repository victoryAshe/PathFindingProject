#pragma once

#include "Node.h"
#include "Util/Util.h"

#include <vector>

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
		std::vector<Vector2> ConstructPath(Node* goalNode) const;

		// 휴리스틱(hCost) 계산 함수.
		float CalculateHeuristic(const Node* currentNode, const Node* goalNode);

		// 탐색하려는 위치(Node)가 grid 범위 안에 있는지 확인.
		// 유효성 검증.
		bool IsInRange(int x, int y, const std::vector<std::vector<int>>& grid);

		bool IsBlocked(int x, int y, const std::vector<std::vector<int>>& grid) const;

		// 이미 방문했는지 확인하는 함수.
		bool HasVisited(int x, int y, float gCost);

		// 탐색하려는 Node가 goalNode인지 확인하는 함수.
		bool IsDestination(const Node* node) const;

		// 대각선 이동이 가능한지 질의 (대각선의 가로세로가 막혀있는지 질의.)
		bool CanMoveDiagonal(
			const Vector2& current,
			int nextX,
			int nextY,
			const std::vector<std::vector<int>>& grid
		);

		// 매 호출 전 이전 탐색에서 남은 정보를 삭제.
		void ClearLists();

	private:
		// 열린 리스트: 탐색해야할 Node의 list.
		std::vector<Node*> openList;

		// 닫힌 리스트: 탐색이 끝난 Node의 list.
		std::vector<Node*> closedList;

		// 시작 Node.
		Node* startNode = nullptr;

		// 목표 Node.
		Node* goalNode = nullptr;

	};


}