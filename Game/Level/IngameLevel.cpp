#include "IngameLevel.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"
#include "Render/Renderer.h"

// Actor
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/Wall.h"
#include "Actor/PlayerBullet.h"

// UI 문자열 처리를 위해.
#include <cstring>

// Engine FPS 가져오기 위해.
#include "Engine/Engine.h"

IngameLevel::IngameLevel()
	: levelNavigation(this),
	enemySpawner(this)
{
	const IntRect& worldRect = GetWorldRect();

	const Vector2 initialPlayerPosition(
		worldRect.width / 2,
		worldRect.height / 2
	);

	player = new Player(initialPlayerPosition);
	AddNewActor(player);

	// PlayerHP LabelUI 생성.
	playerHpTitleLabel = new LabelUI(
		"PlayerHP:",
		Vector2(3, 3),
		Color::White,
		10000
	);

	playerHpValueLabel = new LabelUI(
		"",
		Vector2(12, 3), // 타이틀 오른쪽에 배치
		Color::Red,
		10000
	);

	// Player Fire cool down LabelUI 생성.
	fireCooldownLabel = new LabelUI(
		"Fire: [######]",
		Vector2(3, 5),
		Color::White,
		10000
	);

	// Player Money LabelUI 생성.
	moneyLabel = new LabelUI(
		"Money: ",
		Vector2(3, 7),
		Color::Green,
		10000
	);

	// FPS LabelUI 생성
	fpsLabel = new LabelUI(
		"FPS: 0",
		Vector2(2, 1),
		Color::Yellow,
		100000
	);
	// WorldRect에 그려지도록.
	fpsLabel->SetRenderInWorldRect(true);
	AddNewUIElement(fpsLabel);

	AddNewUIElement(playerHpTitleLabel);
	AddNewUIElement(playerHpValueLabel);
	AddNewUIElement(fireCooldownLabel);
	AddNewUIElement(moneyLabel);

	// 카드 업그레이드 UI 생성
	upgradeSelectionUI = new UpgradeSelectionUI();
	upgradeSelectionUI->SetVisible(false);
	AddNewUIElement(upgradeSelectionUI);

	// 초기 HP text 반영.
	RefreshPlayerHpUI();


	// Enemy Spawn 주기 시작
	enemySpawner.SetSpawnInterval(3.0f);
	enemySpawner.Start();
}

IngameLevel::~IngameLevel()
{
}

void IngameLevel::Tick(float deltaTime)
{

	// UI 문자열을 매 프레임 최신 상태로 갱신.
	UpdateFireCooldownUI();
	UpdateMoneyText();

	ToggleDrawPath();


	// 카드 선택 중에는 월드 진행을 멈추고 입력만 받는다.
	if (isUpgradeSelectionActive)
	{
		ProcessUpgradeInput();
		return;
	}

	super::Tick(deltaTime);

	// super::Tick이 지나가고 deltaTime 계산이 된 뒤에, FPS Label 갱신/
	UpdateFpsLabel(deltaTime);

	// Player 사망 시, 대기 흐름만 처리.
	if (isPlayerDead)
	{
		UpdatePlayerDeathFlow(deltaTime);
		return;
	}
	
	// ESC키 처리.
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// 메뉴 활성화.
		MoveToMenu();
		return;
	}

	//Enemy 자동 스폰 Tick
	enemySpawner.Tick(deltaTime);

	ProcessCollisionPlayerBullet();

	// 마우스 왼쪽 input되면 Wall Spawn.
	if (Input::Get().GetMouseButtonDown(0))
	{
		const Vector2 mouseScreenPosition = Input::Get().MousePosition();
		Vector2 worldLocalPosition;
		if (IsInsideUIRectScreen(mouseScreenPosition)) return;

		if (TryConvertScreenToWorldPosition(mouseScreenPosition, worldLocalPosition))
		{
			SpawnWallAt(worldLocalPosition);
		}
		return;
	} 
}


void IngameLevel::Draw()
{
	super::Draw();
	
	if (isPlayerDead)
	{
		Renderer::Get().Submit(
			"! Game Over !",
			Vector2(10, 3),
			Color::Red,
			200
		);
	}
}

std::vector<Vector2> IngameLevel::FindPath(
	const Vector2& start, 
	const Vector2& goal
)
{
	std::vector<std::vector<int>> navGrid = levelNavigation.BuildNavGrid();
	std::vector<Vector2> path = navigationController.FindPath(start, goal, navGrid);
	return path;
}

