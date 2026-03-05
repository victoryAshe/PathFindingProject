#pragma once

#include "Common/Common.h"
#include "Common/RTTI.h"
#include "Assets/AsciiLoader.h"
#include "Math/Vector2.h"
#include "Math/Color.h"


#include <memory>

namespace Wanted
{
	struct AsciiArt;
	class Level;

	class WANTED_API UI : public RTTI
	{
		RTTI_DECLARATIONS(UI, RTTI)

	public:
		// AsciiArt::shared_ptr 생성자
		// explicit keyword: 계산 도중 일어나는 자동 형변환을 원천봉쇄.
		explicit UI(
			const std::string asciiUIName,
			const Vector2& position = Vector2::Zero,
			Color color = Color::White,
			bool spaceTransparent = true
		);

		virtual ~UI();

	protected:
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// AsciiArt를 Name을 통해 불러옴.
		void SetAsciiArtByName(const std::string& stateName);

		void SetPosition(const Vector2& newPosition);
		inline Vector2 GetPosition() const { return position; }

		// Add/Read Ownership.
		inline void SetOwner(Level* newOwner) { owner = newOwner; }
		inline Level* GetOwner() const { return owner; }

		inline bool IsActive() const
		{
			return isActive;
		}

		inline int GetSortingOrder() const { return sortingOrder; }

		inline int GetWidth() const { return width; }
		inline int GetHeight() const { return height; }

		inline bool IsSpaceTransparent() const { return spaceTransparent; }

	protected:
		// 활성화 상태 여부.
		bool isActive = true;

		// 2D AsciiArt의 shared_ptr.
		std::shared_ptr<const AsciiArt> sharedArt;

		int width = 0;
		int height = 0;

		// ASCII 공백의 투명도를 인정
		// true=> 공백인 곳에 sortingOrder가 뒤에 있는 동일위치의 Image를 합성
		bool spaceTransparent = true;

		// 색상
		Color color = Color::White;

		// Ownership.
		Level* owner = nullptr;

		// Render priority: 값이 크면 우선순위가 높도록 설정.
		int sortingOrder = 100;

		// 위치.
		Vector2 position;

		std::string asciiUIName;
		std::string stateName;

	};
}