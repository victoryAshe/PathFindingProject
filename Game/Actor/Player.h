#pragma once

#include "Actor/Actor.h"
#include "Util/Timer.h"


using namespace Wanted;

class ICanActorMove;

constexpr int playerSortingOrder = 10;
class Player: public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	Player(const Vector2& position);

protected:
	virtual void SetOwner(Level* newOwner) override;

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
	

private:
	void Move();
	void ProcessFire();

	void CreateBullet(const Vector2 moveDirection);

	// 발사 가능여부 확인 함수.
	bool CanShoot() const;


public:
	int hp = 3;

private:
	static ICanActorMove* canPlayerMoveInterface;

	// 타이머 변수.
	Timer timer;
	// 연사 시간 간격.
	float fireInterval = 0.3f;
};