#include "GameEngine.h"

// levels 목록에 추가할 level.
#include "Level/IngameLevel.h"
#include "Level/MenuLevel.h"

#include <iostream>


GameEngine* GameEngine::instance = nullptr;

GameEngine::GameEngine()
{
	instance = this;

	// level 생성 및 배열에 추가.
	// ! 새로운 level 생성 시 enum GameState과 맞추기 ! 
	levels.emplace_back(new MenuLevel());
	levels.emplace_back(new IngameLevel());

	// 시작 상태 설정.
	state = GameState::Menu;

	// 게임 시작 시 활성화할 level설정.
	mainLevel = levels[0];
}

GameEngine::~GameEngine()
{
	// 중복 제거 방지.
	mainLevel = nullptr;

	// 모든 레벨 삭제.
	for (Level*& level : levels)
	{
		delete level;
		level = nullptr;
	}

	// 배열 정리.
	levels.clear();
}

void GameEngine::ChangeLevel(GameState state)
{
	// 변경할 index => state.
	int index = static_cast<int>(state);
	this->state = state;

	// 메인 레벨 변경.
	mainLevel = levels[index];
}

void GameEngine::CreateNewInGame()
{
	// InGameLevel의 index는 1로 고정.
	Level* newGame = new IngameLevel();

	// 기존 inGameLevel은 삭제한 뒤에 GameEngine의 pointer를 바꿔줌.
	delete levels[1];        
	levels[1] = newGame;

	hasActivePlayableSession = true;
}

GameEngine& GameEngine::Get()
{
	// Exception Handling.
	if (!instance)
	{
		std::cerr << "Game::Get() - instance is null\n";
		__debugbreak();
	}

	// 정적 변수 반환.
	return *instance;
}