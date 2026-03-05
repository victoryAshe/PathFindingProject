#include "Input.h"
#include "Engine/Engine.h"
#include "Util/Util.h"

#include <Windows.h>
#include <iostream>
#include <cassert>

namespace Wanted 
{ 
	// 전역 변수 초기화.
	Input* Input::instance = nullptr;

	Input::Input()
	{
		// 객체가 초기화되면 자기 자신의 주소를 저장.
		instance = this;
	}

	Input::~Input()
	{
		// memory 관리는 Engine에서 하기 때문에 따로 지울 필요는 없음.
	}

	void Input::ProcessInput()
	{
		// 입력 Handle 얻어오기 & Activate Mouse Input.
		static HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
		static bool initialized = false;

		if (!initialized)
		{
			// Activate Mouse Input.
			DWORD mode = ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
			mode &= ~ENABLE_QUICK_EDIT_MODE;
			BOOL result = SetConsoleMode(inputHandle, mode);

			// Exception Handling: failed to setting.
			if (result == FALSE)
			{
				// Print error message.
				MessageBoxA(
					nullptr,
					"Input::ProcessInput() - Failed to set console mode",
					"Error",
					MB_OK
				);

				__debugbreak();
			}

			initialized = true;
		}

		// keyboard/mouse input을 받기 위한 변수.
		const int recordCount = 256;

		// 수신한 InputEvent를 저장하기 위한 배열.
		INPUT_RECORD records[recordCount] = {};

		// 이번 frame에 처리한 InputEvent 수.
		DWORD eventReadCount = 0;

		// 들어온 InputEvnet가 있는지 확인.
		if (PeekConsoleInput(inputHandle, records, recordCount, &eventReadCount)
			&& eventReadCount > 0)
		{
			// InputEvent 존재: Event를 읽어서 처리.
			if (ReadConsoleInput(inputHandle, records, recordCount, &eventReadCount))
			{
				// 들어온 InputEvent의 수 만큼 loop.
				for (int ix = 0; ix < static_cast<int>(eventReadCount); ++ix)
				{
					INPUT_RECORD& record = records[ix];
					
					// Event type별로 처리.
					switch(record.EventType)
					{
					//KeyEvnet인 경우.
					case KEY_EVENT:
					{
						// Key Down 처리.
						if (record.Event.KeyEvent.bKeyDown)
						{
							keyStates[record.Event.KeyEvent.wVirtualKeyCode].isKeyDown = true;
						}
						// Key가 안 눌렸을 때.
						else
						{
							keyStates[record.Event.KeyEvent.wVirtualKeyCode].isKeyDown = false;
						}
					}
					break;


					// Mose Event 처리.
					case MOUSE_EVENT:
					{
						// Set Mouse xPosition
						mousePosition.x = record.Event.MouseEvent.dwMousePosition.X;

						// xPosition 조정: 화면 안에 고정되도록.
						mousePosition.x
							= Util::Clamp<int>(mousePosition.x, 0, Engine::Get().GetWidth() - 1);

						// yPosition 조정.
						mousePosition.y
							= Util::Clamp<int>(mousePosition.y, 0, Engine::Get().GetHeight() - 1);

						// Mouse Click 여부 상태 저장.
						keyStates[VK_LBUTTON].isKeyDown
							= (record.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;

						keyStates[VK_RBUTTON].isKeyDown
							= (record.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;

					}
					break;
					}
				}
			}
		}
	}

	void Input::SavePreviousInputStates()
	{
		// 현재 입력 값을 이전 입력 값으로 저장.
		for (int ix = 0; ix < 255; ++ix)
		{
			keyStates[ix].wasKeyDown = keyStates[ix].isKeyDown;
		}
	}

	bool Input::GetKeyDown(int keyCode)
	{
		return keyStates[keyCode].isKeyDown
			&& !keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetKeyUp(int keyCode)
	{
		return !keyStates[keyCode].isKeyDown
			&& keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetKey(int keyCode)
	{
		return keyStates[keyCode].isKeyDown;
	}

	bool Input::GetMouseButtonDown(int buttonCode)
	{
		// buttonCode가 0이거나 1인지 확인.
		// 이 검사를 통과 못하면 debugbreak가 동작함.
		assert(buttonCode == 0 || buttonCode == 1);

		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return keyStates[keyCode].isKeyDown
			&& !keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetMouseButtonUp(int buttonCode)
	{
		// buttonCode가 0이거나 1인지 확인.
		// 이 검사를 통과 못하면 debugbreak가 동작함.
		assert(buttonCode == 0 || buttonCode == 1);

		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return !keyStates[keyCode].isKeyDown
			&& keyStates[keyCode].wasKeyDown;
	}

	bool Input::GetMouseButton(int buttonCode)
	{
		// buttonCode가 0이거나 1인지 확인.
		// 이 검사를 통과 못하면 debugbreak가 동작함.
		assert(buttonCode == 0 || buttonCode == 1);
		
		int keyCode = 0;
		if (buttonCode == 0)
		{
			keyCode = VK_LBUTTON;
		}
		else if (buttonCode == 1)
		{
			keyCode = VK_RBUTTON;
		}

		return keyStates[keyCode].isKeyDown;
	}

	Input& Input::Get()
	{
		// Singleton.
		// 이 함수는 Contents Project에서 접근하므로,
		// Engine은 이미 초기화 완료 상태.
		if (!instance)
		{
			std::cout << "Error: Input::Get(). instance is null.\n";

			// Debug Mode에서만 동작!
			// 자동으로 중단점 걸림.
			__debugbreak();
		}

		// Lazy-Pattern.
		// Effective C++에 나오는 것.
		// 실행 시점 직전에 instance를 생성.
		// Engine에서는 Lazy-Pattern을 안씀!
		//static Input instance;
		//return instance;

		return *instance;
	}

}