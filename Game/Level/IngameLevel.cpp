#include "IngameLevel.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"

#include "Actor/Enemy.h"
#include "Actor/MouseTester.h"

IngameLevel::IngameLevel()
{

	screenSize = Vector2(GameEngine::Get().GetWidth(), GameEngine::Get().GetHeight());

	// TODO: 지우기.
	AddNewActor(new MouseTester());
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
	std::vector<std::vector<int>> grid = BuildNavigationGrid();

	if (!grid.empty() && !grid[0].empty())
	{
		if (start.y >= 0 && start.y < static_cast<int>(grid.size())
			&& start.x >= 0 && start.x < static_cast<int>(grid[0].size()))
		{
			grid[start.y][start.x] = 0;
		}

		if (goal.y >= 0 && goal.y < static_cast<int>(grid.size())
			&& goal.x >= 0 && goal.x < static_cast<int>(grid[0].size()))
		{
			grid[goal.y][goal.x] = 0;
		}
	}

	return aStar.FindPath(start, goal, grid);
}

bool IngameLevel::CanMove(
	const Wanted::Vector2& curPositon, 
	const Wanted::Vector2& nextPosition, 
	int sortingOrder)
{
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

std::vector<std::vector<int>> IngameLevel::BuildNavigationGrid() const
{
	std::vector<std::vector<int>> grid(
		screenSize.y,
		std::vector<int>(screenSize.x, 0)
	);

	for (Actor* const actor : actors)
	{
		if (!actor || actor->DestroyRequested())
		{
			continue;
		}

		const Vector2 pos = actor->GetPosition();

		if (pos.x < 0 || pos.x >= screenSize.x
			|| pos.y < 0 || pos.y >= screenSize.y)
		{
			continue;
		}

		// MouseTester 같은 디버그용 Actor까지 막히면 불편하므로,
		// 우선 sortingOrder가 0보다 큰 Actor만 장애물처럼 취급.
		if (actor->GetSortingOrder() > 0)
		{
			grid[pos.y][pos.x] = 1;
		}
	}

	return grid;
}
