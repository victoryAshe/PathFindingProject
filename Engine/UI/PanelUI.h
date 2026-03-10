#pragma once

#include "UI/UIElement.h"

namespace Wanted
{
	class WANTED_API PanelUI : public UIElement
	{
		RTTI_DECLARATIONS(PanelUI, UIElement)

	public:
		PanelUI(
			const char* text,
			const Vector2& localPosition = Vector2::Zero,
			Color color = Color::White,
			int sortingOrder = 10000,
			bool spaceTransparent = false
		)
			: UIElement(localPosition, color, sortingOrder, spaceTransparent)
		{
			SetMultilineText(text);
		}

	public:
		void SetPanelText(const char* newText)
		{
			SetMultilineText(newText);
		}
	};
}