#pragma once

#include "Util/Timer.h"

class Enemy;

class EnemyStateMachine
{
public:
	enum class State
	{
		Move,
		PreAttack, // Arrived.
		Attack,
		Idle
	};

public:
	void Tick(Enemy& owner, float deltaTime);

	// Getter.
	State GetState() const { return currentState; }

private:
	void ChangeState(Enemy& owner, State newState);

	void TickMoveState(Enemy& owner, float deltaTime);
	void TickPreAttackState(Enemy& owner, float deltaTime);
	void TickAttackState(Enemy& owner, float deltaTime);
	void TickIdleState(Enemy& owner, float deltaTime);

private:
	State currentState = State::Move;

	// 각 State마다 시간을 관리해줄 timer memeber.
	Timer stateTimer;

private:

	static constexpr float PRE_ATTACK_DELAY_SECONDS = 0.15f;
	static constexpr float ATTACK_DURATION_SECONDS = 0.2f;
	static constexpr float IDLE_DURATION_SECONDS = 0.8f;
};