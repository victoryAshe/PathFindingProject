#include "NpcTable.h"
#include "Util/CsvReader.h"

using namespace Wanted;

std::unordered_map<std::string, NpcRow> NpcTable::table;

bool NpcTable::LoadFromCSV(const std::string& path)
{
	std::vector<std::vector<std::string>> rows;
	if (!CsvReader::ReadAll(path, rows)) return false;

	bool skipHeader = true;
	for (const std::vector<std::string>& fields : rows)
	{
		if (fields.empty()) continue;

		if (skipHeader)
		{
			skipHeader = false;
			continue;
		}

		// stageName, npcName, dialogueStartID, dialogueEndID
		if (fields.size() < 4) continue;

		NpcRow row;
		row.StageName = fields[0];
		row.npcName = fields[1];
		row.dialogueStartID = CsvReader::ToInt(fields[2]);
		row.dialogueEndID = CsvReader::ToInt(fields[3]);

		table[row.StageName] = row;
	}

	return true;
}

const bool NpcTable::TryGet(const std::string& stageName, NpcRow& out)
{
	auto it = table.find(stageName);
	if (it == table.end()) return false;

	out = it->second;
	return true;
}