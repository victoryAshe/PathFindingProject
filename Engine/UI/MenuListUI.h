#pragma once

#include "UI/UIElement.h"

#include <string>
#include <vector>

namespace Wanted
{
	class WANTED_API MenuListUI : public UIElement
	{
		RTTI_DECLARATIONS(MenuListUI, UIElement)

	public:
		MenuListUI(
			const Vector2& localPosition = Vector2::Zero,
			Color color = Color::White,
			int sortingOrder = 10000
		);

	public:
		void SetItems(const std::vector<std::string>& newItems);
		void AddItem(const std::string& newItem);
		void ClearItems();

		void SetSelectedIndex(int newSelectedIndex);
		int GetSelectedIndex() const;

		void SetSelectedPrefix(const std::string& newSelectedPrefix);
		void SetUnselectedPrefix(const std::string& newUnselectedPrefix);

		const std::vector<std::string>& GetItems() const;

	private:
		void RebuildDisplayText();

	private:
		std::vector<std::string> items;
		int selectedIndex = 0;

		std::string selectedPrefix = "> ";
		std::string unselectedPrefix = "  ";
	};
}