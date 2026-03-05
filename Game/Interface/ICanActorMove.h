#pragma once

#include "Math/Vector2.h"

// Actor가 이동할 위치를 판단해주는 Interface.
// Interface: 순수 가상 함수를 갖는 class.

class ICanActorMove
{
public:
	// Actor가 이동 가능한지 질의할 때 사용할 함수.
	// curPosition: Actor의 현재 위치.
	// nextPositoin: Actor의 다음 위치.
	// int sortingOrder: 해당 actor의 sortingOrder 
	//   => 이 숫자를 통해 이동을 막는 등의 처리 진행. 
	virtual bool CanMove(
		const Wanted::Vector2& curPositoin,
		const Wanted::Vector2& nextPosition,
		int sortingOrder
	) = 0;
};