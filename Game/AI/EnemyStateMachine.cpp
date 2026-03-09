#include "EnemyStateMachine.h"

#include "Actor/Enemy.h"
#include "AI/EnemyMovementController.h"

void EnemyStateMachine::Tick(Enemy& owner, float deltaTime)
{
	stateTimer.Tick(deltaTime);

	switch (currentState)
	{
	case State::Move:
		TickMoveState(owner, deltaTime);
		break;
		
	case State::PreAttack:
		TickPreAttackState(owner, deltaTime);
		break;

	case State::Attack:
		TickAttackState(owner, deltaTime);
		break;

	case State::Idle:
		TickIdleState(owner, deltaTime);
		break;
	}
}

void EnemyStateMachine::ChangeState(Enemy& owner, State newState)
{
	currentState = newState;

	switch (newState)
	{
	case State::Move:
		stateTimer.Reset();
		break;

	case State::PreAttack:
		stateTimer.Restart(PRE_ATTACK_DELAY_SECONDS);
		break;

	case State::Attack:
		// 공격은 상태 진입 시점에 실행
		owner.Attack();

		stateTimer.Restart(ATTACK_DURATION_SECONDS);
		break;

	case State::Idle:
		stateTimer.Restart(IDLE_DURATION_SECONDS);
		break;
	}
}

void EnemyStateMachine::TickMoveState(Enemy& owner, float deltaTime)
{
	owner.UpdateMovement(deltaTime);

	if (owner.CanAttackPlayer())
	{
		ChangeState(owner, State::PreAttack);
	}
}

void EnemyStateMachine::TickPreAttackState(Enemy& owner, float deltaTime)
{
	// 기존 목표 도착지점에 도착했는데, AttackRange에 Player가 없음
	if (!owner.CanAttackPlayer())
	{
		// 다시 이동 상태로.
		ChangeState(owner, State::Move);
		return;
	}

	if (stateTimer.IsTimeOut())
	{
		ChangeState(owner, State::Attack);
	}
}

void EnemyStateMachine::TickAttackState(Enemy& owner, float deltaTime)
{
	if (stateTimer.IsTimeOut())
	{
		ChangeState(owner, State::Idle);
	}
}

void EnemyStateMachine::TickIdleState(Enemy& owner, float deltaTime)
{
	if (!stateTimer.IsTimeOut()) return;

	// Idle 시간이 다 지났는데, AttackRange에 Player가 있음.
	if (owner.CanAttackPlayer())
	{
		// 한 번 더 공격
		ChangeState(owner, State::Attack);
	}
	else
	{
		// Idle 시간 이후, AttackRange 안에 Player가 없음.
		// 다시 이동.
		owner.ResetMovement();
		ChangeState(owner, State::Move);
	}
}
