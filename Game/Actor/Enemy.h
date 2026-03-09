#pragma once

#include "Actor/Actor.h"

#include "AI/EnemyMovementController.h"
#include "AI/EnemyStateMachine.h"

using namespace Wanted;

class IngameLevel;

constexpr int EnemySortingOrder = 5;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor);

private:

public:
	explicit Enemy(const Vector2& newPosition);

	// ========== Damaged ========== 
	// 데미지 받았을 때 처리할 함수.
	void OnDamaged(int damage);


	// ========== Attack ========== 
	// Player가 AttackRange 안에 있는지를 반환.ㄴ
	bool CanAttackPlayer() const;

	// Attack 실행.
	void Attack();


	// ========== Update Member ========== 
	void UpdateMovement(float deltaTime);
	void ResetMovement();
	void DrawMovementPath() const;


	// ========== Getter ========== 
	IngameLevel* GetIngameLevel() const { return owningIngameLevel; }
	int GetAttackRange() const { return attackRange; }
	
protected:
	virtual void Tick(float deltaTime) override;

	virtual void SetOwner(Level* newOwner) override;

private:
	IngameLevel* owningIngameLevel = nullptr;

	int hp = 0;
	
	// Attack.
	int attackPower = 1;
	int attackRange = 1;
	

	// State 제어.
	EnemyStateMachine stateMachine;

	// Movement 제어: Navigation까지.
	EnemyMovementController movementController;
};

