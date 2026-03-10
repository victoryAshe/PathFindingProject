#pragma once

#include "Engine/Engine.h"

#include <vector>

using namespace Wanted;

// Enum for Level Management.
enum class GameState
{
	Menu = 0,
	GamePlay = 1,
	Dialogue = 2,
	Length
};


class GameEngine : public Engine
{
public:
	GameEngine();
	~GameEngine();

	// 기존 level을 삭제하지 않고, Level만 전환하는 함수.
	void ChangeLevel(GameState state);

	// 새로운 InGameLevel만들고 기존 InGameLevel 폐기.
	void CreateNewInGame();

	static GameEngine& Get();

public:
	bool hasActivePlayableSession = false;

private:
	// Game에서 관리하는 level.
	std::vector<Level*> levels;

	// 현재 활성화된 Level을 나타냄
	GameState state = GameState::Menu;

	// Singleton.
	static GameEngine* instance;
};

