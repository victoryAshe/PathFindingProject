#include "Level.h"
#include "Actor/Actor.h"
#include "UI/UIElement.h"
#include "Engine/Engine.h"
#include "Render/Renderer.h"

namespace Wanted {
	Level::Level()
	{
		const int screenWidth = Engine::Get().GetWidth();
		const int screenHeight = Engine::Get().GetHeight();
		const int uiPanelWidth = 24;

		// 테두리 부분 포함 X
		worldRect = IntRect(
			1, 1, 
			screenWidth - uiPanelWidth - 2, 
			screenHeight - 2
		);

		uiRect = IntRect(
			screenWidth - uiPanelWidth +1, 
			1, 
			uiPanelWidth - 2, 
			screenHeight - 2
		);
	}

	Level::~Level()
	{
		// actor memory 정리.
		for (Actor*& actor : actors)
		{
			if (actor)
			{
				delete actor;
				actor = nullptr;
			}
		}

		// actor 배열 초기화.
		actors.clear();

		// ui memory 정리.
		for (UIElement*& uiElement : uiElements)
		{
			if (uiElement)
			{
				delete uiElement;
				uiElement = nullptr;
			}
		}
		// ui 배열 초기화.
		uiElements.clear();
	}

	void Level::BeginPlay() 
	{
		// Actor에 event 흘리기.
		for (Actor*& actor : actors)
		{
			// 이미 BeginPlay 호출된 actor는 건너뛰기.
			if (actor->HasBeganPlay())
			{
				continue;
			}

			actor->BeginPlay();
		}

		for (UIElement*& uiElement : uiElements)
		{
			if (!uiElement)
			{
				continue;
			}

			uiElement->BeginPlay();
		}
	}

	void Level::Tick(float deltaTime)
	{
		// Actor에 event 흘리기.
		for (Actor*& actor : actors)
		{
			actor->Tick(deltaTime);
		}

		for (UIElement*& uiElement : uiElements)
		{
			if (!uiElement)
			{
				continue;
			}

			uiElement->Tick(deltaTime);
		}
	}

	void Level::Draw()
	{
		for (Actor*& actor : actors)
		{
			if (!actor->IsActive())
			{
				continue;
			}

			actor->Draw();
		}


		for (UIElement*& uiElement : uiElements)
		{
			if (!uiElement || !uiElement->IsVisible())
			{
				continue;
			}

			uiElement->Draw();
		}

		// [추가] 영역 확인용
		Renderer::Get().SubmitRectOutline(worldRect, Color::White, 999998);
		Renderer::Get().SubmitRectOutline(uiRect, Color::White, 999999);
	}

	void Level::AddNewActor(Actor* newActor)
	{
		// 나중에 추가를 위해 임시 배열에 저장.
		addRequestedActors.emplace_back(newActor);

		// Set Ownership
		newActor->SetOwner(this);
	}

	void Level::ProcessAddAndDestroyActors()
	{
		// 제거 처리.
		for (int ix = 0; ix < static_cast<int>(actors.size());)
		{
			// 제거 요청된 Actor가 있는지 확인.
			if (actors[ix]->DestroyRequested())
			{
				// 삭체 처리.
				delete actors[ix];
				actors.erase(actors.begin() + ix);

				continue;
			}

			++ix;
		}

		// 추가 처리.
		if(addRequestedActors.size()==0) 
		{
			return;
		}

		for(Actor* const actor : addRequestedActors)
		{
			actors.emplace_back(actor);
		}

		// 처리가 끝났으면 배열 초기화.
		addRequestedActors.clear();
	}

	void Level::AddNewUIElement(UIElement* newUIElement)
	{
		if (!newUIElement)
		{
			return;
		}

		addRequestedUIElements.emplace_back(newUIElement);
		newUIElement->SetOwner(this);
	}

	void Level::ProcessPendingUIElements()
	{
		if (addRequestedUIElements.empty())
		{
			return;
		}

		for (UIElement* const uiElement : addRequestedUIElements)
		{
			uiElements.emplace_back(uiElement);
		}

		addRequestedUIElements.clear();
	}
}