#include "EnemyLevelDefinitionTable.h"
#include "Util/CsvReader.h"

using namespace Wanted;

std::unordered_map<int, EnemyLevelDefinitionRow> EnemyLevelDefinitionTable::table;

bool EnemyLevelDefinitionTable::LoadFromCSV(const std::string& path)
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

		// EnemyLevel, MinHp, MaxHp
		if (fields.size() < 3)
		{
			continue;
		}

		EnemyLevelDefinitionRow row;
		row.enemyLevel = CsvReader::ToInt(fields[0]);
		row.minHp = CsvReader::ToInt(fields[1]);
		row.maxHp = CsvReader::ToInt(fields[2]);

		table[row.enemyLevel] = row;
	}

	return true;
}

bool EnemyLevelDefinitionTable::TryGet(int enemyLevel, EnemyLevelDefinitionRow& out)
{
	const auto it = table.find(enemyLevel);
	if (it == table.end())
	{
		return false;
	}

	out = it->second;
	return true;
}