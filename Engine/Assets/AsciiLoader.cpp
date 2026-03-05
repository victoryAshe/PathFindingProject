#include "AsciiLoader.h"
#include "Util/PathUtil.h"
#include "Util/FileIO.h"
#include "Assets/AsciiLoader.h"

#include <sstream>
#include <iostream>

namespace Wanted 
{
	std::unordered_map<std::string, std::shared_ptr<const AsciiArt>> AsciiLoader::cache;
	std::shared_ptr<const AsciiArt> AsciiLoader::GetOrLoad(const std::string& actorName, const std::string& state)
	{
		// CacheHit: 이미 Load되어있는지 확인, 있으면: return.
		auto it = cache.find(actorName);
		if (it != cache.end()) return it->second;

		// 새로 Loaad.
		const std::string path = BuildSpritePath(actorName, state);
		
		if (!FileIO::FileExists(path))
		{
			std::cout << "[AsciiLoader] sprite.txt not found: \n" << path << "\n";
			__debugbreak();

			// 빈 shared_ptr return.
			return std::shared_ptr<const AsciiArt>();
		}

		const std::string text = FileIO::ReadAllTextFile(path);
		AsciiArt art = ParseTextToAsciiArt(text);

		// Text File Load는 성공했으나 AsciiArt가 invalid.
		if (!art.isValid())
		{
			std::cout << "[AsciiLoader] sprite.txt parsed invalid: \n" << path << "\n";
			__debugbreak();

			// 빈 shared_ptr return.
			return std::shared_ptr<const AsciiArt>();
		}

		// heap에 AsciiArt를 올려 공유!
		// shared_ptr에 new로 객체를 생성해서 넣어줄 때,
		// Deleter를 함수 포인터로 넣어주면 Reset될 때 해당 함수로 객체를 해제!
		std::shared_ptr<const AsciiArt> ptr = 
			std::make_shared<AsciiArt>(std::move(art));

		cache.emplace(actorName, ptr);

		// shared_ptr을 반환함을 명심할 것.
		return ptr;
	}
	
	void AsciiLoader::ClearCache()
	{
		cache.clear();
	}


	// ExeDir 기준으로, Assets\{ActorName}\{spriteName}.txt를 로드.
	/*
	* // TODO: {ActorName}\의 모든 파일을 이름을 key로 해서 unordered_map에 저장해,
	* // Animation으로 사용할 수 있도록 한다.
	*/
	std::string AsciiLoader::BuildSpritePath(const std::string& actorName, const std::string& state)
	{
		// {ExeDir}/Assets/{ActorName}/{spriteName}.txt.
		std::string path = PathUtil::GetExeDir();
		path = PathUtil::JoinPath(path, "AsciiArt");
		path = PathUtil::JoinPath(path, actorName);
		path = PathUtil::JoinPath(path, state+".txt");

		return path;
	}

	AsciiArt AsciiLoader::ParseTextToAsciiArt(const std::string& text)
	{
		std::vector<std::string> lines;
		lines.reserve(128);

		std::string line;
		std::istringstream iss(text);
		while (std::getline(iss, line))
		{
			if (!line.empty() && line.back() == '\n')
			{
				line.pop_back();
			}
			lines.push_back(line);
		}

		while (!lines.empty() && lines.back().empty())
		{
			lines.pop_back();
		}

		if (lines.empty()) return AsciiArt();

		int w = 0;
		for (size_t i = 0; i < lines.size(); ++i)
		{
			w = max(w, static_cast<int>(lines[i].size()));
		}

		const int h = static_cast<int>(lines.size());
		if (w <= 0 || h <= 0) return AsciiArt();

		AsciiArt art;
		art.width = w, art.height = h;
		art.pixels.assign(w * h, ' ');

		for (int y = 0; y < h; ++y)
		{
			const std::string& source = lines[y];
			const int copyCount = static_cast<int>(source.size());
			for (int x = 0; x < copyCount; ++x)
			{
				art.pixels[(y * w) + x] = source[x];
			}
		}

		return art;
	}
}