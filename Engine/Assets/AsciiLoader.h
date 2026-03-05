#pragma once

#include "Common/Common.h"
#include "AsciiArt.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace Wanted
{
	class WANTED_API AsciiLoader
	{
	public:
		static std::shared_ptr<const AsciiArt> GetOrLoad(const std::string& actorName, const std::string& state);

		// 성능 측정 시에 캐시 영향 제거용 => 함 볼 것.
		static void ClearCache();

	private:
		// Load 대상 File 경로.
		static std::string BuildSpritePath(const std::string& actorName, const std::string& state);
		static AsciiArt ParseTextToAsciiArt(const std::string& text);

	private:
		static std::unordered_map<std::string, std::shared_ptr<const AsciiArt>> cache;
	};
}