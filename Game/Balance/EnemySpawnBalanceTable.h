#pragma once

#include "EnemySpawnBalanceRow.h"

#include <string>
#include <unordered_map>

class EnemySpawnBalanceTable
{
public:
	static bool LoadFromCSV(const std::string& path);
	static bool TryGet(int playerLevel, EnemySpawnBalanceRow& out);

private:
	static std::unordered_map<int, EnemySpawnBalanceRow> table;
};