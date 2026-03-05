#pragma once

#include "Level/Level.h"
#include "Interface/ICanActorMove.h"

using namespace Wanted;

class IngameLevel 
	: public Level,
	public ICanActorMove
{
	RTTI_DECLARATIONS(IngameLevel, Level)

public:
	// 나중에 추가할 것이 생기면 생성자/소멸자 수정.
	IngameLevel() = default;
	~IngameLevel() = default;

	virtual void Tick(float deltaTime) override;

private:

	virtual bool CanMove(
		const Wanted::Vector2& curPositon,
		const Wanted::Vector2& nextPosition,
		int sortingOrder
	)override;


private:

};

