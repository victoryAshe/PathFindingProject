#include "Actor.h"
#include "Util/Util.h"
#include "Assets/AsciiLoader.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"
#include "Level/Level.h"

#include <iostream>
#include <Windows.h>
#include <cstring>

namespace Wanted
{
	// ================================
	// Actor.cpp 전용 helper: internal linkage
	// ================================
	namespace
	{
		void CopyLine(
			char*& dest,
			int& outW,
			int& outH,
			const char* source
		)
		{
			SafeDeleteArray(dest);
			
			if (!source)
			{
				outW = 0; outH = 0;
				return;
			}

			outH = 1;
			outW = static_cast<int>(strlen(source));

			dest = new char[outW + 1];
			strcpy_s(dest, outW + 1, source); // '\0' 포함.
		}
	}

	Actor::Actor(const char* image,
		const Vector2& position,
		Color color)
		: position(position), color(color), height(1)
	{
		// 1줄 Text는 공백도 그려야하는 경우가 많으니 기본 false로 지정.
		// 나중에 필요하면 true로 바꾸던가 하기.
		spaceTransparent = false; 
		CopyLine(this->image, width, height, image);
	}

	
	Actor::Actor(
		const std::string asciiActorName,
		const Vector2& position,
		Color color,
		bool spaceTransparent)
		: position(position), color(color), spaceTransparent(spaceTransparent)
	{
		SetAsciiArtByName(asciiActorName);
	}

	Actor::~Actor()
	{
		// Free memory.
		SafeDeleteArray(image);
	}

	void Actor::BeginPlay() 
	{
		// Event 받은 후에는 flag 설정.
		hasBeganPlay = true;
	}

	void Actor:: Tick(float deltaTime)
	{
	}

	void Actor:: Draw()
	{
		const IntRect renderRect = owner ? owner->GetWorldRect() : IntRect();

		// shared_ptr을 가진 AsciiArtActor
		// Renderer에 shared_ptr을 넘겨서 RenderQueue가 자원 수명 보장.
		if (sharedArt && sharedArt->isValid())
		{
			Renderer::Get().Submit(
				sharedArt,
				position,
				renderRect,
				color,
				sortingOrder,
				spaceTransparent
			);
			return;
		}

		// 기존 1줄: Renderer에 Data 제출.
		Renderer::Get().Submit(
			image, 
			width,
			height,
			position,
			renderRect,
			color, 
			sortingOrder,
			spaceTransparent
		);
	}

	void Actor::SetAsciiArtByName(const std::string& stateName)
	{
		sharedArt = AsciiLoader::GetOrLoad(actorName,stateName);

		// 공유 resource를 쓰는 경우: 기존 소유 buffer가 필요없으면 해제.
		SafeDeleteArray(image);

		if (sharedArt && sharedArt->isValid())
		{
			width = sharedArt->width;
			height = sharedArt->height;
		}
	}


	void Actor::Destroy()
	{
		// 삭제 flag 설정.
		destroyRequested = true;

		// 삭제 event 호출.
		OnDestroy();
	}

	void Actor::OnDestroy()
	{ }

	void Actor::QuitGame()
	{
		// Engine 종료 요청.
		Engine::Get().QuitEngine();
	}

	bool Actor::TestIntersect(const Actor* const other)
	{
		// AABB (Axis Aligned Bounding Box).
		
		// 계산 필요가 없는 경우 filter.
		if (width <= 0 || height <= 0 || other->GetWidth() <= 0 || other->GetHeight() <= 0)
		{
			return false;
		}

		// 자기 자신의 좌표 정보.
		int aMinX = position.x;
		int aMinY = position.y;
		int aMaxX = position.x + width - 1;
		int aMaxY = position.y + height - 1;


		// 충돌을 비교할 다른 Actor의 좌표 정보.
		Vector2 bPos = other->GetPosition();
		int bMinX = bPos.x;
		int bMinY = bPos.y;
		int bMaxX = bPos.x + other->GetWidth() - 1;
		int bMaxY = bPos.y + other->GetHeight() - 1;

		// 분리되어있는지 확인.
		bool separated =
			(bMinX > aMaxX) || (bMaxX < aMinX) ||
			(bMinY > aMaxY) || (bMaxY < aMinY);

		return !separated;
	}

	void Actor::ChangeImage(const char* newImage)
	{
		// 기존 memory 해제.
		SafeDeleteArray(image);

		// 새로운 문자열 복사.
		width = static_cast<int>(strlen(newImage));
		image = new char[width + 1];
		strcpy_s(image, width+1, newImage);
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		// 변경하려는 위치가 현재 위치와 같으면 건너뜀.
		if(position == newPosition)
		{
			return;
		}

		// 새로운 위치 설정.
		position = newPosition;
	}
}