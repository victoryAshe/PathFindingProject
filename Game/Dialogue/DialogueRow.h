#pragma once

#include <string>
#include <Windows.h>

struct DialogueRow
{
	int dialogueID = 0;
	UINT eventID = 0;

	std::wstring script;
	std::wstring character;

	int jump1 = 0;
	int jump2 = 0;
	int jump3 = 0;
};