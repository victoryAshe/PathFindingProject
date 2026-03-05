#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"

namespace Wanted 
{
	class WANTED_API Input
	{		
		friend class Engine;

		// Data.
		// Key에 대하여: Down/Up/Key
		struct KeyState
		{
			// 현재 키가 눌렸는지.
			bool isKeyDown = false;

			// 이전에 키가 눌렸는지.
			bool wasKeyDown = false;
		};

	private:
		Input();
		~Input();

	public:
		// ==== 키보드 입력 확인 함수. ====
		// 이전에 입력이 안됐는데, 현재 입력이 된 경우 1번 호출
		bool GetKeyDown(int keyCode);
		
		// 이전에 입력이 됐는데, 현재 입력이 취소된 경우 1번 호출.
		bool GetKeyUp(int keyCode);
		
		// 현재 눌려있으면 반복 호출.
		bool GetKey(int keyCode);

		// ==== 마우스 버튼 입력 확인 함수. ====
		// 바튼 클릭 확인.
		// buttonCode:0 -> left, 1-> right.
		bool GetMouseButtonDown(int buttonCode);

		// 마우스 버튼 클릴 해제 여부 확인.
		bool GetMouseButtonUp(int buttonCode);

		// 마우스 눌림 상태 확인.
		bool GetMouseButton(int buttonCode);

		// 전역적으로 접근하는 함수.
		static Input& Get();

		// Mouse Cursor 위치 확인 함수.
		inline Vector2 MousePosition() const { return mousePosition; }

	private:
		// 입력 처리 함수.
		void ProcessInput();

		// 기존 입력 저장 함수.
		void SavePreviousInputStates();

	private:
		// Key State 저장용 배열.
		KeyState keyStates[255] = {};

		// Mouse Cursor 위치.
		Vector2 mousePosition;

		// 전역적으로 접근 가능하도록 만들기 위한 전역 변수.
		static Input* instance;

	};
}