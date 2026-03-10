#pragma once

#include "Level/Level.h"
#include "Math/Color.h"

#include <vector>
#include <functional>

using namespace Wanted;

// Menu에서 돌아갈 UI 버튼(키 업/다운 및 Enter로 작동하는) 구조체.
struct MenuItem
{
	// Menu 선택 시 실행될 함수의 type (FunctionPointer)
	using OnSelected = std::function<void()>;

	MenuItem(const char* text, OnSelected onSelected)
		:onSelected(std::move(onSelected))
	{
		// copy text.
		size_t length = strlen(text) + 1;
		this->text = new char[length];
		strcpy_s(this->text, length, text);
	}

	~MenuItem()
	{
		if (text)
		{
			delete[] text;
			text = nullptr;
		}
	}

	// Menu에 표시될 ui text.
	char* text = nullptr;

	// Menu 선택됐을 때 실행될 logic.
	OnSelected onSelected = nullptr;

	// 화면에 표시할 지 여부.
	bool isVisible = true;
};

class MenuLevel: public Level
{
	RTTI_DECLARATIONS(MenuLevel, Level);

public:
	MenuLevel();
	~MenuLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:
	void RebuildVisibleMenuItems();
	void SyncResumeMenuVisibility();
	void HandleMenuInput();

private:
	// 전체 items 기준이 아니라 현재 보이는 항목 기준 선택 인덱스. 
	int currentVisibleIndex = 0;

	// 선택된 아이템의 색상.
	Color selectedColor = Color::Green;

	// 선택되지 않은 아이템의 색상.
	Color unselectedColor = Color::White;

	// 메뉴 아이템 배열. 
	std::vector<MenuItem*> items;

	// 보이는 MenuItem index 배열.
	std::vector<int> visibleItemIndices;


	// 고정 메뉴 인덱스.
	static constexpr int NewMenuItemIndex = 0;
	static constexpr int ResumeMenuItemIndex = 1;
	static constexpr int QuitMenuItemIndex = 2;
};

