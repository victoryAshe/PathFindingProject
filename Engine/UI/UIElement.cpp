#include "UI/UIElement.h"

#include "Render/Renderer.h"
#include "Level/Level.h"
#include "Util/Util.h"

#include <cstring>
#include <string>
#include <vector>

namespace Wanted
{
	UIElement::UIElement(
		const Vector2& localPosition,
		Color color,
		int sortingOrder,
		bool spaceTransparent)
		: localPosition(localPosition),
		color(color),
		sortingOrder(sortingOrder),
		spaceTransparent(spaceTransparent)
	{
	}

	UIElement::~UIElement()
	{
		ClearTextBuffer();
	}

	void UIElement::BeginPlay()
	{
	}

	void UIElement::Tick(float deltaTime)
	{
	}

	void UIElement::Draw()
	{
		if (!isVisible)
		{
			return;
		}

		OnDrawUI();
	}

	void UIElement::OnDrawUI()
	{


		if (!textBuffer || width <= 0 || height <= 0)
		{
			return;
		}

		const IntRect renderRect = owner ? owner->GetUIRect() : IntRect();

		Renderer::Get().Submit(
			textBuffer,
			width,
			height,
			localPosition,
			renderRect,
			color,
			sortingOrder,
			spaceTransparent
		);
		return;

	}

	void UIElement::SetLocalPosition(const Vector2& newLocalPosition)
	{
		if (localPosition == newLocalPosition)
		{
			return;
		}

		localPosition = newLocalPosition;
	}


	void UIElement::SetText(const char* newText)
	{
		CopySingleLineText(newText);
	}

	void UIElement::SetMultilineText(const char* newText)
	{
		CopyMultilineText(newText);
	}


	void UIElement::ClearTextBuffer()
	{
		SafeDeleteArray(textBuffer);
		width = 0;
		height = 0;
	}

	void UIElement::CopySingleLineText(const char* sourceText)
	{
		ClearTextBuffer();

		if (!sourceText)
		{
			return;
		}

		width = static_cast<int>(strlen(sourceText));
		height = 1;

		if (width <= 0)
		{
			return;
		}

		textBuffer = new char[width];
		memcpy(textBuffer, sourceText, width);
	}

	void UIElement::CopyMultilineText(const char* sourceText)
	{
		ClearTextBuffer();

		if (!sourceText)
		{
			return;
		}

		std::vector<std::string> lines;
		std::string currentLine;

		for (const char* p = sourceText; *p != '\0'; ++p)
		{
			if (*p == '\r')
			{
				continue;
			}

			if (*p == '\n')
			{
				lines.emplace_back(currentLine);
				currentLine.clear();
				continue;
			}

			currentLine.push_back(*p);
		}

		lines.emplace_back(currentLine);

		height = static_cast<int>(lines.size());
		width = 0;

		for (const std::string& line : lines)
		{
			const int lineLength = static_cast<int>(line.size());
			if (lineLength > width)
			{
				width = lineLength;
			}
		}

		if (width <= 0 || height <= 0)
		{
			ClearTextBuffer();
			return;
		}

		textBuffer = new char[width * height];
		memset(textBuffer, ' ', width * height);

		for (int y = 0; y < height; ++y)
		{
			const std::string& line = lines[y];
			if (line.empty())
			{
				continue;
			}

			memcpy(
				textBuffer + (y * width),
				line.c_str(),
				line.size()
			);
		}
	}
}