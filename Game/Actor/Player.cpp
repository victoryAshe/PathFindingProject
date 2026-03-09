#include "Player.h"

#include "Interface/ICanActorMove.h"
#include "Level/Level.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"
#include "Actor/PlayerBullet.h"

ICanActorMove* Player::canPlayerMoveInterface = nullptr;

Player::Player(const Vector2& position)
	:super("P", position, Color::Red)
{
	sortingOrder = playerSortingOrder;
}

void Player::SetOwner(Level* newOwner)
{
	super::SetOwner(newOwner);
	canPlayerMoveInterface = dynamic_cast<ICanActorMove*>(GetOwner());
}

void Player::BeginPlay()
{
	super::BeginPlay();
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	timer.Tick(deltaTime);

	Move();
	ProcessFire();
}

void Player::Draw()
{
	super::Draw();
}

void Player::Move()
{

	// 오너십 확인 (null 확인).
	if (!GetOwner() || !canPlayerMoveInterface) return;

	// 이동.
	if (Input::Get().GetKeyDown(VK_RIGHT) && GetPosition().x < GameEngine::Get().GetWidth())
	{
		// 이동 가능 여부 판단.
		Vector2 newPosition(GetPosition().x + 1, GetPosition().y);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition, playerSortingOrder))
		{
			SetPosition(newPosition);
		}
	}

	if (Input::Get().GetKeyDown(VK_LEFT) && GetPosition().x > 0)
	{
		// 이동 가능 여부 판단.
		Vector2 newPosition(GetPosition().x - 1, GetPosition().y);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition, playerSortingOrder))
		{
			SetPosition(newPosition);
		}
	}

	if (Input::Get().GetKeyDown(VK_DOWN) && GetPosition().y < GameEngine::Get().GetHeight())
	{
		// 이동 가능 여부 판단.
		Vector2 newPosition(GetPosition().x, GetPosition().y + 1);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition, playerSortingOrder))
		{
			SetPosition(newPosition);
		}
	}

	if (Input::Get().GetKeyDown(VK_UP) && GetPosition().y > 0)
	{
		// 이동 가능 여부 판단.
		Vector2 newPosition(GetPosition().x, GetPosition().y - 1);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition, playerSortingOrder))
		{
			SetPosition(newPosition);
		}
	}
}

void Player::ProcessFire()
{
	// 발사 가능 여부 확인.
	if (!CanShoot())
	{
		return;
	}

	if (Input::Get().GetKeyDown(VK_W))
	{
		CreateBullet(Vector2(0, -1));
		return;
	}

	if (Input::Get().GetKeyDown(VK_A))
	{
		CreateBullet(Vector2(-1, 0));
		return;
	}
	if (Input::Get().GetKeyDown(VK_S))
	{
		CreateBullet(Vector2(0, 1));
		return;
	}
	if (Input::Get().GetKeyDown(VK_D))
	{
		CreateBullet(Vector2(1, 0));
		return;
	}

}

void Player::CreateBullet(const Vector2 moveDirection)
{
	timer.Reset();

	// 위치 설정.
	Vector2 bulletPosition(
		position.x + (width / 2),
		position.y
	);

	// 액터 생성.
	GetOwner()->AddNewActor(new PlayerBullet(bulletPosition, moveDirection));
}


bool Player::CanShoot() const
{
	// 경과 시간 확인.
	// 발사 간격보다 더 많이 흘렀는지.
	return timer.IsTimeOut();
}
