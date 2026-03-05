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
		[this]()
		{
			// 만약 게임이 시작되지 않았으면.
			if (GameEngine::Get().IsGameInitialized == false)
			{
				CreateNewMenu(1, new MenuItem(
					"Resume Game",
					[]()
					{
						GameEngine::Get().ChangeLevel(GameState::GamePlay);
					}
				));
			}
	
			// 새 게임 만들기.
			GameEngine::Get().CreateNewInGame();
	
			// 만든 게임으로 바꾸기.
			GameEngine::Get().ChangeLevel(GameState::GamePlay);
		}
	));

	// temporary.
	//items.emplace_back(new MenuItem(
	//	"New Game",
	//	[]()
	//	{
	//
	//		// 새 게임 만들기.
	//		GameEngine::Get().CreateNewInGame();
	//
	//		// 만든 게임으로 바꾸기.
	//		GameEngine::Get().ChangeLevel(GameState::GamePlay);
	//	}
	//));

	items.emplace_back(new MenuItem(
		"Quit Game",
		[]()
		{
			// 게임 종료.
			GameEngine::Get().QuitEngine();
		}
	));

	
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
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 입력 처리 (방향키 위/아래키, 엔터키, ESC키).
	// 배열 길이.
	int length = static_cast<int>(items.size());
	if (Input::Get().GetKeyDown(VK_UP))
	{
		// 인덱스 돌리기 (위 방향으로).
		currentIndex = (currentIndex - 1 + length) % length;
	}

	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		// 인덱스 돌리기.
		currentIndex = (currentIndex + 1) % length;
	}

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		// 메뉴 아이템이 저장한 함수 포인터 호출.
		items[currentIndex]->onSelected();
	}

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// 메뉴 나가기
		GameEngine::Get().ChangeLevel(GameState::GamePlay);

		// 인덱스 초기화.
		currentIndex = 0;
	}
}

void MenuLevel::Draw()
{

	Renderer::Get().Submit("Sokoban Game", Vector2::Zero);

	// 메뉴 아이템 출력.
	for (int ix = 0; ix < static_cast<int>(items.size()); ++ix)
	{
		// 아이템 색상 확인 (선택됐는지 여부).
		Color textColor =
			(ix == currentIndex) ? selectedColor : unselectedColor;

		Renderer::Get().Submit(
			items[ix]->text,
			Vector2(0, 2 + ix),
			textColor
		);
	}
}

void MenuLevel::CreateNewMenu(int i, MenuItem* item)
{
	items.insert(items.begin() + i, item);
}
