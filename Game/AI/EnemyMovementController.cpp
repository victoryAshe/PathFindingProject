#include "EnemyMovementController.h"

#include "Actor/Enemy.h"
#include "Actor/Player.h"
#include "Level/IngameLevel.h"

void EnemyMovementController::TickMovement(Enemy& owner, float deltaTime)
{
	repathTimer.Tick(deltaTime);

	IngameLevel* const level = owner.GetIngameLevel();

	// Exception Handling.
	if (!level || !level->GetPlayerActor())
	{
		return;
	}

	// 경로가 없으면 재탐색
	// 경로가 이미 있어도, repathTimer timeOut이면 이동 중에 재탐색.
	const bool hasActivePath = 
		!cachedPath.empty() &&
		nextPathIndex < cachedPath.size();
	if (!hasActivePath)
	{
		if (!CanRequestPath(deltaTime)
			|| !RequestPath(owner, level)) return;
	}
	else if (repathTimer.IsTimeOut())
	{
		// 이동 중 주기적 재탐색.
		// 새 경로가 정상적으로 나왔으면 cachedPath가 교체됨
		// 실패시: RequestPath 내부에서 BlockedCooldown 호출.
		RequestPath(owner, level);
	}

	if (cachedPath.size() < 2 || nextPathIndex >= cachedPath.size())
	{
		moveStepAccumulator = 0.0f;
		return;
	}

	moveStepAccumulator += deltaTime;

	const float secondsPerTile = 1.0f / moveSpeed;
	if (moveStepAccumulator < secondsPerTile)
	{
		return;
	}

	moveStepAccumulator -= secondsPerTile;

	const Vector2 nextPosition = cachedPath[nextPathIndex];

	// 다음 지점에 다른 Actor가 있음.
	if (!level->CanMove(owner.GetPosition(), nextPosition, owner.GetSortingOrder()))
	{
		ResetNavigationState();

		StartBlockedRepathCooldown();
		return;
	}

	// 이동.
	owner.SetPosition(nextPosition);
	++nextPathIndex;

	// cache된 경로의 끝에 도착
	if (nextPathIndex >= cachedPath.size())
	{
		cachedPath.clear();
		nextPathIndex = 1;

		// 재탐색 타이머 시작.
		StartRepathCooldown();
	}
}

void EnemyMovementController::DrawPath(const Enemy& owner) const
{
	IngameLevel* const level = owner.GetIngameLevel();

	// Exception Handling.
	if (!level || cachedPath.empty() || nextPathIndex >= cachedPath.size())
	{
		return;
	}

	// 지금 Path의 출발점이면
	if (nextPathIndex <= 1)
	{
		//: 전부 그리고 return.
		level->DrawPath(cachedPath);
		return;
	}

	// Path의 지나온 지점을 빼고 넘겨서 그리기.
	std::vector<Vector2> remainingPath(
		cachedPath.begin() + static_cast<size_t>(nextPathIndex - 1),
		cachedPath.end()
	);

	level->DrawPath(remainingPath);
}

void EnemyMovementController::ResetNavigationState()
{
	hasCachedApproachPosition = false;
	cachedApproachPosition = Vector2(-1, -1);

	cachedPath.clear();
	nextPathIndex = 1;

	moveStepAccumulator = 0.0f;

	// 즉시 재탐색 가능 상태로 초기화
	repathTimer.SetTargetTime(0.0f);
	repathTimer.Reset();
}

bool EnemyMovementController::CanRequestPath(float deltaTime)
{
	return repathTimer.IsTimeOut();
}

bool EnemyMovementController::RequestPath(Enemy& owner, IngameLevel* level)
{
	if (!level) return false;

	Player* const player = level->GetPlayerActor();
	if (!player) return false;

	Vector2 selectedApproachPosition(-1, -1);

	std::vector<Vector2> newPath = level->FindPathToActor(
		owner.GetPosition(),
		player->GetPosition(),
		owner.GetAttackRange(),
		hasCachedApproachPosition ? &cachedApproachPosition : nullptr,
		&selectedApproachPosition
	);

	// blocked로 판별됨 => 잠시 대기하다가 경로 재탐색.
	if (newPath.empty())
	{
		ResetNavigationState();

		StartBlockedRepathCooldown();
		return false;
	}

	hasCachedApproachPosition = true;
	cachedApproachPosition = selectedApproachPosition;

	cachedPath = std::move(newPath);
	nextPathIndex = 1;

	// 경로 재탐색 타이머 시작.
	StartRepathCooldown();

	// 이미 도착.
	if (cachedPath.size() < 2)
	{
		moveStepAccumulator = 0.0f;
		cachedPath.clear();
		nextPathIndex = 1;
		return false;
	}

	// 이동 가능, 경로를 반환.
	return true;
}

void EnemyMovementController::StartRepathCooldown()
{
	repathTimer.SetTargetTime(REPATH_INTERVAL_SECONDS);
	repathTimer.Reset();
}

void EnemyMovementController::StartBlockedRepathCooldown()
{
	repathTimer.SetTargetTime(BLOCKED_REPATH_INTERVAL_SECONDS);
	repathTimer.Reset();
}