std::vector<Vector2> IngameLevel::FindPathToActor(
	const Vector2& startPosition,
	const Vector2& targetPosition,
	const int attackRange,
	const Vector2* preferredApproachPosition,
	Vector2* outSelectedApproachPosition
)
{
	// 이미 공격 가능한 거리 안이면 이동할 필요가 없다.
	if (levelNavigation.IsWithinAttackRange(startPosition, targetPosition, attackRange))
	{
		if (outSelectedApproachPosition)
		{
			*outSelectedApproachPosition = startPosition;
		}

		return { startPosition };
	}

	std::vector<std::vector<int>> navGrid = levelNavigation.BuildNavGrid();

	const std::vector<Vector2> approachCandidates =
		levelNavigation.FindApproachPositions(navGrid, targetPosition, attackRange);

	if (approachCandidates.empty())
	{
		return {};
	}

	const std::vector<Vector2> orderedApproachCandidates =
		levelNavigation.SortApproachPositionsByHeuristic(startPosition, approachCandidates);

	std::vector<Vector2> path =
		navigationController.FindShortestPathToAnyGoal(
			startPosition,
			orderedApproachCandidates,
			navGrid,
			preferredApproachPosition,
			outSelectedApproachPosition
		);

	return path;
}

void IngameLevel::MoveToMenu()
{
	GameEngine::Get().ChangeLevel(GameState::Menu);
}

void IngameLevel::SpawnWallAt(const Vector2& position)
{
	if (player->money <= 0) return;

	// 방어적으로 한 번 더 검사.
	if (!IsInsideWorldBounds(position)) return;

	if (position == player->GetPosition()) return;

	player->ChangeBalance(-10);
	AddNewActor(new Wall(position));
}

void IngameLevel::SpawnEnemyAtRandomLocation()
{
	Vector2 spawnLocation;

	if (!TryFindEnemySpawnLocation(
		spawnLocation,
		enemySpawnMinDistanceFromPlayer,
		enemySpawnSearchAttemptCount))
	{
		return;
	}

	// spawnLocation은 반드시 world local 좌표여야 함
	if (!IsInsideWorldBounds(spawnLocation))
	{
		return;
	}

	// 점유된 위치면 생성하지 않음
	if (IsOccupiedByBlockingActor(spawnLocation))
	{
		return;
	}

	AddNewActor(new Enemy(spawnLocation));
}

void IngameLevel::OnEnemyKilled()
{
	enemyKillCount++;

	if (enemyKillCount >= killsRequiredForUpgradeSelection)
	{
		enemyKillCount = 0;
		StartUpgradeSelection();
	}
}

void IngameLevel::ProcessCollisionPlayerBullet()
{
	// 플레이어 탄약과 적 액터, Wall 필터링.
	std::vector<Actor*> bullets;
	std::vector<Enemy*> enemies;
	std::vector<Actor*> walls;

	// 액터 필터링.
	for (Actor* const actor : actors)
	{
		if (actor->IsTypeOf<PlayerBullet>())
		{
			bullets.emplace_back(actor);
			continue;
		}

		if (actor->IsTypeOf<Enemy>())
		{
			enemies.emplace_back(actor->As<Enemy>());
			continue;
		}

		if (actor->IsTypeOf<Wall>())
		{
			walls.emplace_back(actor);
			continue;
		}
	}

	// 판정 안해도 되는지 확인.<
	if (bullets.size() > 0 && enemies.size() > 0)
	{
		// 충돌 판정.
		for (Actor* const bullet : bullets)
		{
			for (Enemy* const enemy : enemies)
			{
				if (!bullet || !enemy) continue;

				// [추가] 이미 파괴 예정이면 무시
				if (bullet->DestroyRequested() || enemy->DestroyRequested())
				{
					continue;
				}

				// AABB 겹침 판정.
				if (bullet->TestIntersect(enemy))
				{
					enemy->OnDamaged(player->attackPower);
					bullet->Destroy();

					// TODO: EXP 처리.
					continue;
				}
			}
		}
	}

	// 총알 <=> 벽 충돌 판정.
	if (bullets.size() > 0 && walls.size() > 0)
	{
		// 충돌 판정.
		for (Actor* const wall : walls)
		{
			for (Actor* const bullet : bullets)
			{
				// AABB 겹침 판정.
				if (wall->TestIntersect(bullet))
				{
					bullet->Destroy();
					continue;
				}
			}
		}
	}


}

void IngameLevel::UpdateFireCooldownUI()
{
	if (!fireCooldownLabel || !player)
	{
		return;
	}

	if (player->IsDead())
	{
		fireCooldownLabel->SetLabelText("Fire: [------]");
		return;
	}

	BuildFireCooldownGaugeText(
		fireCooldownGaugeText,
		sizeof(fireCooldownGaugeText)
	);

	fireCooldownLabel->SetLabelText(fireCooldownGaugeText);
}

