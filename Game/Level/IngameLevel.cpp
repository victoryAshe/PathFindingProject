#include "IngameLevel.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"

#include "Actor/Enemy.h"
#include "Actor/MouseTester.h"

IngameLevel::IngameLevel()
{
	// TODO: 아래 Actor들을 마우스 Input 등에 따라 Spawn.
	AddNewActor(new MouseTester());
	//AddNewActor(new Enemy(Vector2(0,0)));
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

bool IngameLevel::CanMove(
	const Wanted::Vector2& curPositon, 
	const Wanted::Vector2& nextPosition, 
	int sortingOrder)
{

	return false;
}
