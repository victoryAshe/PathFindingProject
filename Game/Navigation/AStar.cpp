#include "AStar.h"

namespace Navigation
{

    AStar::AStar()
    {
    }

    AStar::~AStar()
    {
        // 메모리 정리.
        ClearLists();
    }


    void AStar::ClearLists()
    {
        for (Node* node : openList)
        {
            SafeDelete(node);
        }
        openList.clear();

        for (Node* node : closedList)
        {
            SafeDelete(node);
        }
        closedList.clear();
    }

    std::vector<Vector2> AStar::FindPath(
        const Vector2& start,
        const Vector2& goal,
        const std::vector<std::vector<int>>& grid
    )
    {
        ClearLists();

        // Exception Handling.
        if (grid.empty() || grid[0].empty())
        {
            // 빈 경로 반환(오류).
            return {};
        }

        // 탐색을 안해도 되는 상황인지 검사.
        if (!IsInRange(start.x, start.y, grid) || !IsInRange(goal.x, goal.y, grid))
        {
            return {};
        }

        if (IsBlocked(start.x, start.y, grid) || IsBlocked(goal.x, goal.y, grid))
        {
            return {};
        }

        // 시작/목표 노드 저장.
        startNode = new Node(start);
        goalNode = new Node(goal);

        startNode->hCost = CalculateHeuristic(startNode, goalNode);
        startNode->fCost = startNode->gCost + startNode->hCost;

        // startNode를 openList에 추가 및 탐색 시작.
        openList.emplace_back(startNode);

        // 대각선 비용 상수.
        const float diagonalcost = 1.41421345f;

        // 비용 계산에 사용할 변수 값 설정.
        std::vector<Direction> directions =
        {
            // 하상우좌 이동.
            { 0, 1, 1.0f }, { 0, -1, 1.0f }, {1, 0, 1.0f }, {-1, 0, 1.0f},
            // 대각선 이동.
            { 1, 1, diagonalcost }, { 1, -1, diagonalcost },
            { -1, 1, diagonalcost }, { -1, -1, diagonalcost },
        };

        // 탐색 가능한 위치가 있으면 계속 진행.
        while (!openList.empty())
        {
            // currentNode를 현재 openList 중 fCost가 가장 낮은 노드로 설정
            // 그를 위한 선형 탐색.
            Node* currentNode = openList[0];

            for (Node* const node : openList)
            {
                if (node->fCost < currentNode->fCost)
                {
                    currentNode = node;
                }
                // tie breaker: fCost가 동점일 때 처리
                // 처리 안해도 탐색은 잘 되지만,
                // 해주는 것보다 성능이 떨어짐
                else if(node->fCost == currentNode->fCost 
                    && node->hCost < currentNode->hCost)
                {
                    currentNode = node;
                }
            }

            // 지금 openList에서 뽑은 currentNode가 목표Node라면,
            if (IsDestination(currentNode))
            {
                // 경로 반환 후 종료.
                return ConstructPath(currentNode);
            }

            // 방문 처리를 위해 openList에서 제거.
            for (auto it = openList.begin(); it != openList.end(); ++it)
            {
                if (*it == currentNode)
                {
                    openList.erase(it);
                    break;
                }
            }

            closedList.emplace_back(currentNode);

            // 이웃 노드 방문(탐색)
            for (const Direction& direction : directions)
            {
                // 다음에 이동할 위치(이웃 Node 위치).
                int newX = currentNode->position.x + direction.x;
                int newY = currentNode->position.y + direction.y;

                // 탐색을 안해도 되는 상황인지 검사.
                // 1. grid 범위 내에 없는 위치
                if (!IsInRange(newX, newY, grid))
                {
                    continue;
                }

                // 2. 장애물이 존재하는 위치.
                if (IsBlocked(newX, newY, grid))
                {
                    continue;
                }

                // 3. 대각선이면 이동 가능한지 여부. (직선이면 true)
                if (!CanMoveDiagonal(currentNode->position, newX, newY, grid))
                {
                    continue;
                }

                // currentNode를 기준으로 새 gCost 계산.
                const float newGCost = currentNode->gCost + direction.cost;

                // 갈 수는 있지만, 이미 방문한 곳인지 확인.
                // 이 때, 해당 newGCost를 넣어주는 이유는 더 적은 cost의 노드로 갱신해주기 위함.
                if (HasVisited(newX, newY, newGCost))
                {
                    continue;
                }

                // 방문을 위한 이웃 노드 생성.
                Node* neighbourNode = new Node(Vector2(newX, newY), currentNode);
                // 비용 계산.
                neighbourNode->gCost = newGCost;
                neighbourNode->hCost = CalculateHeuristic(neighbourNode, goalNode);
                neighbourNode->fCost = neighbourNode->gCost + neighbourNode->hCost;

                // neighbourNode가 openList에 있는지 확인.
                Node* openListNode = nullptr;
                for (Node* const node : openList)
                {
                    // 위치만 비교해서 openList에 넣을지 여부 확인.
                    if (*node == *neighbourNode)
                    {
                        openListNode = node;
                        break;
                    }
                }

                // 위에서 neighbourNode와 같은 위치의 node가 openList에 있었으면:
                if (openListNode)
                {
                    // neighbourNode가 더 좋은 비용일 때만 바꿔줌
                    if (neighbourNode->gCost < openListNode->gCost)
                    {
                        // 부모 노드 업데이트.
                        openListNode->parentNode = neighbourNode->parentNode;
                        // 비용 업데이트.
                        openListNode->gCost = neighbourNode->gCost;
                        openListNode->hCost = neighbourNode->hCost;
                        openListNode->fCost = neighbourNode->fCost;
                    }

                    SafeDelete(neighbourNode);
                    continue;
                }
                openList.emplace_back(neighbourNode);
 
            }
        }

        return {};
    }

