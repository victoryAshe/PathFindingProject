#include "Enemy.h"

#include "Level/IngameLevel.h"

Enemy::Enemy(const Vector2& newPosition)
	: super("E", newPosition, Color::Blue)
{
	sortingOrder = EnemySortingOrder;
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	Move(deltaTime);
}

void Enemy::Move(float deltaTime)
{
	IngameLevel* level = dynamic_cast<IngameLevel*>(GetOwner());
	if (!level)
	{
		return;
	}


	Vector2 selectedApproachPosition(-1, -1);

	std::vector<Vector2> path = level->FindPathToActor(
		GetPosition(),
		level->GetPlayerActor()->GetPosition(),
		attackRange,
		hasCachedApproachPosition ? &cachedApproachPosition : nullptr,
		&selectedApproachPosition
	);

	if (path.empty())
	{
		hasCachedApproachPosition = false;
		cachedApproachPosition = Vector2(-1, -1);
		moveStepAccumulater = 0.0f;
		return;
	}

	hasCachedApproachPosition = true;
	cachedApproachPosition = selectedApproachPosition;

	// 이미 공격 가능 위치에 도달했으면 이동 X.
	if (path.size() < 2)
	{
		moveStepAccumulater = 0.0f;
		return;
	}

	// Tick은 계속 돌지만, 실제 이동은 일정 시간마다만 수행.
	moveStepAccumulater += deltaTime;

	const float secondsPerTile = 1.0f / moveSpeed;

	if (moveStepAccumulater < secondsPerTile)
	{
		return;
	}

	// 1칸 이동 처리.
	moveStepAccumulater -= secondsPerTile;
	SetPosition(path[1]);
}
