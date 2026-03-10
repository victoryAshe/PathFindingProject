#pragma once

#include "UI/UIElement.h"

namespace Wanted
{
	class WANTED_API LabelUI : public UIElement
	{
		RTTI_DECLARATIONS(LabelUI, UIElement)

	public:
		LabelUI(
			const char* text,
			const Vector2& localPosition = Vector2::Zero,
			Color color = Color::White,
			int sortingOrder = 10000
		)
			: UIElement(localPosition, color, sortingOrder, false)
		{
			SetText(text);
		}

	public:
		void SetLabelText(const char* newText)
		{
			SetText(newText);
		}
	};
}