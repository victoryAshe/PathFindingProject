#include "IngameLevel.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"

#include "Actor/Enemy.h"

IngameLevel::IngameLevel()
	: levelNavigation(this)
{
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
		GameEngine::Get().ChangeLevel(GameState::Menu);
		return;
	}

	// 마우스 input되면 적 spawn되도록 처리.
	// TODO: 키 입력에 따라, 마우스 input 처리를 바꾸기.
	if (Input::Get().GetMouseButtonDown(0))
	{
		Vector2 mousePosition = Input::Get().MousePosition();
		AddNewActor(new Enemy(mousePosition));
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
	return navigationController.FindPath(start, goal, navGrid);
}

std::vector<Vector2> IngameLevel::FindPathToActor(
	const Vector2& start, 
	const Vector2& targetPosition
)
{
	std::vector<std::vector<int>> navGrid = levelNavigation.BuildNavGrid();

	// Actor와 인접한 칸 수집
	const std::vector<Vector2> approachPositions =
		levelNavigation.FindApproachPositions(navGrid, targetPosition);

	// Actor의 인접한 칸에 접근 불가능.
	if (approachPositions.empty()) return {};

	// 인접한 칸 목록에서 최적의 칸 선택.
	const Vector2 selectedApproachPosition =
		levelNavigation.SelectBestApproachPosition(start, approachPositions);

	return navigationController.FindPath(start, selectedApproachPosition, navGrid);
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
