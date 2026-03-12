#include "UI/UpgradeSelectionUI.h"

#include "Render/Renderer.h"
#include "Engine/GameEngine.h"

#include <algorithm>


UpgradeSelectionUI::UpgradeSelectionUI()
	: UIElement(
		Vector2::Zero,
		Color::White,
		200000,   // 월드/UI보다 위에 오도록 높은 sorting order
		false     // 공백도 덮어쓰도록 false
	)
{
	SetVisible(false); 
}

void UpgradeSelectionUI::SetChoices(const std::vector<PlayerUpgradeDefinition>& newChoices)
{
	choices = newChoices;
	RebuildLayoutText();
}

void UpgradeSelectionUI::ClearChoices()
{
	choices.clear(); 
	SetText("");   
}

void UpgradeSelectionUI::OnDrawUI()
{
	if (!IsVisible())
	{
		return;
	}

	if (!textBuffer || width <= 0 || height <= 0)
	{
		return;
	}

	// 기본 UIElement는 owner->GetUIRect()에 clip하지만,
	// 카드 선택 UI는 화면 중앙 overlay여야 하므로 전체 화면 기준으로 직접 Submit.
	Renderer::Get().Submit(
		textBuffer,
		width,
		height,
		localPosition,
		color,
		sortingOrder,
		spaceTransparent
	);
}

void UpgradeSelectionUI::RebuildLayoutText()
{
	// [추가]
	// 카드 3장을 가로로 배치하는 멀티라인 ASCII UI 생성
	const int panelWidth = 82;
	const int panelHeight = 13;

	std::vector<std::string> canvas(
		panelHeight,
		std::string(panelWidth, ' ')
	);

	// 바깥 패널 테두리
	canvas[0] = "+" + std::string(panelWidth - 2, '-') + "+";
	canvas[panelHeight - 1] = "+" + std::string(panelWidth - 2, '-') + "+";

	for (int y = 1; y < panelHeight - 1; ++y)
	{
		canvas[y][0] = '|';
		canvas[y][panelWidth - 1] = '|';
	}

	WriteText(canvas, 28, 1, "Choose Upgrade");
	WriteText(canvas, 19, 2, "Press 1, 2, or 3 to select a card");

	const int cardWidth = 24;
	const int cardHeight = 7;
	const int cardStartY = 4;

	// panel 내부에 3장 배치
	const int firstCardX = 2;
	const int gap = 2;

	for (int index = 0; index < static_cast<int>(choices.size()) && index < 3; ++index)
	{
		const int cardX = firstCardX + (index * (cardWidth + gap));
		BuildCardBlock(canvas, cardX, cardStartY, index, choices[index]);
	}

	// std::string으로 합치기
	std::string multilineText;
	for (int y = 0; y < panelHeight; ++y)
	{
		multilineText += canvas[y];
		if (y != panelHeight - 1)
		{
			multilineText += '\n';
		}
	}

	SetMultilineText(multilineText.c_str());

	// [추가] 화면 중앙에 위치시키기
	const int screenWidth = GameEngine::Get().GetWidth();
	const int screenHeight = GameEngine::Get().GetHeight();

	const int panelX = max(0, (screenWidth - panelWidth) / 2);
	const int panelY = max(0, (screenHeight - panelHeight) / 2);

	SetLocalPosition(Vector2(panelX, panelY));
}

void UpgradeSelectionUI::BuildCardBlock(
	std::vector<std::string>& canvas,
	int startX,
	int startY,
	int cardIndex,
	const PlayerUpgradeDefinition& upgrade
) const
{
	// [추가]
	const int cardWidth = 24;
	const int cardHeight = 7;

	// 상단/하단
	for (int x = 0; x < cardWidth; ++x)
	{
		canvas[startY][startX + x] = '-';
		canvas[startY + cardHeight - 1][startX + x] = '-';
	}

	// 좌/우
	for (int y = 0; y < cardHeight; ++y)
	{
		canvas[startY + y][startX] = '|';
		canvas[startY + y][startX + cardWidth - 1] = '|';
	}

	// 모서리
	canvas[startY][startX] = '+';
	canvas[startY][startX + cardWidth - 1] = '+';
	canvas[startY + cardHeight - 1][startX] = '+';
	canvas[startY + cardHeight - 1][startX + cardWidth - 1] = '+';

	// 카드 번호
	char indexText[8] = {};
	snprintf(indexText, sizeof(indexText), "[%d]", cardIndex + 1);
	WriteText(canvas, startX + 2, startY + 1, indexText);

	// 카드 제목 / 설명
	WriteText(canvas, startX + 6, startY + 1, upgrade.title);
	WriteText(canvas, startX + 2, startY + 3, upgrade.description);
}

void UpgradeSelectionUI::WriteText(
	std::vector<std::string>& canvas,
	int x,
	int y,
	const char* text
) const
{
	if (!text)
	{
		return;
	}

	if (y < 0 || y >= static_cast<int>(canvas.size()))
	{
		return;
	}

	std::string& row = canvas[y];

	for (int i = 0; text[i] != '\0'; ++i)
	{
		const int targetX = x + i;

		if (targetX < 0 || targetX >= static_cast<int>(row.size()))
		{
			continue;
		}

		row[targetX] = text[i];
	}
}