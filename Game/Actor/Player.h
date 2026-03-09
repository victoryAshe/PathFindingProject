#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

constexpr int playerSortingOrder = 10;
class Player: public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	Player(const Vector2& position);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

public:
	int hp = 3;

private:

};