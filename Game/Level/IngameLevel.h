#pragma once

#include "Level/Level.h"
#include "Interface/ICanActorMove.h"
#include "Navigation/NavigationController.h"
#include "Navigation/LevelNavigation.h"

using namespace Wanted;

class IngameLevel 
	: public Level,
	public ICanActorMove
{
	RTTI_DECLARATIONS(IngameLevel, Level)

	
public:
	IngameLevel();
	~IngameLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	// 경로 질의.
	std::vector<Vector2> FindPath(
		const Vector2& start,
		const Vector2& goal
	);

	// 특정 Actor 추적 경로 질의
	// : 해당 Actor의 인접한 칸을 선택해 경로 반환.
	std::vector<Vector2> FindPathToActor(
		const Vector2& start,
		const Vector2& targetPosition
	);

	const std::vector<Actor*>& GetActors() const
	{
		return actors;
	}


private:

	// ICanActorMove Interface를 받아서 override.
	virtual bool CanMove(
		const Wanted::Vector2& curPositon,
		const Wanted::Vector2& nextPosition,
		int sortingOrder
	)override;

private:

	// PathFinding을 위한 class. 
	Navigation::LevelNavigation levelNavigation;
	Navigation::NavigationController navigationController;
};

