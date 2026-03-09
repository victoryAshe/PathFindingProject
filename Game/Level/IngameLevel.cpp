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

	
	// ESC키 처리.
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// 메뉴 활성화.
		MoveToMenu();
		return;
	}

	ProcessCollisionPlayerBulletAndEnemy();
	ProcessCollisionPlayerAndEnemyBullet();

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
				enemy->OnDamaged();
				bullet->Destroy();

				// 점수 추가.
				// TODO: EXP 처리.
				//score += 1;
				continue;
			}
		}
	}
}

void IngameLevel::ProcessCollisionPlayerAndEnemyBullet()
{
}

void IngameLevel::ShowPlayerUI()
{
	static const char* playerHPstring = "PlayerHP: ";
	static const int len = static_cast<int>(strlen(playerHPstring)) +1;
	static char heart[1];
	heart[0] = 3;
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
