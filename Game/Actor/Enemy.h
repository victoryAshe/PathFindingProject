#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

const int EnemySortingOrder = 5;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor);

public:
	Enemy(const Vector2& newPosition);

protected:
	virtual void Tick(float deltaTime) override;

private:
	void Move(float deltaTime);

private:
	bool canAttack = false;
	int attackRange = 3;

	bool hasCachedApproachPosition = false;
	Vector2 cachedApproachPosition = Vector2(-1, -1);

	// 이동 속도 제어.
	float moveSpeed = 3.0f;
	float moveStepAccumulater = 0.0f;
};

