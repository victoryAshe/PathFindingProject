#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor);

public:
	Enemy(const Vector2& newPosition);

};

