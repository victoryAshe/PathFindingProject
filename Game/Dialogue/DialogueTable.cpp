#include "DialogueTable.h"
#include "Util/CsvReader.h"

using namespace Wanted;

std::unordered_map<int, DialogueRow> DialogueTable::table;

bool DialogueTable::LoadFromCSV(const std::string& path)
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

        // dialogueID, EventID, script, character, JumpAnswer1, JumpAnsewer2, JumpAnswer3.
        if (fields.size() < 7) continue;

        DialogueRow row;
        row.dialogueID = CsvReader::ToInt(fields[0]);
        row.eventID = CsvReader::ParseEventID(fields[1]);
        row.script = CsvReader::UTF8toWide(fields[2]);
        row.character = CsvReader::UTF8toWide(fields[3]);
        row.jump1 = CsvReader::ToInt(fields[4]);
        row.jump2 = CsvReader::ToInt(fields[5]);
        row.jump3 = CsvReader::ToInt(fields[6]);

        table[row.dialogueID] = row;
    }

    return true;
}

const bool DialogueTable::TryGet(int dialougeID, DialogueRow& out)
{
    auto it = table.find(dialougeID);
    if (it == table.end()) return false;

    out = it->second;
    return true;
}
