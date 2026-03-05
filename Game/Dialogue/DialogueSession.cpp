#include "DialogueSession.h"
#include "NPC/NPC.h"
#include "Util/MessageEvent.h"

#include "Dialogue/DialogueTable.h"
#include <string>

DialogueSession::DialogueSession(
	NPC* inOwnerNpc, 
	const std::string& csvPath, 
	int startDialogueID)
	: ownerNPC(inOwnerNpc),
	csvPath(csvPath),
	currentID(startDialogueID)
{
	// ManagerActor이므로 Render 안하도록 처리.
	isActive = false;
}

void DialogueSession::BeginPlay()
{
	super::BeginPlay();

	loaded = DialogueTable::LoadFromCSV(csvPath);
	if(!loaded)
	{
		MessageEvent::MessageOK(L"Dialogue Error", L"Failed to load dialogue CSV.");
		Destroy();
		return;
	}

	pendingShow = true;
}

void DialogueSession::Tick(float deltaTime)
{
	if (!loaded || !pendingShow) return;

	pendingShow = false;

	DialogueRow row;
	if (!DialogueTable::TryGet(currentID, row))
	{
		MessageEvent::MessageOK(L"Dialogue Error", L"DialogueID not found.");
		Destroy();
		return;
	}

	lastID = currentID;

	const int nextID = ShowAndGetNextDialogueID(row);
	if (nextID == 0)
	{
		if (ownerNPC && !ownerNPC->DestroyRequested())
		{
			ownerNPC->OnDialgoueSessionEnded(lastID);
		}
		Destroy();
		return;
	}

	currentID = nextID;
	pendingShow = true;
}

// 아무것도 해주지 않기.
void DialogueSession::Draw()
{
}

int DialogueSession::ShowAndGetNextDialogueID(const DialogueRow& row)
{
	// title: characterName, body: script.
	const WCHAR* title = row.character.c_str();
	const WCHAR* body = row.script.c_str();

	// eventID == 0: OK => jump1
	// eventID == 4: 2지선다 => jump1~2 분기.
	// eventID == 2: 3지선다 => jump1~3 분기.
	if (row.eventID == 0)
	{
		MessageEvent::MessageOK(title, body);
		return row.jump1;
	}

	if (row.eventID == 2)
	{
		const int choice = MessageEvent::MessageAbortTryIgnore(title, body);
		if (choice == 1) return row.jump1;
		if (choice == 2) return row.jump2;
		if (choice == 3) return row.jump3;

		// return 0: 닫힘이나 취소도 종료로 간주.
		return 0;
	}

	if (row.eventID == 4)
	{
		const int choice = MessageEvent::MessageYesNo(title, body);
		if (choice == 1) return row.jump1;
		if (choice == 2) return row.jump2;

		return 0;
	}

	// 그 외 Event는 OK 처리.
	MessageEvent::MessageOK(title, body);
	return row.jump1;
}
