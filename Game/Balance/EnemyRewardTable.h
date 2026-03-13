#pragma once

#include "EnemyRewardRow.h"

#include <string>
#include <unordered_map>

class EnemyRewardTable
{
public:
	static bool LoadFromCSV(const std::string& path);
	static bool TryGet(EnemyType enemyType, int enemyLevel, EnemyRewardRow& out);

private:
	static int MakeRewardKey(EnemyType enemyType, int enemyLevel);
	static bool TryParseEnemyType(const std::string& value, EnemyType& outEnemyType);

private:
	static std::unordered_map<int, EnemyRewardRow> table;


	static const std::unordered_map<std::string, EnemyType> enemyTypeMap;

};