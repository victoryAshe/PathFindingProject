#include "NPC.h"
#include "Level/Level.h"

#include "Util/MessageEvent.h"

#include "Dialogue/DialogueSession.h"


NPC::NPC(const Vector2& newPosition,
	const char* npcName,
	const char* dialogueCsvPath,
	int startDialougeID,
	int endDialougeID
	)
	:super("N", newPosition, Color::Red), 
	npcName(npcName ? npcName : ""), 
	dialogueCsvPath(dialogueCsvPath), 
	startDialogueID(startDialougeID),
	endDialogueID(endDialougeID)
{
	// Set SortingOrder
	sortingOrder = 8;
}

void NPC::PlayCollisionEvent()
{
	// 이미 대화 중이면 || 대화가 잘 끝났으면 재발동 금지.
	if (isDialogueActive || isDialogueCompleted) return;

	StartDialogueSession();
}

void NPC::OnDialgoueSessionEnded(int lastDialogueID)
{
	// Flag 해제: Session이 끝나면 다시 접촉 가능하도록.
	isDialogueActive = false;

	// EndID로 잘 끝났는지 판단.
	isDialogueCompleted = (lastDialogueID == endDialogueID);
	if (isDialogueCompleted==true)
	{
		Destroy();
	}
}


void NPC::StartDialogueSession()
{
	Level* level = GetOwner();
	if (!level) return;

	isDialogueActive = true;

	// 1회성 session Actor를 생성해서 Level에 추가.
	// Session이 끝나면 NPC->NotifyDialogueEnded() 호출 후 자기자신을 Destroy.
	level->AddNewActor(new DialogueSession(this, dialogueCsvPath, startDialogueID));
}



