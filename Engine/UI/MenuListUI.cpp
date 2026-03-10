#include "UI/MenuListUI.h"

namespace Wanted
{
	MenuListUI::MenuListUI(
		const Vector2& localPosition,
		Color color,
		int sortingOrder)
		: UIElement(localPosition, color, sortingOrder, false)
	{
	}

	void MenuListUI::SetItems(const std::vector<std::string>& newItems)
	{
		items = newItems;

		if (items.empty())
		{
			selectedIndex = 0;
			ClearTextBuffer();
			return;
		}

		if (selectedIndex < 0)
		{
			selectedIndex = 0;
		}
		else if (selectedIndex >= static_cast<int>(items.size()))
		{
			selectedIndex = static_cast<int>(items.size()) - 1;
		}

		RebuildDisplayText();
	}

	void MenuListUI::AddItem(const std::string& newItem)
	{
		items.emplace_back(newItem);

		if (items.size() == 1)
		{
			selectedIndex = 0;
		}

		RebuildDisplayText();
	}

	void MenuListUI::ClearItems()
	{
		items.clear();
		selectedIndex = 0;
		ClearTextBuffer();
	}

	void MenuListUI::SetSelectedIndex(int newSelectedIndex)
	{
		if (items.empty())
		{
			selectedIndex = 0;
			ClearTextBuffer();
			return;
		}

		if (newSelectedIndex < 0)
		{
			newSelectedIndex = 0;
		}
		else if (newSelectedIndex >= static_cast<int>(items.size()))
		{
			newSelectedIndex = static_cast<int>(items.size()) - 1;
		}

		selectedIndex = newSelectedIndex;
		RebuildDisplayText();
	}

	int MenuListUI::GetSelectedIndex() const
	{
		return selectedIndex;
	}

	void MenuListUI::SetSelectedPrefix(const std::string& newSelectedPrefix)
	{
		selectedPrefix = newSelectedPrefix;
		RebuildDisplayText();
	}

	void MenuListUI::SetUnselectedPrefix(const std::string& newUnselectedPrefix)
	{
		unselectedPrefix = newUnselectedPrefix;
		RebuildDisplayText();
	}

	const std::vector<std::string>& MenuListUI::GetItems() const
	{
		return items;
	}

	void MenuListUI::RebuildDisplayText()
	{
		if (items.empty())
		{
			ClearTextBuffer();
			return;
		}

		std::string mergedText;

		for (int i = 0; i < static_cast<int>(items.size()); ++i)
		{
			if (i > 0)
			{
				mergedText += '\n';
			}

			mergedText += (i == selectedIndex) ? selectedPrefix : unselectedPrefix;
			mergedText += items[i];
		}

		SetMultilineText(mergedText.c_str());
	}
}