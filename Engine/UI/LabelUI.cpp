#include "LabelUI.h"

namespace Wanted
{
	LabelUI::LabelUI(
		const char* text, 
		const Vector2& localPosition, 
		Color color, 
		int sortingOrder)
		: super(localPosition, color, sortingOrder)
	{
		SetColor(color);
		SetText(text);
	}

	LabelUI::~LabelUI()
	{
	}

	void LabelUI::SetLabelText(const char* newText)
	{
		SetText(newText);
	}
}