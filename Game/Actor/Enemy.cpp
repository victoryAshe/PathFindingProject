#include "Enemy.h"

#include "Level/IngameLevel.h"
#include "Actor/EnemyDestroyEffect.h"
#include "Util/Util.h"

Enemy::Enemy(const Vector2& newPosition)
	: super("E", newPosition, Color::Red)
{
	sortingOrder = EnemySortingOrder;

	hp = Util::Random(1, 5);
	
	ChangeImage(std::to_string(hp).c_str());
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	stateMachine.Tick(*this, deltaTime);
	DrawMovementPath();
}

void Enemy::OnDamaged(int damage)
{
	if (damage <= 0) return;

	hp = max(hp - damage, 0);
	ChangeImage(std::to_string(hp).c_str());

	if (hp == 0)
	{
		// 액터 제거.
		Destroy();

		// 이펙트 생성 (재생을 위해).
		GetOwner()->AddNewActor(new EnemyDestroyEffect(position));

		// Player 돈 추가
		owningIngameLevel->GetPlayerActor()->ChangeBalance(10);

		return;
	}	
}

bool Enemy::CanAttackPlayer() const
{
	if (!owningIngameLevel) return false;

	// Player가 죽은 상태면: 더 공격 못하도록.
	if (owningIngameLevel->IsPlayerDead()) return false;
 
	Player* player = owningIngameLevel->GetPlayerActor();

	if (!player) return false;
	
	// IngameLevel에 질의하면 
	// : LevelNavigation이 Ingame 규칙에 따라 처리한 뒤 결과 return.
	return owningIngameLevel->CanAttackFromPosition(
		GetPosition(),
		player->GetPosition(),
		attackRange
	);
}


void Enemy::Attack()
{
	if (!owningIngameLevel) return;

	// Player가 이미 죽었으면 Attack X.
	if (owningIngameLevel->IsPlayerDead()) return;

	Player* player = owningIngameLevel->GetPlayerActor();
	if (!player) return;

	if (!CanAttackPlayer()) return;

	player->OnDamaged(attackPower);
}

void Enemy::UpdateMovement(float deltaTime)
{
	movementController.TickMovement(*this, deltaTime);
}

void Enemy::ResetMovement()
{
	movementController.ResetNavigationState();
}

void Enemy::DrawMovementPath() const
{
	movementController.DrawPath(*this);
}

void Enemy::SetOwner(Level* newOwner)
{
	super::SetOwner(newOwner);
	owningIngameLevel = dynamic_cast<IngameLevel*>(newOwner);
}