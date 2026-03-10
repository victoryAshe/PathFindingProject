#include "Engine.h"
#include "Level/Level.h"
#include "Core/Input.h"
#include "Util/Util.h"

#include "Util/PathUtil.h"
#include "Render/Renderer.h"

#include <iostream>
#include <Windows.h>

namespace Wanted 
{
	// 전역 변수 초기화.
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		// 전역 변수 값 초기화.
		instance = this;

		// 입력 관리자 생성.
		input = new Input();

		// 설정 파일 load.
		LoadSetting();

		// Create Renderer instance 
		renderer = new Renderer(Vector2(setting.width, setting.height));

		// 커서 끄기.
		Util::TurnOffCursor();
	}

	Engine::~Engine()
	{
		// mainLevel 제거
		if (mainLevel)
		{
			// mainLevel 제거.
			delete mainLevel;
			mainLevel = nullptr;
		}

		// 입력 관리자 제거.
		if (input)
		{
			delete input;
			input = nullptr;
		}

		// Delete Renderer instance
		SafeDelete(renderer);
	}

	void Engine::Run()
	{
		// 시계의 정밀도.
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		// Frame 계산용 변수
		int64_t currentTime = 0;
		int64_t previousTime = 0;
		
		// Hardware Timer로 시간 구하기.
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		// Engine 시작 직전에는 두 시간 값을 동일하도록 맞춤.
		currentTime = time.QuadPart;
		previousTime = currentTime;

		// 기준 frame (단위: 초)
		setting.framerate
			= setting.framerate == 0.0f ? 60.0f : setting.framerate;
		float oneFrameTime = 1.0f / setting.framerate;

		// Engine Loop (Game Loop)
		while (!isQuit)
		{
			// 현재 시간 구하기.
			QueryPerformanceCounter(&time);
			currentTime = time.QuadPart;

			// Frame Time 계산
			float deltaTime
				= static_cast<float>(currentTime - previousTime);

			// 초 단위로 변환
			deltaTime = deltaTime
				/ static_cast<float>(frequency.QuadPart);

			// 고정 frame 기법.
			if (deltaTime >= oneFrameTime)
			{
				input->ProcessInput();

				// Frame 처리.
				BeginPlay();
				Tick(deltaTime);
				Draw();

				// 이전 시간 값 갱신.
				previousTime = currentTime;

				input->SavePreviousInputStates();
			
				// Level에 요청됨 Actor 추가/제거 처리.
				if (mainLevel)
				{
					mainLevel->ProcessAddAndDestroyActors();
					mainLevel->ProcessPendingUIElements();
				}

				// Level 전환 처리.
				if(nextLevel)
				{
					// 기존 Level 제거.
					SafeDelete(mainLevel);

					// 전환할 Level을 mainLevel로 지정.
					mainLevel = nextLevel;

					// pointer 정리.
					nextLevel = nullptr;
				}
			}

		}

		// 정리
		Shutdown();
	}

	void Engine::QuitEngine()
	{
		isQuit = true;
	}

	void Engine::SetNewLevel(Level* newLevel)
	{
		// 기존 레벨 있는지 확인.
		// 있으면 기존 레벨 제거.
		// TODO: 임시 코드. 레벨 전환할 때는 바로 제거하면 안됨!
		if (mainLevel)
		{
			delete mainLevel;
			mainLevel = nullptr;
		}

		mainLevel = newLevel;
	}

	Engine& Engine::Get()
	{
		// 예외처리.
		if (!instance)
		{
			// Silent is violent!
			std::cout << "Error: Engine::Get(). instance is null.\n";
			__debugbreak();
		}

		return *instance;
	}

	void Engine::Shutdown()
	{
		// 정리 작업.
		std::cout << "Engine has been shutdown....";

		// 커서 끄기.
		Util::TurnOnCursor();
	}

	void Engine::LoadSetting()
	{
		// Open Engine Setting File.
		FILE* file = nullptr;
		
		fopen_s(&file, "../Config/Setting.txt", "rt");

		// Exception Handling
		if (!file)
		{
			std::cout << "Failed to open Engine Setting file.\n";
			__debugbreak();
			return;
		}

		// file에서 읽은 data를 담을 buffer.
		char buffer[2048] = {};

		// read string size in file
		size_t readSize = fread(buffer, sizeof(char), 2048, file);

		// Parse string.
		char* context = nullptr;
		char* token = nullptr;
		token = strtok_s(buffer, "\n", &context);

		// 반복해서 자르기.
		while(token)
		{
			// setting text에서 parameter name만 읽기.
			char header[10] = {};
			
			// 문자열 읽기 함수 활용.
			// 이때 "%s"로 읽으면, space가 있을 시에 그 전까지 읽음.
			// strtok_s에 Delimiter를 ' '로 설정해서 읽어도 되긴 함.
			sscanf_s(token, "%s", header, 10);

			// 문자열 비교 및 값 읽기.
			if(strcmp(header, "framerate")==0)
			{
				sscanf_s(token, "framerate = %f", &setting.framerate);
			}
			else if (strcmp(header, "width") == 0)
			{
				sscanf_s(token, "width = %d", &setting.width);
			}
			else if (strcmp(header, "height") == 0)
			{
				sscanf_s(token, "height = %d", &setting.height);
			}

			// 개행 문자로 문자열 분리.
			token = strtok_s(nullptr, "\n", &context);
		}

		// Data extract using string format.
		sscanf_s(buffer, "framerate = %f", &setting.framerate);

		// Close File.
		fclose(file);
	}

	void Engine::BeginPlay()
	{
		// Level이 있으면 event 전달.
		if (!mainLevel)
		{
			// Silent is violent. 침묵은 폭력이다.
			// Log message 남기지 않으면 안된다!
			std::cout << "Error: Engine::BeginPlay(). mainLevel is empty.\n";
			return;
		}

		mainLevel->BeginPlay();

	}

	void Engine::Tick(float deltaTime)
	{
		//std::cout
		//	<< "DeltaTime: " << deltaTime
		//	<< ", FPS: " << (1.0f / deltaTime) << "\n";

		// Level에 event 흘리기.
		// 예외 처리.
		if (!mainLevel)
		{
			std::cout << "Error: Engine::Tick(). mainLevel is empty.\n";
			return;
		}

		mainLevel->Tick(deltaTime);
	}

	void Engine::Draw()
	{
		// Level에 event 흘리기.
		// 예외 처리.
		if (!mainLevel)
		{
			std::cout << "Error: Engine::Draw(). mainLevel is empty.\n";
			return;
		}

		// Level의 모든 Actor가 Render Data를 Submit.
		mainLevel->Draw();

		// Renderer에 Draw 명령 전달.
		renderer->Draw();     
	}
}