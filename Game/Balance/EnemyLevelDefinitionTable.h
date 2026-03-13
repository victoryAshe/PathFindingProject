#pragma once

#include "EnemyLevelDefinitionRow.h"

#include <string>
#include <unordered_map>

class EnemyLevelDefinitionTable
{
public:
	static bool LoadFromCSV(const std::string& path);
	static bool TryGet(int enemyLevel, EnemyLevelDefinitionRow& out);

private:
	static std::unordered_map<int, EnemyLevelDefinitionRow> table;
};