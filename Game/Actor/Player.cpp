#include "Player.h"

#include "Interface/ICanActorMove.h"
#include "Level/Level.h"
#include "Core/Input.h"


Player::Player(const Vector2& position)
	:super("P", position, Color::Red)
{
	sortingOrder = playerSortingOrder;
}

void Player::BeginPlay()
{
	super::BeginPlay();
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 인터페이스 확인.
	static ICanActorMove* canPlayerMoveInterface = nullptr;

	// 오너십 확인 (null 확인).
	if (!canPlayerMoveInterface && GetOwner())
	{
		// 인터페이스로 형변환.
		canPlayerMoveInterface = dynamic_cast<ICanActorMove*>(GetOwner());
	}

	// 이동.
	if (Input::Get().GetKeyDown(VK_RIGHT) && GetPosition().x < 20)
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

	if (Input::Get().GetKeyDown(VK_DOWN) && GetPosition().y < 15)
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

void Player::Draw()
{
	super::Draw();
}
