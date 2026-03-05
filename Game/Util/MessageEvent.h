#pragma once

#include <Windows.h>

namespace MessageEvent
{
	// 0: 0x00000000L
	inline void MessageOK(const WCHAR* title, const WCHAR* message)
	{
		MessageBoxW(nullptr,
			message,
			title,
			MB_OK | MB_ICONSTOP
		);
	}

	// 4: 0x00000004L
	inline int MessageYesNo(const WCHAR* title, const WCHAR* message)
	{
		int ret = MessageBoxW(nullptr,
			message,
			title,
			MB_YESNO | MB_ICONSTOP
		);
		if (ret == IDYES) return 1;
		if (ret == IDNO) return 2;
		return 0;
	}

	// 2: 0x00000002L
	inline int MessageAbortTryIgnore(const WCHAR* title, const WCHAR* message)
	{
		int ret = MessageBoxW(nullptr,
			message,
			title,
			MB_ABORTRETRYIGNORE | MB_ICONWARNING
		);

		if (ret == IDABORT) return 1;
		if (ret == IDRETRY) return 2;
		if (ret == IDIGNORE) return 3;
		return 0;
	}
}