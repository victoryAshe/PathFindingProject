#include "EnemyRewardTable.h"
#include "Util/CsvReader.h"

using namespace Wanted;

std::unordered_map<int, EnemyRewardRow> EnemyRewardTable::table;

const std::unordered_map<std::string, EnemyType> EnemyRewardTable::enemyTypeMap =
{
	{ "Normal", EnemyType::Normal },
	{ "Tank", EnemyType::Tank },
	{ "Fast", EnemyType::Fast },
	{ "Elite", EnemyType::Elite }
};

int EnemyRewardTable::MakeRewardKey(EnemyType enemyType, int enemyLevel)
{
	return (static_cast<int>(enemyType) * 1000) + enemyLevel;
}

bool EnemyRewardTable::TryParseEnemyType(const std::string& value, EnemyType& outEnemyType)
{
	const auto it = enemyTypeMap.find(value);
	if (it == enemyTypeMap.end())
	{
		return false;
	}

	outEnemyType = it->second;
	return true;
}

bool EnemyRewardTable::LoadFromCSV(const std::string& path)
{
	std::vector<std::vector<std::string>> rows;
	if (!CsvReader::ReadAll(path, rows))
	{
		return false;
	}

	table.clear();

	bool skipHeader = true;
	for (const std::vector<std::string>& fields : rows)
	{
		if (fields.empty())
		{
			continue;
		}

		if (skipHeader)
		{
			skipHeader = false;
			continue;
		}

		// EnemyType, EnemyLevel, ExpReward, GoldReward, ScoreReward.
		if (fields.size() < 5)
		{
			continue;
		}

		EnemyType parsedEnemyType;
		if (!TryParseEnemyType(fields[0], parsedEnemyType))
		{
			continue;
		}

		EnemyRewardRow row;
		row.enemyType = parsedEnemyType;
		row.enemyLevel = CsvReader::ToInt(fields[1]);
		row.expReward = CsvReader::ToInt(fields[2]);
		row.goldReward = CsvReader::ToInt(fields[3]);
		row.scoreReward = CsvReader::ToInt(fields[4]);

		table[MakeRewardKey(row.enemyType, row.enemyLevel)] = row;
	}

	return true;
}

bool EnemyRewardTable::TryGet(EnemyType enemyType, int enemyLevel, EnemyRewardRow& out)
{
	const auto it = table.find(MakeRewardKey(enemyType, enemyLevel));
	if (it == table.end())
	{
		return false;
	}

	out = it->second;
	return true;
}