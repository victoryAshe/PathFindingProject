#pragma once

#include "UI/UIElement.h"
#include "Upgrade/PlayerUpgradeDefinition.h"

#include <vector>
#include <string>

using namespace Wanted;

// 카드 3장을 화면 중앙에 오버레이로 표시하는 UI.
// 기존 UIElement 기본 draw는 UIRect에 clip되므로,
// 이 클래스는 OnDrawUI()를 override해서 전체 화면 기준으로 직접 Submit한다.
class UpgradeSelectionUI : public UIElement
{
	RTTI_DECLARATIONS(UpgradeSelectionUI, UIElement)

public:
	UpgradeSelectionUI();

	void SetChoices(const std::vector<PlayerUpgradeDefinition>& newChoices);
	void ClearChoices();

protected:
	virtual void OnDrawUI() override;

private:
	void RebuildLayoutText();
	void BuildCardBlock(
		std::vector<std::string>& canvas,
		int startX,
		int startY,
		int cardIndex,
		const PlayerUpgradeDefinition& upgrade
	) const;

	void WriteText(
		std::vector<std::string>& canvas,
		int x,
		int y,
		const char* text
	) const;

private:
	std::vector<PlayerUpgradeDefinition> choices;
};