#include "Player.h"

Player::Player(const Vector2& position)
	:super("P", position, Color::Red)
{
	sortingOrder = 10;
}

void Player::BeginPlay()
{
	super::BeginPlay();
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void Player::Draw()
{
	super::Draw();
}
