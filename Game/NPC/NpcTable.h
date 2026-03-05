#pragma once
#include "NpcRow.h"

#include <unordered_map>
#include <string>


// CSV Load.
// table: StageName => NPC Data.
class NpcTable
{
public:
	static bool LoadFromCSV(const std::string& path);
	const static bool TryGet(const std::string& stageName, NpcRow& out);

private:
	
private:
	static std::unordered_map<std::string, NpcRow> table;
};

