#pragma once

#include "Level/Level.h"

#include "Actor/Player.h"
#include "Util/Timer.h"

// Navigation
#include "Interface/ICanActorMove.h"
#include "Navigation/NavigationController.h"
#include "Navigation/LevelNavigation.h"

// UI
// TODO: UI Manager로 빼기.
#include "UI/LabelUI.h"


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

	void DrawPath(const std::vector<Vector2>& path);

	// ICanActorMove Interface를 받아서 override.
	virtual bool CanMove(
		const Wanted::Vector2& currentPositon,
		const Wanted::Vector2& nextPosition,
		int sortingOrder
	)override;

	// 실제 플레이 가능한 월드 영역의 시작 screen 좌표.
	Vector2 GetWorldScreenOrigin() const;

	// 실제 플레이 가능한 월드 영역 크기.
	Vector2 GetPlayableWorldSize() const;

	// screen 좌표가 UI Rect에 속하는지 검사.
	bool IsInsideUIRectScreen(const Vector2& screenPosition) const;

	bool IsInsideWorldScreenRect(const Vector2& screenPosition) const;

	//  world local 좌표가 플레이 가능한 영역인지 검사
	bool IsInsideWorldBounds(const Vector2& worldLocalPosition) const;

	// screen position => world local position 변환.
	bool TryConvertScreenToWorldPosition(
		const Vector2& screenPosition,
		Vector2& outWorldLocalPosition
	) const;
	
	// Player 죽음 처리.
	void OnPlayerDead();
	bool IsPlayerDead() const { return isPlayerDead; }

	void RefreshPlayerHpUI();

	// Getter.
	const std::vector<Actor*>& GetActors() const { return actors; }
	
	Player* GetPlayerActor() const { return player; }

private:

	void MoveToMenu();
	void SpawnEnemyAt(const Vector2& spawnPosition);
	void SpawnWallAt(const Vector2& wallPosition);

	// 충돌 판정 처리 함수.
	void ProcessCollisionPlayerBullet();

	// Player 죽음 처리.
	void UpdatePlayerDeathFlow(float deltaTime);
	void ReturnToMenuAfterPlayerDeath();


private:
	Player* player = nullptr;

	// Player HP UI를 LabelUI 2개로 분리(색을 다르게 처리하기 위해).
	LabelUI* playerHpTitleLabel = nullptr;
	LabelUI* playerHpValueLabel = nullptr;
	
	// Player HP 문자열 버퍼.
	char playerHpValueBuffer[32] = {};

	// 플레이어가 죽었는지 확인.
	bool isPlayerDead = false;

	// 플레이어가 죽은 위치 (Draw에서 처리하기 위해 Tick에서 저장).
	Vector2 playerDeadPosition;

	static constexpr float playerDeathWaitTime = 4.0f;

	// Player가 죽은 뒤, 4초 동안 멈춤을 처리해줄 timer.
	Timer playerDeathTimer = Timer(playerDeathWaitTime);


	// PathFinding을 위한 class. 
	Navigation::LevelNavigation levelNavigation;
	Navigation::NavigationController navigationController;
};

