#pragma once

#include "Level/Level.h"

#include "Actor/Player.h"

// Navigation
#include "Interface/ICanActorMove.h"
#include "Navigation/NavigationController.h"
#include "Navigation/LevelNavigation.h"


using namespace Wanted;


class IngameLevel 
	: public Level,
	public ICanActorMove
{
	RTTI_DECLARATIONS(IngameLevel, Level)

	
public:
	IngameLevel();
	~IngameLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	// 경로 질의.
	std::vector<Vector2> FindPath(
		const Vector2& start,
		const Vector2& goal
	);

	// 특정 Actor 추적 경로 질의
	// : 해당 Actor의 인접한 칸을 선택해 경로 반환.
	std::vector<Vector2> FindPathToActor(
		const Vector2& startPosition,
		const Vector2& targetPosition,
		const int attackRange,
		const Vector2* preferredApproachPosition,
		Vector2* outSelectedApproachPosition
	);

	// Enemy가 Attack할 수 있는지 질의.
	// 실제 게임 규칙은 LevelNavigation에서 처리하도록 넘김.
	// (좌표/벽 판정 등).
	bool CanAttackFromPosition(
		const Vector2& attackPosition,
		const Vector2& targetPosition,
		int attackRange
	) const;

	void DrawPath(std::vector<Vector2> const path);

	// ICanActorMove Interface를 받아서 override.
	virtual bool CanMove(
		const Wanted::Vector2& curPositon,
		const Wanted::Vector2& nextPosition,
		int sortingOrder
	)override;
	
	// Getter.
	const std::vector<Actor*>& GetActors() const { return actors; }
	
	Player* GetPlayerActor() const { return player; }


private:

	void MoveToMenu();
	void CreateEnemy(const Vector2 position);
	void CreateWall(const Vector2 position);

	// 충돌 판정 처리 함수.
	void ProcessCollisionPlayerBulletAndEnemy();
	void ProcessCollisionPlayerAndEnemy();

	void ShowPlayerUI();

private:
	Player* player = nullptr;

	// player 체력 출력용.
	// TODO: 별도의 UI로 분리할 수 있으면 할 것.
	char playerUIstring[128] = {};

	// 플레이어가 죽었는지 확인.
	bool isPlayerDead = false;

	// 플레이어가 죽은 위치 (Draw에서 처리하기 위해 Tick에서 저장).
	Vector2 playerDeadPosition;

	// PathFinding을 위한 class. 
	Navigation::LevelNavigation levelNavigation;
	Navigation::NavigationController navigationController;
};

