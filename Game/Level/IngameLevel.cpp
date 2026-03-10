#include "IngameLevel.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"
#include "Render/Renderer.h"

// Actor
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/Wall.h"
#include "Actor/PlayerBullet.h"

// UI 문자열 처리를 위해 include.
#include <cstring>


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

	// =========================
	// Player HP UI 생성
	// : localPosition은 UIRect 내부 기준 좌표
	// =========================
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

	AddNewUIElement(playerHpTitleLabel);
	AddNewUIElement(playerHpValueLabel);
	AddNewUIElement(fireCooldownLabel);


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
	super::Tick(deltaTime);

	// UI 문자열을 매 프레임 최신 상태로 갱신.
	UpdateFireCooldownUI();


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
	// 방어적으로 한 번 더 검사.
	if (!IsInsideWorldBounds(position)) return;

	if (position == player->GetPosition()) return;
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

	// 방어적으로 월드 범위 재검사
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
	// [추가]
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
	const int maxBufferLength = static_cast<int>(sizeof(playerHpValueBuffer)) - 1;
	const int heartCount = (hp < maxBufferLength) ? hp : maxBufferLength;

	for (int i = 0; i < heartCount; ++i)
	{
		playerHpValueBuffer[i] = heartGlyph;
	}
	playerHpValueBuffer[heartCount] = '\0';

	playerHpValueLabel->SetLabelText(playerHpValueBuffer);
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

	const int randomX = Util::Random(worldRect.GetLeft(), worldRect.GetRight() - 1);
	const int randomY = Util::Random(worldRect.GetTop(), worldRect.GetBottom() - 1);

	return Vector2(
		static_cast<float>(randomX),
		static_cast<float>(randomY)
	);
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
