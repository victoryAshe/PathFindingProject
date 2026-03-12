#pragma once

#include "Common/Common.h"
#include "Math/Vector2.h"

namespace Wanted
{
	// 전방 선언
	class Input;
	class Renderer;

	// Engine Setting Structure.
	struct EngineSetting
	{
		// Frame spped(== Framerate.)
		float framerate = 0.0f;

		// ScreenSize.
		int width = 0;
		int height = 0;
	};

	// Main game engine class.
	class WANTED_API Engine
	{
	public:
		Engine();
		virtual ~Engine();

		// Engine Loop(Game Loop)
		void Run();

		// Engine 종료 함수.
		void QuitEngine();

		// 새 level을 추가(설정)하는 함수.
		void SetNewLevel(class Level* newLevel);

		// 디버그 표시용 FPS / FrameTime 조회
		inline float GetDisplayedFps() const { return displayedFps; }
		inline float GetDisplayedFrameTimeMs() const { return displayedFrameTimeMs; }

		// 화면 너비 반환 함수.
		inline int GetWidth() const { return setting.width; }
		
		// 화면 높이 반환 함수.
		inline int GetHeight() const { return setting.height; }

		Vector2 const GetScreenSize() { return Vector2(setting.width, setting.height); }

		// 전역 접근 함수.
		static Engine& Get();


	protected:

		// 정리 함수.
		void Shutdown();

		// Load Setting File.
		void LoadSetting();

		// GamePlay 시작 함수.
		// Unity의 경우: Start/Awake.
		void BeginPlay();

		// Update 함수
		void Tick(float deltaTime);

		// 그리기 함수. (Draw/Render).
		void Draw();

		// 디버그 통계 갱신
		void UpdateFrameStatistics(float frameInterval);

	protected:
		// Engine 종료 Flag
		bool isQuit = false;

		// Engine Setting value.
		EngineSetting setting;

		// 입력 관리자.
		Input* input = nullptr;

		// Renderer instance.
		Renderer* renderer = nullptr;

		// Main Level
		class Level* mainLevel = nullptr;

		// 전환될 Level을 임시 저장(caching)하는 변수.
		class Level* nextLevel = nullptr;

		// FPS 표시용 누적=> 평균 통계.
		float displayedFps = 0.0f;
		float displayedFrameTimeMs = 0.0f;

		float frameStatAccumulatedTime = 0.0f;
		int frameStatAccumulatedCount = 0;

		// displayedFPS를 0.25초 평균으로 갱신하기 위한 static float.
		static constexpr float frameStatUpdateInterval = 0.25f;


		// 전역 변수.
		static Engine* instance;
	};


}

