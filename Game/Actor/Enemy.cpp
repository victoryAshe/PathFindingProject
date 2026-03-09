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
		hasCahcedPosition ? &cachedApproachPosition : nullptr,
		&selectedApproachPosition
	);

	if (path.empty())
	{
		hasCahcedPosition = false;
		cachedApproachPosition = Vector2(-1, -1);
		return;
	}

	hasCahcedPosition = true;
	cachedApproachPosition = selectedApproachPosition;

	if (path.size() < 2)
	{
		return;
	}

	SetPosition(path[1]);
}