void IngameLevel::BuildFireCooldownGaugeText(char* outBuffer, int bufferSize) const
{
	if (!outBuffer || bufferSize <= 0)
	{
		return;
	}

	if (!player)
	{
		snprintf(outBuffer, bufferSize, "Fire: [------]");
		return;
	}

	float progressRatio = player->GetFireCooldownProgressRatio();
	bool isReady = (progressRatio == 1.0f);

	int filledCellCount = static_cast<int>(progressRatio * fireCooldownGaugeCellCount);

	if (filledCellCount < 0)
	{
		filledCellCount = 0;
	}
	else if (filledCellCount > fireCooldownGaugeCellCount)
	{
		filledCellCount = fireCooldownGaugeCellCount;
	}

	char gaugeBuffer[fireCooldownGaugeCellCount + 1] = {};

	for (int i = 0; i < fireCooldownGaugeCellCount; ++i)
	{
		if (i < filledCellCount)
		{
			gaugeBuffer[i] = '#';
		}
		else
		{
			gaugeBuffer[i] = '-';
		}
	}

	gaugeBuffer[fireCooldownGaugeCellCount] = '\0';

	if (isReady)
	{
		snprintf(
			outBuffer,
			bufferSize,
			"Fire:[%s] READY",
			gaugeBuffer
		);
	}
	else
	{
		snprintf(
			outBuffer,
			bufferSize,
			"Fire:[%s]",
			gaugeBuffer
		);
	}
}

void IngameLevel::UpdateMoneyText()
{
	snprintf(
		moneyText,
		sizeof(moneyText),
		"Money: %d",
		player->money
	);

	moneyLabel->SetLabelText(moneyText);
}

bool IngameLevel::CanMove(
	const Wanted::Vector2& currentPositon,
	const Wanted::Vector2& nextPosition, 
	int sortingOrder)
{
	// 이동도 playable world local 기준으로 제한.
	if (!IsInsideWorldBounds(nextPosition))
	{
		return false;
	}

	for (Actor* const actor : actors)
	{
		if (!actor || actor->DestroyRequested())
		{
			continue;
		}

		// 자기 자신의 현재 위치는 제외.
		if (actor->GetPosition() == currentPositon)
		{
			continue;
		}

		if (actor->GetPosition() == nextPosition)
		{
			if (actor->GetSortingOrder() >= sortingOrder)
			{
				return false;
			}
		}
	}

	return true;
}


bool IngameLevel::CanAttackFromPosition(const Vector2& attackPosition, const Vector2& targetPosition, int attackRange) const
{
	return levelNavigation.CanAttackFromPosition(
		attackPosition,
		targetPosition,
		attackRange
	);
}

void IngameLevel::DrawPath(const std::vector<Vector2>& path)
{
	if (path.empty()) return;
	if (!IsPathDrawMode) return;

	const IntRect& worldRect = GetWorldRect();

	for (const Vector2& localPathPosition : path)
	{
		Renderer::Get().Submit(
			"*",
			localPathPosition,
			worldRect,
			Color::Green,
			1
		);
	}
}


void IngameLevel::OnPlayerDead()
{
	// 이미 죽은 상태면 처리 X.
	if (isPlayerDead) return;

	isPlayerDead = true;
	playerDeadPosition = player ? player->GetPosition() : Vector2::Zero;

	// Player 사망 시 Enemy 자동 Spawn 중지.
	enemySpawner.Stop();

	playerDeathTimer.Restart(playerDeathWaitTime);
}

void IngameLevel::RefreshPlayerHpUI()
{
	if (!playerHpValueLabel || !player)
	{
		return;
	}

	// 하트 문자 뒤에 이어붙이기.
	const char heartGlyph = '\x03';

	const int hp = player->hp;
	const int maxBufferLength = static_cast<int>(sizeof(playerHpValueText)) - 1;
	const int heartCount = (hp < maxBufferLength) ? hp : maxBufferLength;

	for (int i = 0; i < heartCount; ++i)
	{
		playerHpValueText[i] = heartGlyph;
	}
	playerHpValueText[heartCount] = '\0';

	playerHpValueLabel->SetLabelText(playerHpValueText);
}

void IngameLevel::UpdatePlayerDeathFlow(float deltaTime)
{
	playerDeathTimer.Tick(deltaTime);

	if (playerDeathTimer.IsTimeOut())
	{
		ReturnToMenuAfterPlayerDeath();
	}
}

