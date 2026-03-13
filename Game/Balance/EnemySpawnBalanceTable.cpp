#include "EnemySpawnBalanceTable.h"
#include "Util/CsvReader.h"

using namespace Wanted;

std::unordered_map<int, EnemySpawnBalanceRow> EnemySpawnBalanceTable::table;

bool EnemySpawnBalanceTable::LoadFromCSV(const std::string& path)
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

		// PlayerLevel, MinEnemyLevel, MaxEnemyLevel
		if (fields.size() < 3)
		{
			continue;
		}

		EnemySpawnBalanceRow row;
		row.playerLevel = CsvReader::ToInt(fields[0]);
		row.minEnemyLevel = CsvReader::ToInt(fields[1]);
		row.maxEnemyLevel = CsvReader::ToInt(fields[2]);

		table[row.playerLevel] = row;
	}

	return true;
}

bool EnemySpawnBalanceTable::TryGet(int playerLevel, EnemySpawnBalanceRow& out)
{
	const auto it = table.find(playerLevel);
	if (it == table.end())
	{
		return false;
	}

	out = it->second;
	return true;
}