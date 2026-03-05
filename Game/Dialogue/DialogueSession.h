#pragma once

#include "Actor/Actor.h"
#include "Dialogue/DialogueRow.h"
#include <string>

/*
* NPC가 생성하는 대화 진행 Session Actor.
* - CSV Load 이후 DialogueID 기반으로 1개씩 표시
* - 선택 결과에 따라 대화 jump.
* - 종료 시 NPC에 NotifyDialogueEnded() 통지.
* - TODO: Session을 Level로 전환.
*/

class NPC;

using namespace Wanted;

class DialogueSession: public Actor
{
	RTTI_DECLARATIONS(DialogueSession, Actor)

public:
	DialogueSession(NPC* inOwnerNpc, const std::string& csvPath, int startDialogueID);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override; // Renderering X.

private:
	int ShowAndGetNextDialogueID(const DialogueRow& row);

private:
	NPC* ownerNPC = nullptr;

	std::string csvPath;

	int currentID = 0;
	bool loaded = false;

	// MeassageBox를 한 frame에 한 단계만 처리하기 위함.
	bool pendingShow = true;

	// 마지막으로 표시했던 dialogueID.
	int lastID = 0;
};