    std::vector<Vector2> AStar::ConstructPath(Node* goalNode) const
    {
        std::vector<Vector2> path;

        // 역추적하면서 path에 채우기.
        Node* current = goalNode;
        while (current)
        {
            path.emplace_back(current->position);
            current = current->parentNode;
        }

        // 이렇게 얻은 결과는 순서가 거꾸로.
        // 그래서 거꾸로 다시 정렬이 필요함.
        std::reverse(path.begin(), path.end());
        return path;
    }

    float AStar::CalculateHeuristic(const Node* currentNode, const Node* goalNode)
    {
        const float straightCost = 1.0f;
        const float diagonalCost = 1.41421356f;

        const int dx = std::abs(currentNode->position.x - goalNode->position.x);
        const int dy = std::abs(currentNode->position.y - goalNode->position.y);

        const int minDist = (dx < dy) ? dx : dy;
        const int maxDist = (dx > dy) ? dx : dy;

        return diagonalCost * minDist + straightCost * (maxDist - minDist);
    }

    bool AStar::IsInRange(int x, int y, const std::vector<std::vector<int>>& grid)
    {
        return y >= 0 && y < static_cast<int>(grid.size())
            && x >= 0 && x < static_cast<int>(grid[0].size());
    }

    bool AStar::IsBlocked(int x, int y, const std::vector<std::vector<int>>& grid) const
    {
        return grid[y][x] == 1;
    }

    bool AStar::HasVisited(int x, int y, float gCost)
    {
        // 닫힌 리스트에 이미 같은 위치가 있고, 
        // 비용이 더 낮으면 방문했다고 판단.
        for (Node* const node : closedList)
        {
            if (node->position.x == x && node->position.y == y
                && node->gCost <= gCost)
            {
                return true;
            }
        }

        return false;
    }

    bool AStar::IsDestination(const Node* node) const
    {
        return node
            && goalNode
            && node->position.x == goalNode->position.x
            && node->position.y == goalNode->position.y;
    }
    bool AStar::CanMoveDiagonal(const Vector2& current, int nextX, int nextY, const std::vector<std::vector<int>>& grid)
    {
        const int dx = nextX - current.x;
        const int dy = nextY - current.y;

        if (std::abs(dx) != 1 || std::abs(dy) != 1)
        {
            return true;
        }

        const int sideX = current.x + dx;
        const int sideY = current.y;

        const int downX = current.x;
        const int downY = current.y + dy;

        if (!IsInRange(sideX, sideY, grid) || !IsInRange(downX, downY, grid))
        {
            return false;
        }

        if (IsBlocked(sideX, sideY, grid))
        {
            return false;
        }

        if (IsBlocked(downX, downY, grid))
        {
            return false;
        }

        return true;
    }
}