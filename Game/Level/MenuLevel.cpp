#include "MenuLevel.h"
#include "Engine/GameEngine.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Render/Renderer.h"

MenuLevel::MenuLevel()
{
	// Create MenuItems.
	items.emplace_back(new MenuItem(
		"New Game",
		[]()
		{
			// 새 게임 만들기.
			GameEngine::Get().CreateNewInGame();
	
			// 만든 게임으로 바꾸기.
			GameEngine::Get().ChangeLevel(GameState::GamePlay);
		}
	));

	items.emplace_back(new MenuItem(
		"Resume Game",
		[]()
		{
			GameEngine::Get().ChangeLevel(GameState::GamePlay);
		}
	));

	items.emplace_back(new MenuItem(
		"Quit Game",
		[]()
		{
			// 게임 종료.
			GameEngine::Get().QuitEngine();
		}
	));

	// 시작 시 Resume은 숨김.
	items[ResumeMenuItemIndex]->isVisible = false;
	RebuildVisibleMenuItems();
}

MenuLevel::~MenuLevel()
{
	// Delete MenuItems.
	for (MenuItem*& item : items)
	{
		delete item;
		item = nullptr;
	}

	// 배열 초기화.
	items.clear();
	visibleItemIndices.clear();
}

void
MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	SyncResumeMenuVisibility();
	RebuildVisibleMenuItems();
	HandleMenuInput();

	if (Input::Get().GetKeyDown(VK_ESCAPE) && GameEngine::Get().hasActivePlayableSession)
	{
		// 메뉴 나가기
		GameEngine::Get().ChangeLevel(GameState::GamePlay);

		// 인덱스 초기화.
		currentVisibleIndex = 0;
	}
}

void MenuLevel::Draw()
{
	const static char* title = "Path Finding Project";
	const static int titleLen = static_cast<int>(strlen(title));
	const static Vector2 titlePos = Vector2( (GameEngine::Get().GetWidth() - titleLen) / 2, GameEngine::Get().GetHeight()/2-5);
	Renderer::Get().Submit(title, titlePos);

	// 메뉴 아이템 출력.
	for (int visibleIndex = 0; visibleIndex < static_cast<int>(visibleItemIndices.size()); ++visibleIndex)
	{
		const int sourceItemIndex = visibleItemIndices[visibleIndex];
		MenuItem* menuItem = items[sourceItemIndex];
		if (!menuItem)
		{
			continue;
		}

		
		// 아이템 색상 확인 (선택됐는지 여부).
		Color textColor =
			(visibleIndex == currentVisibleIndex) ? selectedColor : unselectedColor;

		int menuTextLen = static_cast<int>(strlen(menuItem->text));
		Renderer::Get().Submit(
			menuItem->text,
			Vector2( (GameEngine::Get().GetWidth()- menuTextLen) / 2, titlePos.y + 2 + visibleIndex),
			textColor
		);
	}
}

void MenuLevel::RebuildVisibleMenuItems()
{
	visibleItemIndices.clear();

	for (int itemIndex = 0; itemIndex < static_cast<int>(items.size()); ++itemIndex)
	{
		MenuItem* menuItem = items[itemIndex];
		if (!menuItem)
		{
			continue;
		}

		if (menuItem->isVisible)
		{
			visibleItemIndices.emplace_back(itemIndex);
		}
	}

	if (visibleItemIndices.empty())
	{
		currentVisibleIndex = -1;
		return;
	}

	if (currentVisibleIndex < 0)
	{
		currentVisibleIndex = 0;
	}
	else if (currentVisibleIndex >= static_cast<int>(visibleItemIndices.size()))
	{
		currentVisibleIndex = static_cast<int>(visibleItemIndices.size()) - 1;
	}
}

void MenuLevel::SyncResumeMenuVisibility()
{
	MenuItem* resumeMenuItem = items[ResumeMenuItemIndex];
	if (!resumeMenuItem)
	{
		return;
	}

	resumeMenuItem->isVisible = GameEngine::Get().hasActivePlayableSession;
}

void MenuLevel::HandleMenuInput()
{
	// 입력 처리 (방향키 위/아래키, 엔터키, ESC키).
	// 배열 길이.
	const int visibleMenuCount = static_cast<int>(visibleItemIndices.size());
	if (Input::Get().GetKeyDown(VK_UP))
	{
		// 인덱스 돌리기 (위 방향으로).
		currentVisibleIndex = (currentVisibleIndex - 1 + visibleMenuCount) % visibleMenuCount;
	}

	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		// 인덱스 돌리기.
		currentVisibleIndex =
			(currentVisibleIndex + 1) % visibleMenuCount;
	}

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		// 메뉴 아이템이 저장한 함수 포인터 호출.
		const int selectedSourceItemIndex = visibleItemIndices[currentVisibleIndex];
		MenuItem* selectedMenuItem = items[selectedSourceItemIndex];

		if (selectedMenuItem && selectedMenuItem->onSelected)
		{
			selectedMenuItem->onSelected();
		}
	}


}
