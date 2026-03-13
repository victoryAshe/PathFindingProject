#include "PlayerLevelProgressionTable.h"
#include "Util/CsvReader.h"

using namespace Wanted;

std::unordered_map<int, PlayerLevelProgressionRow> PlayerLevelProgressionTable::table;

bool PlayerLevelProgressionTable::LoadFromCSV(const std::string& path)
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

		// Level, RequiredExp
		if (fields.size() < 2)
		{
			continue;
		}

		PlayerLevelProgressionRow row;
		row.level = CsvReader::ToInt(fields[0]);
		row.requiredExp = CsvReader::ToInt(fields[1]);

		table[row.level] = row;
	}

	return true;
}

bool PlayerLevelProgressionTable::TryGet(int level, PlayerLevelProgressionRow& out)
{
	const auto it = table.find(level);
	if (it == table.end())
	{
		return false;
	}

	out = it->second;
	return true;
}
