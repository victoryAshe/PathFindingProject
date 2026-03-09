#pragma once


#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

constexpr int PlayerBulletSortingOrder = 15;

class PlayerBullet : public Actor
{
	RTTI_DECLARATIONS(PlayerBullet, Actor)

public:
	PlayerBullet(
		const Vector2& startPosition,
		const Vector2& moveDirection
	);
	~PlayerBullet();

	virtual void Tick(float deltaTime) override;

private:
	Vector2 moveDirection = Vector2::Zero;

	// 이동 속도.
	// 초당 몇칸 이동하는지를 나타냄.
	float moveSpeed = 10.0f;

	// 위치 갱신을 할 때 소수점 누적처리를 위한 변수.
	float preciseX = 0.0f;
	float preciseY = 0.0f;

};

