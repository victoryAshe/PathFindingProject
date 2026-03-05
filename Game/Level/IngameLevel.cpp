#include "IngameLevel.h"
#include "Core/Input.h"
#include "Engine/GameEngine.h"


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

}

bool IngameLevel::CanMove(
	const Wanted::Vector2& curPositon, 
	const Wanted::Vector2& nextPosition, 
	int sortingOrder)
{

	return false;
}
