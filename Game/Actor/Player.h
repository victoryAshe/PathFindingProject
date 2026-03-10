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

	// 피격 시 Event.
	void OnDamaged(int damage);

	// Getter: 생존 상태.
	bool IsDead() const { return isDead; }

	// Getters for Fire timer.
	
	// 발사 가능 여부.
	bool IsFireReady() const;

	// 다음 발사까지 남은 시간.
	float GetRemainingFireCooldown() const;

	// Fire Cooldown 진행 비율.
	float GetFireCooldownProgressRatio() const;

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

	// 사망 처리.
	void HandleDeath();

public:
	int attackPower = 1;
	int hp = 3;

private:
	static ICanActorMove* canPlayerMoveInterface;

	// 타이머 변수.
	Timer timer;

	// 연사 시간 간격.
	float fireInterval = 0.35f;

	bool isDead = false;
};