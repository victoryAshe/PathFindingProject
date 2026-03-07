#include "AStar.h"

namespace Navigation
{

    AStar::AStar()
    {
    }

    // 계산에 필요한 자료구조를 전부 지역변수로 대체했으므로,
    // 메모리 관리를 하지 않아도 됨.
    AStar::~AStar()
    {
    }

    std::vector<Vector2> AStar::FindPath(
        const Vector2& start,
        const Vector2& goal,
        const std::vector<std::vector<int>>& grid
    )
    {
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

        const int height = static_cast<int>(grid.size());
        const int width = static_cast<int>(grid[0].size());

        //std::priority_queue<
        //    OpenNode,                   // Q안에 들어갈 자료 구조.
        //    std::vector<OpenNode>,      // Q를 담을 형태.
        //    CompareOpenNode             // Q의 정렬 방법.
        //> openQueue;
        std::priority_queue<OpenNode, std::vector<OpenNode>, CompareOpenNode> openQueue;

        // 해당 cell을 방문했는지 처리.
        // closed[y][x].
        std::vector<std::vector<bool>> closed(height, std::vector<bool>(width, false));

        // 해당 cell의 bestCost 저장 배열.
        // bestGCost[y][x].
        std::vector<std::vector<float>> bestGCost(height, std::vector<float>(width, FLT_MAX));

        // 각 node의 parent를 저장하는 배열
        // union_find 할 때 parent 배열 만들 때를 생각하면 됨
        // 다만, 여기서는 "path"가 중요하므로 부모를 통일하지 않는다.
        // parent[y][x].
        std::vector<std::vector<Vector2>> parent(
            height,
            std::vector<Vector2>(width, Vector2(-1, -1))
        );


        // 시작/목표 노드 저장.
        OpenNode startNode;
        startNode.position = start;
        startNode.gCost = 0.0f;
        startNode.hCost = CalculateHeuristic(start, goal);
        startNode.fCost = startNode.gCost + startNode.hCost;

        bestGCost[start.y][start.x] = 0.0f;
        openQueue.push(startNode);

        // 직선 비용 상수.
        const float straightCost = 1.0f;

        // 대각선 비용 상수.
        const float diagonalcost = 1.41421345f;

        // 비용 계산에 사용할 변수 값 설정.
        std::vector<Direction> directions =
        {
            // 하상우좌 이동.
            { 0, 1, straightCost}, { 0, -1, straightCost}, 
            {1, 0, straightCost},  {-1, 0, straightCost},
            
            // 대각선 이동.
            { 1, 1, diagonalcost },  { 1, -1, diagonalcost },
            { -1, 1, diagonalcost }, { -1, -1, diagonalcost },
        };

        // 탐색 가능한 위치가 있으면 계속 진행.
        while (!openQueue.empty())
        {
            OpenNode current = openQueue.top();
            openQueue.pop();

            const int curX = current.position.x;
            const int curY = current.position.y;

            // visit 처리된 cell이라면 넘김
            if (closed[curY][curX]) continue;

            // visit 처리.
            closed[curY][curX] = true;

            // Goal에 도착했다면:
            if (curX == goal.x && curY == goal.y)
            {
                // Path를 만들어 넘김
                return ConstructPath(start, goal, parent);
            }

            for (const Direction& direction : directions)
            {
                const int newX = curX + direction.x;
                const int newY = curY + direction.y;

                // 탐색 처리 안해도 되는 상황인지 검사.
                if (!IsInRange(newX, newY, grid)) continue;

                if (IsBlocked(newX, newY, grid)) continue;

                // 직선은 true, 대각선이면 움직일 수 있는 경우에만 true.
                if (!CanMoveDiagonal(current.position, newX, newY, grid)) continue;

                // visit 처리된 cell이라면 넘김.
                if (closed[newY][newX]) continue;

                const float newGCost = current.gCost + direction.cost;

                // 기존 저장된 경로보다 비용이 높다면 넘김.
                if (newGCost >= bestGCost[newY][newX]) continue;

                // bestGCost 갱신.
                bestGCost[newY][newX] = newGCost;
                parent[newY][newX] = current.position;

                OpenNode nextNode;
                nextNode.position = Vector2(newX, newY);
                nextNode.gCost = newGCost;
                nextNode.hCost = CalculateHeuristic(nextNode.position, goal);
                nextNode.fCost = nextNode.gCost + nextNode.hCost;

                openQueue.push(nextNode);
            }
        }

        // Goal까지 가는 경로를 찾지 못한 경우.
        return {};
    }

    std::vector<Vector2> AStar::ConstructPath(
        const Vector2& start,
        const Vector2& goal,
        const std::vector<std::vector<Vector2>>& parent
    ) const
    {
        std::vector<Vector2> path;

        // goal에서부터 역추적
        Vector2 current = goal;
        path.emplace_back(current);

        // current에 닿기 전까지.
        while (!(current.x == start.x && current.y == start.y))
        {
            const Vector2 prev = parent[current.y][current.x];

            // Exception Handling
            // 1. parent chain is broken 
            // || 2. start == goal.
            if (prev.x == -1 && prev.y == -1)
            {
                return {};
            }

            current = prev;
            path.emplace_back(current);
        }

        // 역추적한 것이므로, 재정렬.
        std::reverse(path.begin(), path.end());
        return path;
    }

    float AStar::CalculateHeuristic(const Vector2& current, const Vector2& goal) const
    {
        const float straightCost = 1.0f;
        const float diagonalCost = 1.41421356f;

        const int dx = std::abs(current.x - goal.x);
        const int dy = std::abs(current.y - goal.y);

        const int minDist = (dx < dy) ? dx : dy;
        const int maxDist = (dx > dy) ? dx : dy;

        return diagonalCost * minDist + straightCost * (maxDist - minDist);
    }

    bool AStar::IsInRange(int x, int y, const std::vector<std::vector<int>>& grid) const
    {
        return y >= 0 && y < static_cast<int>(grid.size())
            && x >= 0 && x < static_cast<int>(grid[0].size());
    }

    bool AStar::IsBlocked(int x, int y, const std::vector<std::vector<int>>& grid) const
    {
        return grid[y][x] == 1;
    }

    bool AStar::CanMoveDiagonal(
        const Vector2& current, 
        int nextX, int nextY, 
        const std::vector<std::vector<int>>& grid
    ) const
    {
        const int dx = nextX - current.x;
        const int dy = nextY - current.y;

        // 직선인 경우.
        if (std::abs(dx) != 1 || std::abs(dy) != 1) return true;

        // 대각선인 경우
        // 이동 가능한 것만 true로 반환하기 위한 계산.
        // : 가로와 세로로 막혀있는지.
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