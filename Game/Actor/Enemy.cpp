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

	// 테스트용 목표 지점
	Vector2 goal(0, 0);

	std::vector<Vector2> path = level->FindPath(GetPosition(), goal);

	if (path.size() >= 2)
	{
		SetPosition(path[1]);
	}
}
