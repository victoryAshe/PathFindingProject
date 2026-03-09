#include "IngameLevel.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"
#include "Render/Renderer.h"


// Actor
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/Wall.h"


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
}

std::vector<Vector2> IngameLevel::FindPath(
	const Vector2& start, 
	const Vector2& goal
)
{
	std::vector<std::vector<int>> navGrid = levelNavigation.BuildNavGrid();
	std::vector<Vector2> path = navigationController.FindPath(start, goal, navGrid);
	DrawPath(path);
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

	DrawPath(path);

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
