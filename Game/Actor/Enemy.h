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
	int attackRange = 3;
	bool hasCahcedPosition = false;
	Vector2 cachedApproachPosition = Vector2(-1, -1);
};

