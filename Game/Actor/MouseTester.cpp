#include "MouseTester.h"
#include "Core/Input.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"
#include <iostream>

MouseTester::MouseTester()
	: super("Mouse: ", Vector2(Engine::Get().GetWidth() / 2, Engine::Get().GetHeight() - 1))
{
	
}

void MouseTester::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 마우스 입력 테스트.
	if (Input::Get().GetMouseButton(0))
	{
	
		//ChangeImage("L");
		Vector2 mousePosition = Input::Get().MousePosition();
		char buffer[256] = {};
		sprintf_s(
			buffer,
			256,
			"cursor: (%d, %d)",
			mousePosition.x,
			mousePosition.y
		);
		
		ChangeImage(buffer);
	}
}
