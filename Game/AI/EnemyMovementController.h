#pragma once

#include "Math/Vector2.h"
#include "Util/Timer.h"

#include <vector>

using namespace Wanted;

class Enemy;
class IngameLevel;

class EnemyMovementController
{
public:
	void TickMovement(Enemy& owner, float deltaTime);

	// Path 시각화.
	void DrawPath(const Enemy& owner) const;

	// 재탐색 시 navigation 관련 cahced member들을 초기화.
	void ResetNavigationState();

private:
	// 경로 탐색 실패했을 경우에, 재탐색 시간을 벌기.
	bool CanRequestPath(float deltaTime);

	// 경로 재탐색.
	bool RequestPath(Enemy& owner, IngameLevel* level);

	void StartRepathCooldown();
	void StartBlockedRepathCooldown();

private:
	// Navigation 관련.
	bool hasCachedApproachPosition = false;
	Vector2 cachedApproachPosition = Vector2(-1, -1);

	// navigation 관련 cahcing.
	std::vector<Vector2> cachedPath;
	size_t nextPathIndex = 1;
	
	// 이동 속도 제어.
	float moveSpeed = 3.0f;	// 초당 이동하는 칸의 수.
	float moveStepAccumulator = 0.0f;

private:
	// 재탐색 주기 제어.
	Timer repathTimer;

	// 이미 Path가 있을 때, 주기적 재탐색 시간.
	static constexpr float REPATH_INTERVAL_SECONDS = 0.5f;

	// 길이 막혔을 때, 재시도할 시간.
	static constexpr float BLOCKED_REPATH_INTERVAL_SECONDS = 0.8f;
};

