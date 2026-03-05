#include "UI.h"
#include "Render/Renderer.h"

namespace Wanted
{

	UI::UI(
		const std::string asciiUIName,
		const Vector2& position,
		Color color,
		bool spaceTransparent)
		: position(position), color(color), spaceTransparent(spaceTransparent)
	{
		SetAsciiArtByName(asciiUIName);
	}

	UI::~UI()
	{

	}

	void UI::Tick(float deltaTime)
	{

	}

	void UI::Draw()
	{
		// shared_ptr을 가진 AsciiArtActor
		// Renderer에 shared_ptr을 넘겨서 RenderQueue가 자원 수명 보장.
		if (sharedArt && sharedArt->isValid())
		{
			Renderer::Get().Submit(
				sharedArt,
				position,
				color,
				sortingOrder,
				spaceTransparent
			);
			return;
		}
	}

	// AsciiArt를 Name을 통해 불러옴.
	void UI::SetAsciiArtByName(const std::string& stateName)
	{
		sharedArt = AsciiLoader::GetOrLoad(asciiUIName, stateName);

		if (sharedArt && sharedArt->isValid())
		{
			width = sharedArt->width;
			height = sharedArt->height;
		}
	}

	void UI::SetPosition(const Vector2& newPosition)
	{
		// 변경하려는 위치가 현재 위치와 같으면 건너뜀.
		if (position == newPosition)
		{
			return;
		}

		// 새로운 위치 설정.
		position = newPosition;
		position = newPosition;
	}
}