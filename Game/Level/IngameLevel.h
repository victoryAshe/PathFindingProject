#pragma once

#include "Level/Level.h"
#include "Interface/ICanActorMove.h"
#include "Navigation/AStar.h"

using namespace Wanted;

class IngameLevel 
	: public Level,
	public ICanActorMove
{
	RTTI_DECLARATIONS(IngameLevel, Level)

	
public:
	// 나중에 추가할 것이 생기면 생성자/소멸자 수정.
	IngameLevel();
	~IngameLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	// 경로 찾아주는 용도.
	std::vector<Vector2> FindPath(
		const Vector2& start,
		const Vector2& goal
	);

private:

	// ICanActorMove Interface를 받아서 override.
	virtual bool CanMove(
		const Wanted::Vector2& curPositon,
		const Wanted::Vector2& nextPosition,
		int sortingOrder
	)override;

	// Navigation에 전달할 grid를 생성.
	std::vector<std::vector<int>> BuildNavigationGrid() const;

private:

	// EngineSetting의 screenSize를 갖고 옴.
	Vector2 screenSize;

	// AStar를 멤버로 갖도록 함.
	Navigation::AStar aStar;
};

