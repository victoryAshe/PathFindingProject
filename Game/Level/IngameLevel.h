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
#include "Upgrade/PlayerUpgradeDefinition.h"
#include "UI/UpgradeSelectionUI.h"
// EnemySpawner.
#include "Spawner/EnemySpawner.h"

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

	// === Draw things related to Debugging ===
	void DrawPath(const std::vector<Vector2>& path);

	// === Random Enemy Spawn ===
	void SpawnEnemyAtRandomLocation();

	// === Navigation ===
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
	
	// === Player Death Process (Game Lose) ===
	void OnPlayerDead();
	bool IsPlayerDead() const { return isPlayerDead; }

	// === HP UI ===
	// Player가 호출할 HP UI 업데이트 함수.
	void RefreshPlayerHpUI();

	// === Upgrade ===
	// Enemy가 죽을 때 호출.
	void OnEnemyKilled();

	// === Getter ===
	const std::vector<Actor*>& GetActors() const { return actors; }
	
	Player* GetPlayerActor() const { return player; }

private:

	void MoveToMenu();
	void SpawnWallAt(const Vector2& wallPosition);
	
	// 충돌 판정 처리 함수.
	void ProcessCollisionPlayerBullet();

	// === Draw things related to Debugging ===
	void ToggleDrawPath();
	void UpdateFpsLabel(float deltaTime);

	// === Player Death Process (Game Lose) ===
	void UpdatePlayerDeathFlow(float deltaTime);
	void ReturnToMenuAfterPlayerDeath();

	// === Fire Cool down UI ===
	// UI 문자열 갱신.
	void UpdateFireCooldownUI();

	// 게이지 문자열 생성 함수.
	void BuildFireCooldownGaugeText(char* outBuffer, int bufferSize) const;

	// === Money UI ===
	void UpdateMoneyText();

	// === Random Enemy Spawn ===
	bool TryFindEnemySpawnLocation(
		Vector2& outSpawnLocation,
		float minSpawnDistanceFromPlayer,
		int maxSpawnSearchAttempts
	) const;

	bool IsEnemySpawnLocationValid(
		const Vector2& candidateLocation,
		float minSpawnDistanceFromPlayer
	) const;

	bool IsFarEnoughFromPlayer(
		const Vector2& candidateLocation,
		float minSpawnDistanceFromPlayer
	) const;

	bool IsOccupiedByBlockingActor(const Vector2& candidateLocation) const;
	Vector2 GenerateRandomWorldLocation() const;

	// === Upgrade Card ===
	void StartUpgradeSelection();
	void EndUpgradeSelection();
	void ProcessUpgradeInput();
	void SelectUpgrade(int index);
	void ApplyUpgrade(const PlayerUpgradeDefinition& selectedUpgrade);
	std::vector<PlayerUpgradeDefinition> GenerateUpgradeChoices(int count) const;

private:
	Player* player = nullptr;

	// === HP UI ===
	// Player HP UI를 LabelUI 2개로 분리(색을 다르게 처리하기 위해).
	LabelUI* playerHpTitleLabel = nullptr;
	LabelUI* playerHpValueLabel = nullptr;
	
	// Player HP 문자열.
	char playerHpValueText[32] = {};

	// === Fire Cool down UI ===
	// Player Fire cool down 표시용 UI.
	LabelUI* fireCooldownLabel = nullptr;

	// Fire 게이지 칸 수.
	static constexpr int fireCooldownGaugeCellCount = 6;

	// Player Fire cool down 출력 버퍼.
	char fireCooldownGaugeText[64] = {};

	// === Money UI ===
	LabelUI* moneyLabel = nullptr;
	char moneyText[64] = {};

	// === FPS UI ===
	LabelUI* fpsLabel = nullptr;
	char fpsText[32] = {};

	// === Upgrade Card UI ===
	// Enemy 처치 카운트
	int enemyKillCount = 0;

	bool isUpgradeSelectionActive = false;
	static constexpr int killsRequiredForUpgradeSelection = 5;

	std::vector<PlayerUpgradeDefinition> currentUpgradeChoices;
	UpgradeSelectionUI* upgradeSelectionUI = nullptr;


	// === Player Death Process (Game Lose) ===
	// 플레이어가 죽었는지 확인.
	bool isPlayerDead = false;

	// 플레이어가 죽은 위치 (Draw에서 처리하기 위해 Tick에서 저장).
	Vector2 playerDeadPosition;

	static constexpr float playerDeathWaitTime = 4.0f;

	// Player가 죽은 뒤, 4초 동안 멈춤을 처리해줄 timer.
	Timer playerDeathTimer = Timer(playerDeathWaitTime);


	// === Random Enemy Spawn ===
	// Enemy 랜덤 스폰 기본값
	static constexpr float enemySpawnMinDistanceFromPlayer = 8.0f;
	static constexpr int enemySpawnSearchAttemptCount = 32;

	// Enemy Spawn 주기 관리 객체
	EnemySpawner enemySpawner;

	// === Navigation ===
	// PathFinding을 위한 class. 
	Navigation::LevelNavigation levelNavigation;
	Navigation::NavigationController navigationController;

	bool IsPathDrawMode = false;
};

