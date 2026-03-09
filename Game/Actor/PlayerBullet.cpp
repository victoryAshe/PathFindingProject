#include "PlayerBullet.h"
#include "Engine/GameEngine.h"

PlayerBullet::PlayerBullet(
	const Vector2& startPosition,
	const Vector2& moveDirection)
	: super("@", startPosition, Color::Yellow),
	moveDirection(moveDirection),
	preciseX(static_cast<float>(startPosition.x)),
	preciseY(static_cast<float>(startPosition.y))
{
	sortingOrder = PlayerBulletSortingOrder;
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	preciseX += static_cast<float>(moveDirection.x) * moveSpeed * deltaTime;
	preciseY += static_cast<float>(moveDirection.y) * moveSpeed * deltaTime;

	Vector2 nextPosition(
		static_cast<int>(preciseX),
		static_cast<int>(preciseY)
	);

	const Vector2 screenSize = GameEngine::Get().GetScreenSize();

	if (nextPosition.x < 0 || nextPosition.x >= screenSize.x ||
		nextPosition.y < 0 || nextPosition.y >= screenSize.y)
	{
		Destroy();
		return;
	}

	SetPosition(nextPosition);
}
