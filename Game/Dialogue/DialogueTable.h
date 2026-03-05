#pragma once
#include "DialogueRow.h"
#include <unordered_map>
#include <string>


// CSV Load.
// table: DialogueID => DialogueRow의 저장소.
class DialogueTable
{
public:
	static bool LoadFromCSV(const std::string& path);
	const static bool TryGet(int dialougeID, DialogueRow& out);

private:
	

private:
	static std::unordered_map<int, DialogueRow> table;
};

