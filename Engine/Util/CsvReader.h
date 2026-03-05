#pragma once
#include "Common/Common.h"

#include <string>
#include <vector>
#include <Windows.h>

// CSV 한 줄을 quote 규칙으로 parsing하는 util.
// quote: 쉼표로 구분, 필드 내부 쉼표가 있다면 "" escape 포함.

namespace Wanted
{
	class WANTED_API CsvReader
	{
	public: 
		static std::vector<std::string> ParseLine(const std::string& inLine);
		static bool ReadAll(
			const std::string& path, 
			std::vector<std::vector<std::string>>& outRows
		);

		static int ToInt(const std::string& s);
		static inline std::wstring UTF8toWide(const std::string& utf8);
		static UINT ParseEventID(const std::string& s);
	private:
		static void TrimCR(std::string& s);
	};

}

