#include "Level.h"
#include "Actor/Actor.h"

namespace Wanted {
	Level::Level()
	{

	}

	Level::~Level()
	{
		// memory 정리.
		for (Actor*& actor : actors)
		{
			if (actor)
			{
				delete actor;
				actor = nullptr;
			}
		}

		// 배열 초기화.
		actors.clear();
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
	}
	void Level::Tick(float deltaTime)
	{
		// Actor에 event 흘리기.
		for (Actor*& actor : actors)
		{
			actor->Tick(deltaTime);
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

}