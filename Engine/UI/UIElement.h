#pragma once

#include "Common/Common.h"
#include "Common/RTTI.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace Wanted
{
	class Level;

	class WANTED_API UIElement : public RTTI
	{
		RTTI_DECLARATIONS(UIElement, RTTI)

	public:
		UIElement(
			const Vector2& localPosition = Vector2::Zero,
			Color color = Color::White,
			int sortingOrder = 10000,
			bool spaceTransparent = true
		);

		virtual ~UIElement();

	public:
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

	public:

		// Getter & Setter.
		inline void SetLocalPosition(const Vector2& newLocalPosition);
		inline Vector2 GetLocalPosition() const { return localPosition; }

		inline void SetColor(Color newColor) { color = newColor; }
		inline Color GetColor() const { return color; }

		inline void SetSortingOrder(int newSortingOrder) { sortingOrder = newSortingOrder; }
		inline int GetSortingOrder() const { return sortingOrder; }

		inline void SetVisible(bool newVisible) { isVisible = newVisible; }
		inline bool IsVisible() const { return isVisible; }

		inline void SetSpaceTransparent(bool newSpaceTransparent) { spaceTransparent = newSpaceTransparent; }
		bool IsSpaceTransparent() const { return spaceTransparent; }

		void SetText(const char* newText);
		void SetMultilineText(const char* newText);

		inline void SetRenderInWorldRect(bool RenderInWorldRect) { isRenderInWorldRect = RenderInWorldRect; }
		bool IsRenderingInWorldRect() const { return isRenderInWorldRect; }

		inline const char* GetTextBuffer() const { return textBuffer; }
		int GetWidth() const;
		int GetHeight() const;

		inline void SetOwner(Level* newOwner) { owner = newOwner; }
		Level* GetOwner() const;

		inline bool IsActive() const { return isActive; }


	protected:
		virtual void OnDrawUI();

		void ClearTextBuffer();

	private:
		void CopySingleLineText(const char* sourceText);
		void CopyMultilineText(const char* sourceText);

	protected:
		bool isActive = true;

		Level* owner = nullptr;

		// UIRect 내부 로컬 좌표
		Vector2 localPosition = Vector2::Zero;

		// WorldRect에 렌더할지 여부
		bool isRenderInWorldRect = false;

		Color color = Color::White;

		bool isVisible = true;
		bool spaceTransparent = true;

		int sortingOrder = 10000;

		char* textBuffer = nullptr;
		int width = 0;
		int height = 0;
	};
}