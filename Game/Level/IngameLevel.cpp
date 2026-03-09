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
	player = new Player(Vector2(10, 10));
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

	ProcessCollisionPlayerBulletAndEnemy();

	// 마우스 왼쪽 input되면 Enemy spawn.
	// TODO: 키 입력에 따라, 마우스 input 처리를 바꾸기.
	if (Input::Get().GetMouseButtonDown(0))
	{
		Vector2 mousePosition = Input::Get().MousePosition();
		CreateEnemy(mousePosition);
		return;
	}

	// 마우스 오른쪽 input되면 Wall Spawn.
	if (Input::Get().GetMouseButtonDown(1))
	{
		Vector2 mousePosition = Input::Get().MousePosition();
		CreateWall(mousePosition);
		return;
	} 
}


void IngameLevel::Draw()
{
	super::Draw();

	ShowPlayerUI();

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

void IngameLevel::CreateEnemy(const Vector2 position)
{
	AddNewActor(new Enemy(position));
}

void IngameLevel::CreateWall(const Vector2 position)
{
	if (position == player->GetPosition()) return;
	AddNewActor(new Wall(position));
}

void IngameLevel::ProcessCollisionPlayerBulletAndEnemy()
{
	// 플레이어 탄약과 적 액터 필터링.
	std::vector<Actor*> bullets;
	std::vector<Enemy*> enemies;

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
		}
	}

	// 판정 안해도 되는지 확인.
	if (bullets.size() == 0 || enemies.size() == 0)
	{
		return;
	}

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

void IngameLevel::ShowPlayerUI()
{
	static const char* playerHPstring = "PlayerHP: ";
	static const int len = static_cast<int>(strlen(playerHPstring)) +1;
	static char heart[1];
	heart[0] = '\x03';
	Renderer::Get().Submit(playerHPstring, Vector2(1, 1), Color::White, 100);
	
	for (int i = 0; i < player->hp; ++i)
	{
		Renderer::Get().Submit(
			heart,
			Vector2(len + i, 1),
			Color::Red,
			100
		);
	}
}


bool IngameLevel::CanMove(
	const Wanted::Vector2& curPositon, 
	const Wanted::Vector2& nextPosition, 
	int sortingOrder)
{
	const Vector2 screenSize = GameEngine::Get().GetScreenSize();

	// Exception Handling.
	if (nextPosition.x < 0 || nextPosition.x >= screenSize.x
		|| nextPosition.y < 0 || nextPosition.y >= screenSize.y)
	{
		return false;
	}

	for (Actor* const actor : actors)
	{
		if (!actor || actor->DestroyRequested())
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

void IngameLevel::DrawPath(std::vector<Vector2> const path)
{
	if (path.empty()) return;

	for (const Vector2 pos : path)
	{
		Renderer::Get().Submit(
			"*",
			pos,
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
	GameEngine::Get().CreateNewInGame();
	GameEngine::Get().ChangeLevel(GameState::Menu);
}