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
#include "UpdateClient.h"
#include "InfinityStash.h"
#include "InterfaceStash.h"
#include "Common.h"

#define MAX_CMD_SIZE 200

// Stash Management Commands
const char* CMD_RENAME_PAGE = "/rename";
const char* CMD_SET_INDEX = "/set";
const char* CMD_UNSET_INDEX = "/unset";
const char* CMD_SELECT_PAGE = "/page";
const char* CMD_SWAP = "/swap";

int __stdcall commands(char* ptText)
{
	log_msg("Command : %s\n", ptText);
	Unit* ptChar = D2Client::D2GetClientPlayer();

	char command[MAX_CMD_SIZE];
	ZeroMemory(command, MAX_CMD_SIZE);
	strncpy(command, ptText, MAX_CMD_SIZE - 1);
	_strlwr(command);

	// The order matters here in terms of the rename commands because
	// the code currently checks to see if the string matches a string
	// up to a certain point. Thus /rename##### will match the first match.
	// Just keep this in mind.
	if (!strncmp(command, CMD_RENAME_PAGE, strlen(CMD_RENAME_PAGE)))
	{
		char* param = &ptText[strlen(CMD_RENAME_PAGE)];
		Stash* ptStash = PCPY->currentStash;
		if (!ptStash) return 0;

		int len = strlen(param);

		while (len > 0 && param[0] == ' ')
		{
			param++;
			len--;
		}

		if (len > 0 && len <= 15)
		{
			log_msg("Rename current page on Client : %s -> %s\n", ptStash->name, param);
			renameCurrentStash(ptChar, param);
			for (int i = 0; i <= len; i++)
				updateServer(US_PAGENAME + (param[i] << 8));
		}

		else if (len == 0)
		{
			log_msg("Rename current page on Client: %s\n", ptStash->name);
			renameCurrentStash(ptChar, NULL);
			updateServer(US_PAGENAME);
		}

		return 0;
	}

	if (!strcmp(command, CMD_SET_INDEX))
	{
		updateServer(US_SET_INDEX);
		return 0;
	}

	if (!strcmp(command, CMD_UNSET_INDEX))
	{
		updateServer(US_UNSET_INDEX);
		return 0;
	}

	if (!strncmp(command, CMD_SELECT_PAGE, strlen(CMD_SELECT_PAGE)))
	{
		int page = atoi(&command[strlen(CMD_SELECT_PAGE)]) - 1;
		if (!isStashPageValid(page)) return 1;

		updateServer(US_SELECT_PAGE3 + ((page & 0xFF000000) >> 16));
		updateServer(US_SELECT_PAGE2 + ((page & 0xFF0000) >> 8));
		updateServer(US_SELECT_PAGE1 + (page & 0xFF00));
		updateServer(US_SELECT_PAGE + ((page & 0xFF) << 8));

		return 0;
	}

	if (!strncmp(command, CMD_SWAP, strlen(CMD_SWAP)))
	{
		int page = atoi(&command[strlen(CMD_SWAP)]) - 1;
		if (!isStashPageValid(page)) return 1;

		updateServer(US_SWAP3 + ((page & 0xFF000000) >> 16));
		updateServer(US_SWAP2 + ((page & 0xFF0000) >> 8));
		updateServer(US_SWAP1 + (page & 0xFF00));
		updateServer(US_SWAP + ((page & 0xFF) << 8));

		return 0;
	}

	return 1;
}

FCT_ASM(caller_Commands)
	TEST EAX, EAX
	JE MANAGESOUNDCHAOSDEBUG
	PUSH DWORD PTR SS : [ESP + 0x1C]
	CALL commands
	TEST EAX, EAX
	JNZ MANAGESOUNDCHAOSDEBUG
	ADD DWORD PTR SS : [ESP] , 7
MANAGESOUNDCHAOSDEBUG:
	RETN 8
}}

void Install_Commands()
{
	static bool isInstalled = false;
	if (isInstalled) return;

	Install_UpdateServer();

	log_msg("[Patch] Commands Support\n");

	DWORD CustomCommandOffset = D2Client::GetAddress(0x32BDD);

	// Run custom commmand
	Memory::SetCursor(CustomCommandOffset);
	Memory::ChangeByte(0x83, 0xE8);
	Memory::ChangeCallA(0xC08508C4, (DWORD)caller_Commands);

	if (active_logFileMemory) log_msg("\n");
	isInstalled = true;
}