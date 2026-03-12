#pragma once
// 카드 업그레이드 타입 정의
enum class PlayerUpgradeType
{
	IncreaseHp,
    IncreaseAttackPower,
    ReduceFireCooldown
};

struct PlayerUpgradeDefinition
{
	PlayerUpgradeType type;
	const char* title = "";
	const char* description = "";
	float value = 0.0f;
};