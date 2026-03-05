#pragma once

#include <fstream>
#include <sstream>

namespace FileIO
{
	inline bool FileExists(const std::string& path)
	{
		std::ifstream ifs(path.c_str());
		return ifs.good();
	}

	inline std::string ReadAllTextFile(const std::string& path)
	{
		// 위와 비슷한 구문이지만 ifs를 아래서도 쓰므로,
		// !FileExists(path)로 쓸 수 없어서 다시 써야함.
		std::ifstream ifs(path, std::ios::in | std::ios::binary);
		if (!ifs.is_open()) return {};

		std::ostringstream oss;
		oss << ifs.rdbuf();
		return oss.str();
	}
}