// Copyright (C) 2004-2017 Yohann Nicolas
// Copyright (C) 2017-2018 Jonathan Vasquez <jon@xyinn.org>
//
// This program is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "UpdateServer.h"
#include "InfinityStash.h"
#include "Commands.h"
#include "Common.h"

int renameIndex = 0;
char renameString[16];
DWORD PageSwap;
DWORD PageSelect;

int __stdcall handleServerUpdate(Unit* ptChar, WORD param)
{
	int type = param & 0xFF;
	DWORD arg = (param & 0xFF00) >> 8;
	log_msg("Received custom message: type=%i, arg=%i\n", type, arg);
	switch(type)
	{
		case US_SET_INDEX:				setCurrentStashIndex(ptChar, 1); return 1;
		case US_UNSET_INDEX:			setCurrentStashIndex(ptChar, 0); return 1;
		case US_SELECT_PREVIOUS :		selectPreviousStash( ptChar ); return 1;
		case US_SELECT_NEXT :			selectNextStash( ptChar ); return 1;
		case US_SELECT_PREVIOUS_JUMP:   selectPreviousStashJump(ptChar); return 1;
		case US_SELECT_NEXT_JUMP:       selectNextStashJump(ptChar); return 1;
		case US_SELECT_PREVIOUS_INDEX :	selectPreviousIndexStash( ptChar ); return 1;
		case US_SELECT_NEXT_INDEX :		selectNextIndexStash( ptChar ); return 1;
		case US_SELECT_PAGE3:           PageSelect = arg << 24; return 1;
		case US_SELECT_PAGE2:           PageSelect |= arg << 16; return 1;
		case US_SELECT_PAGE1:           PageSelect |= arg << 8; return 1;
		case US_SELECT_PAGE:            jumpToPage(ptChar, PageSelect | arg); PageSelect = 0; return 1;
		case US_SWAP3 :					PageSwap = arg << 24; return 1;
		case US_SWAP2 :					PageSwap |= arg << 16; return 1;
		case US_SWAP1 :					PageSwap |= arg << 8; return 1;
		case US_SWAP:					swapStash(ptChar, PageSwap | arg); PageSwap = 0; return 1;
		case US_PAGENAME:
			if (renameIndex == 0)
				for (int i = 0; i < 16; i++)
					renameString[i] = 0;
			renameString[renameIndex++] = (char)arg;
			if (arg == 0)
			{
				renameIndex = 0;
				log_msg("Rename current page on Server : %s -> %s\n", PCPY->currentStash->name, renameString);
				renameCurrentStash(ptChar, renameString);
			}
			return 1;
		default :
			return 0;
	}
}

FCT_ASM(caller_handleServerUpdate_9)
	XOR EDX, EDX
	MOV DX, WORD PTR DS : [EAX + 1]
	PUSH ECX
	PUSH EDX
	PUSH EDX
	PUSH ECX
	CALL handleServerUpdate
	POP EDX
	POP ECX
	TEST EAX, EAX
	JNZ END_RCM
	RETN
END_RCM:
	POP EAX
	XOR EAX, EAX
	RETN 8
	}}

void Install_UpdateServer()
{
	static bool isInstalled = false;
	if (isInstalled) return;

	log_msg("[Patch] Receive Button Clicks\n");

	DWORD ManageButtonClickMessageFromClientOffset = D2Game::GetAddress(0x4A702);

	// Manage button click message from Client.
	Memory::SetCursor(ManageButtonClickMessageFromClientOffset);
	Memory::ChangeByte(0x33, 0xE8);
	Memory::ChangeCallA(0x508B66D2, (DWORD)caller_handleServerUpdate_9);
	Memory::ChangeByte(0x01, 0x90);

	if (active_logFileMemory) log_msg("\n");
	isInstalled = true;
}