void IngameLevel::ReturnToMenuAfterPlayerDeath()
{
	GameEngine::Get().hasActivePlayableSession = false;
	GameEngine::Get().ChangeLevel(GameState::Menu);
}

bool IngameLevel::TryFindEnemySpawnLocation(Vector2& outSpawnLocation, float minSpawnDistanceFromPlayer, int maxSpawnSearchAttempts) const
{
	for (int attemptIndex = 0; attemptIndex < maxSpawnSearchAttempts; ++attemptIndex)
	{
		const Vector2 candidateLocation = GenerateRandomWorldLocation();

		if (!IsEnemySpawnLocationValid(candidateLocation, minSpawnDistanceFromPlayer))
		{
			continue;
		}

		outSpawnLocation = candidateLocation;
		return true;
	}

	return false;
}

bool IngameLevel::IsEnemySpawnLocationValid(const Vector2& candidateLocation, float minSpawnDistanceFromPlayer) const
{
	if (!IsInsideWorldBounds(candidateLocation))
	{
		return false;
	}

	if (!IsFarEnoughFromPlayer(candidateLocation, minSpawnDistanceFromPlayer))
	{
		return false;
	}

	if (IsOccupiedByBlockingActor(candidateLocation))
	{
		return false;
	}

	return true;
}

bool IngameLevel::IsFarEnoughFromPlayer(const Vector2& candidateLocation, float minSpawnDistanceFromPlayer) const
{
	if (!player)
	{
		return true;
	}

	const Vector2 playerLocation = player->GetPosition();

	const float deltaX = static_cast<float>(candidateLocation.x - playerLocation.x);
	const float deltaY = static_cast<float>(candidateLocation.y - playerLocation.y);

	const float distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);
	const float minDistanceSquared = minSpawnDistanceFromPlayer * minSpawnDistanceFromPlayer;

	return distanceSquared >= minDistanceSquared;
}
 
bool IngameLevel::IsOccupiedByBlockingActor(const Vector2& candidateLocation) const
{
	if (player && player->GetPosition() == candidateLocation)
	{
		return true;
	}

	for (Actor* const actor : actors)
	{
		if (!actor || actor->DestroyRequested())
		{
			continue;
		}

		if (actor->GetPosition() != candidateLocation)
		{
			continue;
		}

		if (actor->IsTypeOf<Enemy>() || actor->IsTypeOf<Wall>())
		{
			return true;
		}
	}

	return false;
}

Vector2 IngameLevel::GenerateRandomWorldLocation() const
{
	const IntRect& worldRect = GetWorldRect();

	const int randomX = Util::Random(2, worldRect.width - 3);
	const int randomY = Util::Random(2, worldRect.height - 3);

	return Vector2(
		randomX, randomY
	);
}

void IngameLevel::StartUpgradeSelection()
{
	if (isUpgradeSelectionActive)
	{
		return;
	}

	isUpgradeSelectionActive = true;

	currentUpgradeChoices = GenerateUpgradeChoices(3);

	if (upgradeSelectionUI)
	{
		upgradeSelectionUI->SetChoices(currentUpgradeChoices);
		upgradeSelectionUI->SetVisible(true);
	}
}

void IngameLevel::EndUpgradeSelection()
{
	isUpgradeSelectionActive = false;

	currentUpgradeChoices.clear();

	if (upgradeSelectionUI)
	{
		upgradeSelectionUI->ClearChoices();
		upgradeSelectionUI->SetVisible(false);
	}
}

void IngameLevel::ProcessUpgradeInput()
{
	// 사용자 요청대로 숫자 1,2,3으로 즉시 선택
	if (Input::Get().GetKeyDown('1'))
	{
		SelectUpgrade(0);
	}

	if (Input::Get().GetKeyDown('2'))
	{
		SelectUpgrade(1);
	}

	if (Input::Get().GetKeyDown('3'))
	{
		SelectUpgrade(2);
	}
}

void IngameLevel::SelectUpgrade(int index)
{
	if (index < 0 || index >= static_cast<int>(currentUpgradeChoices.size()))
	{
		return;
	}

	ApplyUpgrade(currentUpgradeChoices[index]);
	
	EndUpgradeSelection();
}

