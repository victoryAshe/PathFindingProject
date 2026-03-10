#pragma once

#include "Common/RTTI.h"
#include "Math/IntRect.h"

// STL: Standart Template Library - C++에서 기본 제공
// 크기가 알아서 변경되는 배열 (동적 배열)
#include <vector>

namespace Wanted 
{
	// Class Forward Declaration
	class Actor;
	class UIElement;

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

		// UI 추가.
		void AddNewUIElement(UIElement* newUIElement);

		// UI 추가/제거 처리 함수.
		void ProcessPendingUIElements();

		// 레이아웃 영역
		inline void SetWorldRect(const IntRect& newWorldRect) { worldRect = newWorldRect; }
		inline const IntRect& GetWorldRect() const { return worldRect; }

		inline void SetUIRect(const IntRect& newUIRect) { uiRect = newUIRect; }
		inline const IntRect& GetUIRect() const { return uiRect; }


	protected:
		// Actor 배열
		std::vector<Actor*> actors;

		// 실행 중에 추가 요청된 Actor의 배열.
		std::vector<Actor*> addRequestedActors;

		// UI 배열.
		std::vector<UIElement*> uiElements;
		// 실행 중에 추가 요청된 UI 배열.
		std::vector<UIElement*> addRequestedUIElements;

		// World 영억, UI 영역.
		IntRect worldRect;
		IntRect uiRect;
	};

}
