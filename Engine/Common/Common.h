#pragma once
#pragma warning(disable: 4251)  // 미봉책: Actor*이 Warning을 띄우기 때문에 해당 Warning을 비활성화처리. 어쩔 수 없이 써야함.
#pragma warning(disable: 4172)  // Warning을 무시해도 그 위험성은 인지한 상태로 사용할 것.

#define DLLEXPORT   __declspec(dllexport)
#define DLLIMPORT   __declspec(dllimport)

// ENGINE_BUILD_DLL이 선언되어 있는지에 따라 분기.
#ifdef ENGINE_BUILD_DLL
#define WANTED_API DLLEXPORT
#else
#define WANTED_API DLLIMPORT
#endif