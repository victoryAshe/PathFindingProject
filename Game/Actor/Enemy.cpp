#include "Enemy.h"

Enemy::Enemy(const Vector2& newPosition)
	: super("E", newPosition, Color::Blue)
{
	sortingOrder = 5;
}
