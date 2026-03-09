#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

constexpr int EnemySortingOrder = 5;

class IngameLevel;

class Enemy : public Actor
{
	RTTI_DECLARATIONS(Enemy, Actor);

public:
	explicit Enemy(const Vector2& newPosition);

	// 데미지 받았을 때 처리할 함수.
	void OnDamaged();


protected:
	virtual void Tick(float deltaTime) override;

	virtual void SetOwner(Level* newOwner) override;

private:
	// 경로 탐색 실패했을 경우에, 재탐색 시간을 벌기.
	bool CanRequestPath(float deltaTime);

	// 재탐색 시 navigation 관련 cahced member들을 초기화.
	void ResetNavigationState();

	// 재탐색.
	bool RequestPath(IngameLevel* level);

	// 이동.
	void Move(float deltaTime);

	// Path 시각화.
	void DrawCurrentPath() const;

private:
	IngameLevel* owningIngameLevel = nullptr;

	int hp = 0;
	
	// Attack.
	bool canAttack = false;
	int attackRange = 1;
	
	// Navigation 관련.
	bool hasCachedApproachPosition = false;
	Vector2 cachedApproachPosition = Vector2(-1, -1);

	// navigation 관련 cahcing.
	std::vector<Vector2> cachedPath;
	size_t nextPathIndex = 1;


	// 재탐색 주기 제어.
	// 이미 path가 있는 enemy는 0.25초마다 새 path 재탐색 허용.
	float repathIntervalSeconds = 0.25f;
	float repathCooldownSeconds = 0.0f;

	// 길이 막히면 재시도할 시간
	float blockedRepathIntervalSeconds = 0.8f;
	

	// 이동 속도 제어.
	float moveSpeed = 3.0f;		// 초당 이동하는 칸의 수.
	float moveStepAccumulator = 0.0f;
};