void IngameLevel::ApplyUpgrade(const PlayerUpgradeDefinition& selectedUpgrade)
{
	if (!player)
	{
		return;
	}

	switch (selectedUpgrade.type)
	{
	case PlayerUpgradeType::IncreaseHp:
		player->IncreaseHp(static_cast<int>(selectedUpgrade.value));
		break;

	case PlayerUpgradeType::IncreaseAttackPower:
		player->IncreaseAttackPower(static_cast<int>(selectedUpgrade.value));
		break;

	case PlayerUpgradeType::ReduceFireCooldown:
		player->ReduceFireCooldown(selectedUpgrade.value);
		break;

	default:
		break;
	}

	// 업그레이드 적용 후 UI 즉시 반영
	RefreshPlayerHpUI();
	UpdateFireCooldownUI();
	UpdateMoneyText();
}

std::vector<PlayerUpgradeDefinition> IngameLevel::GenerateUpgradeChoices(int count) const
{
	// 현재 프로젝트에서 바로 적용 가능한 업그레이드만 사용
	// TODO: 카드 추가 및 UI Manager에 넘기기.
	std::vector<PlayerUpgradeDefinition> upgradePool =
	{
		{
			PlayerUpgradeType::IncreaseHp,
			"Reinforced Body",
			"Heal HP +1",
			1.0f
		},
		{
			PlayerUpgradeType::IncreaseAttackPower,
			"Sharpened Shot",
			"Attack Power +1",
			1.0f
		},
		{
			PlayerUpgradeType::ReduceFireCooldown,
			"Rapid Trigger",
			"Fire Interval -0.05",
			0.05f
		}
	};

	std::vector<PlayerUpgradeDefinition> result;

	while (!upgradePool.empty() && static_cast<int>(result.size()) < count)
	{
		const int randomIndex = Util::Random(
			0,
			static_cast<int>(upgradePool.size()) - 1
		);

		result.emplace_back(upgradePool[randomIndex]);
		upgradePool.erase(upgradePool.begin() + randomIndex);
	}

	return result;
}

void IngameLevel::ToggleDrawPath()
{
	if (Input::Get().GetKeyDown(VK_CONTROL))
	{
		IsPathDrawMode = !IsPathDrawMode;
	}
}

void IngameLevel::UpdateFpsLabel(float deltaTime)
{
	if (!fpsLabel)
	{
		return;
	}

	const float displayedFps = Engine::Get().GetDisplayedFps();
	const float frameTimeMs = Engine::Get().GetDisplayedFrameTimeMs();

	snprintf(
		fpsText,
		sizeof(fpsText),
		"FPS: %.1f (%.2f ms)",
		displayedFps,
		frameTimeMs
	);

	fpsLabel->SetLabelText(fpsText);
}

Vector2 IngameLevel::GetWorldScreenOrigin() const
{
	const IntRect& worldRect = GetWorldRect();
	return Vector2(worldRect.x, worldRect.y);
}

Vector2 IngameLevel::GetPlayableWorldSize() const
{
	//  전체 screen 크기에서
	//  좌우 border, 상단 UI Rect + border, 하단 border를 제외한 크기.
	const IntRect& worldRect = GetWorldRect();
	return Vector2(worldRect.width, worldRect.height);
}

bool IngameLevel::IsInsideUIRectScreen(const Vector2& screenPosition) const
{
	const IntRect& uiRect = GetUIRect();
	return uiRect.Contains(screenPosition.x, screenPosition.y);
}

bool IngameLevel::IsInsideWorldScreenRect(const Vector2& screenPosition) const
{
	// screen 좌표 기준으로 실제 플레이 월드 영역인지 검사.
	const IntRect& worldRect = GetWorldRect();
	return worldRect.Contains(screenPosition.x, screenPosition.y);
}

bool IngameLevel::IsInsideWorldBounds(const Vector2& worldLocalPosition) const
{
	const IntRect& worldRect = GetWorldRect();

	//  WorldRect 외곽선 1칸을 제외한 내부만 실제 playable 영역으로 취급.
	return
		worldLocalPosition.x >= 1 &&
		worldLocalPosition.x < worldRect.width - 1 &&
		worldLocalPosition.y >= 1 &&
		worldLocalPosition.y < worldRect.height - 1;
}


bool IngameLevel::TryConvertScreenToWorldPosition(const Vector2& screenPosition, Vector2& outWorldLocalPosition) const
{
	// UI Rect 클릭은 월드 좌표로 변환하지 않음.
	if (IsInsideUIRectScreen(screenPosition))
	{
		return false;
	}

	// 실제 WorldRect 바깥도 차단.
	if (!IsInsideWorldScreenRect(screenPosition))
	{
		return false;
	}

	const IntRect& worldRect = GetWorldRect();

	// world local 좌표로 변환.
	outWorldLocalPosition = Vector2(
		screenPosition.x - worldRect.x,
		screenPosition.y - worldRect.y
	);

	return IsInsideWorldBounds(outWorldLocalPosition);

}
