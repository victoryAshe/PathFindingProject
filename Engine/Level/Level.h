#pragma once

// STL: Standart Template Library - C++에서 기본 제공
// 크기가 알아서 변경되는 배열 (동적 배열)
#include <vector>

#include "Common/RTTI.h"

namespace Wanted 
{
	// Class Forward Declaration
	class Actor;

	// 담당 임무: Level에 있는 모든 Actor(물체 관리)
	class WANTED_API Level: public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)

	public:
		Level();
		virtual ~Level();

		// GamePlay Event.
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// Actor 추가 함수.
		void AddNewActor(Actor* newActor);

		// Actor 추가/제거 처리 함수.
		void ProcessAddAndDestroyActors();

	protected:
		// Actor 배열
		std::vector<Actor*> actors;

		// 실행 중에 추가 요청된 Actor의 배열.
		std::vector<Actor*> addRequestedActors;
	};

}
