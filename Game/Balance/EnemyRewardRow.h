#pragma once

#include "EnemyType.h"

struct EnemyRewardRow
{
	EnemyType enemyType = EnemyType::Normal;
	int enemyLevel = 1;

	int expReward = 0;
	int goldReward = 0;
	int scoreReward = 0;
};