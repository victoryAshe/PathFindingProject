#pragma once

#include "PlayerLevelProgressionRow.h"

#include <string>
#include <unordered_map>

class PlayerLevelProgressionTable
{
public:
	static bool LoadFromCSV(const std::string& path);
	static bool TryGet(int level, PlayerLevelProgressionRow& out);

private:
	static std::unordered_map<int, PlayerLevelProgressionRow> table;
};