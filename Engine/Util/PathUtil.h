#pragma once

#include <string>
#include <Windows.h>


// 마지막 '\' || '/' 기준으로 Target Directory 추출.
namespace PathUtil
{
	// 실행 파일(.exe)이 위치한 directory 경로 반환. (마지막에 '\' 없이.)
	inline std::string GetExeDir()
	{
		char buffer[MAX_PATH] = {};
		const DWORD len = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
		if (len == 0 || len >= MAX_PATH) return {};

		std::string fullPath(buffer, buffer + len);

		// 마지막 '\' || '/' 기준으로 Directory 추출.
		const size_t pos = fullPath.rfind("\\");
		if (pos == std::string::npos) return{};


		return fullPath.substr(0, pos);
	}

	inline std::string JoinPath(const std::string& a, const std::string& b)
	{
		if (a.empty()) return b;
		if (b.empty()) return a;

		const char last = a.back(); // 안되면 a[a.size()-1] 시도.
		if (last == '\\' || last == '/') return a + b;
		
		return a + "\\" + b;
	}
}