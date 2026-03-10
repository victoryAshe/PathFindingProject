#include "IngameLevel.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"
#include "Render/Renderer.h"

// Actor
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/Wall.h"
#include "Actor/PlayerBullet.h"


IngameLevel::IngameLevel()
	: levelNavigation(this)
{
	const IntRect& worldRect = GetWorldRect();

	const Vector2 initialPlayerPosition(
		worldRect.width / 2,
		worldRect.height / 2
	);

	player = new Player(initialPlayerPosition);
	AddNewActor(player);
}

IngameLevel::~IngameLevel()
{
}

void IngameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

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

	ProcessCollisionPlayerBullet();

	// 마우스 왼쪽 input되면 Enemy spawn.
	// TODO: 키 입력에 따라, 마우스 input 처리를 바꾸기.
	if (Input::Get().GetMouseButtonDown(0))
	{
		const Vector2 mouseScreenPosition = Input::Get().MousePosition();
		Vector2 worldLocalPosition;

		if (IsInsideUIRectScreen(mouseScreenPosition)) return;

		if (TryConvertScreenToWorldPosition(mouseScreenPosition, worldLocalPosition))
		{
			SpawnEnemyAt(worldLocalPosition);
		}
		return;
	}

	// 마우스 오른쪽 input되면 Wall Spawn.
	if (Input::Get().GetMouseButtonDown(1))
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

	DrawPlayerUI();

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

void IngameLevel::SpawnEnemyAt(const Vector2& position)
{
	// 방어적으로 한 번 더 검사.
	if (!IsInsideWorldBounds(position)) return;

	AddNewActor(new Enemy(position));
}

void IngameLevel::SpawnWallAt(const Vector2& position)
{
	// 방어적으로 한 번 더 검사.
	if (!IsInsideWorldBounds(position)) return;

	if (position == player->GetPosition()) return;
	AddNewActor(new Wall(position));
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

void IngameLevel::DrawPlayerUI()
{
	static const char* playerHPstring = "PlayerHP: ";
	static const int len = static_cast<int>(strlen(playerHPstring)) +1;
	static char heart[1];
	heart[0] = '\x03';
	Renderer::Get().Submit(playerHPstring, Vector2(2, 2), Color::White, 100);
	
	for (int i = 0; i < player->hp; ++i)
	{
		Renderer::Get().Submit(
			heart,
			Vector2(len + i, 2),
			Color::Red,
			100
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
