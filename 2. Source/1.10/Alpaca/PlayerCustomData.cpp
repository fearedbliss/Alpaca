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

#include "UpdateClient.h"
#include "InfinityStash.h"
#include "InterfaceStash.h"
#include "SavePlayerData.h"
#include "loadPlayerData.h"
#include "Common.h"

bool active_PlayerCustomData = true;

Stash* locateStashFromItem(Unit* ptChar, Unit* currentItem, Stash* currentStash, Stash* stashToSearch)
{
	while (currentStash)
	{
		Unit* retrievedItem;

		if (currentStash == stashToSearch)
		{
			retrievedItem = D2Common::D2InventoryGetFirstItem(PCInventory);
		}
		else
		{
			retrievedItem = currentStash->ptListItem;
		}

		while (retrievedItem)
		{
			if (currentItem == D2Common::D2GetRealItem(retrievedItem))
			{
				return currentStash;
			}
			retrievedItem = D2Common::D2UnitGetNextItem(retrievedItem);
		}

		currentStash = currentStash->nextStash;
	}

	return NULL;
}

Stash* getStashFromItem(Unit* ptChar, Unit* ptItem)
{
	Stash* locatedStash = locateStashFromItem(ptChar, ptItem, PCPY->selfStash, PCPY->currentStash);
	return locatedStash ? locatedStash : NULL;
}

void __stdcall updateClientPlayerOnLoading(Unit* ptChar)
{
	log_msg("--- Start update client on loading ---\n");
	
	selectStash(ptChar, PCPY->selfStash, true);

	log_msg("--- End update client on loading ---\n\n");
}

PlayerData* __fastcall init_PlayerCustomData(DWORD p1, DWORD size, LPCSTR file, DWORD line, DWORD p5)
{
	log_msg("init_PlayerCustomData\n");
	PlayerData* playerData = (PlayerData*)Fog::D2AllocMem(p1,size+sizeof(PYPlayerData),file,line,p5);
	ZeroMemory((BYTE*)playerData + shifting.ptPYPlayerData, sizeof(PYPlayerData));
	return playerData;
}

void freeStash(Stash* ptStash)
{
	if (!ptStash) return;
	freeStash(ptStash->nextStash);
	free(ptStash->nextStash);
	if(ptStash->name)
	{
		free(ptStash->name);
		ptStash->name = NULL;
	}
	ptStash->nextStash = NULL;
}

void __fastcall free_PlayerCustomData(DWORD p1, PlayerData* playerData, LPCSTR file, DWORD line, DWORD p5)
{
	log_msg("free_PlayerCustomData\n");
	PYPlayerData* ptPYPlayerData = (PYPlayerData*)((DWORD)playerData + shifting.ptPYPlayerData);

	freeStash(ptPYPlayerData->selfStash);
	free(ptPYPlayerData->selfStash);
	ptPYPlayerData->selfStash = NULL;

	Fog::D2FreeMem(p1,playerData,file,line,p5);
}

Unit* __stdcall getNextItemToFree(Unit* ptChar, Unit* ptItem)
{
	Unit* item = D2Common::D2UnitGetNextItem(ptItem);
	if (item) return item;

	if (ptChar->nUnitType != UNIT_PLAYER) return NULL;
	if (!PCPlayerData) return NULL;
	if (!PCPY) return NULL;

	Stash* curStash = PCPY->selfStash;
	while ( curStash )
	{
		if (curStash->ptListItem)
		{
			item = curStash->ptListItem;
			curStash->ptListItem = NULL;
			return item;
		}
		curStash = curStash->nextStash;
	}

	return NULL;
}

Unit* __fastcall updateItem_10(GameStruct* ptGame, DWORD type, DWORD itemNum, Unit* ptChar)
{
	//const int INVENTORY = 0;
	const int STASH = 4;
	//const int BELT_OR_PICKED_UP = 255;

	Unit* ptItem = D2Game::D2GameGetObject(ptGame, type, itemNum);
	if (D2Common::D2ItemGetPage(ptItem) == STASH)
	{
		Stash* ptStash = getStashFromItem(ptChar, ptItem);
		if (!ptStash) return NULL;
		selectStash(ptChar, ptStash, true);
	}
	return ptItem;
}

FCT_ASM(caller_updateItem_10)
	PUSH EBP
	PUSH DWORD PTR SS : [ESP + 0x8]
	CALL updateItem_10
	RETN 4
}}

FCT_ASM(callerServer_getNextItemToFree)
	PUSH DWORD PTR SS : [ESP + 4]
	PUSH DWORD PTR SS : [ESP + 0x28]
	CALL getNextItemToFree
	RETN 4
}}

FCT_ASM(callerClient_getNextItemToFree)
	PUSH DWORD PTR SS : [ESP + 4]
	PUSH DWORD PTR SS : [ESP + 0x24]
	CALL getNextItemToFree
	RETN 4
}}

FCT_ASM(caller_updateClientPlayerOnLoading)
	PUSH DWORD PTR SS : [ESP + 0x14]
	CALL updateClientPlayerOnLoading
	POP ECX
	POP EDI
	POP ESI
	POP EBP
	XOR EAX, EAX
	JMP ECX
}}

void Install_PlayerCustomData()
{
	static bool isInstalled = false;
	if (isInstalled || !active_PlayerCustomData) return;

	Install_SavePlayerData();
	Install_LoadPlayerData();
	Install_UpdateClient();

	log_msg("[Patch] Player Custom Data\n");

	DWORD InitializeCustomDataOffset = (DWORD)D2Common::D2InitPlayerData + 0x62;
	DWORD UpdateItemOffset1 = D2Game::GetAddress(0x1100D);
	DWORD UpdateItemOffset2 = D2Game::GetAddress(0x11058);
	DWORD UpdateClientOnLoadingOffset = D2Game::GetAddress(0x25D4);
	DWORD FreeCustomDataOffset = D2Common::GetAddress(0x80483);
	DWORD FreeItemInStashServerSideOffset = D2Game::GetAddress(0x8D5A4);
	DWORD FreeItemInStashClientSideOffset = D2Client::GetAddress(0x89B32);
	DWORD TestIfAlreadyRemovedFromInventoryOffset = D2Common::GetAddress(0x4E689);

	// Initialize custom data.
	Memory::SetCursor(InitializeCustomDataOffset);
	Memory::ChangeCallB((DWORD)Fog::D2AllocMem, (DWORD)init_PlayerCustomData);

	// Update Item
	Memory::SetCursor(UpdateItemOffset1);
	Memory::ChangeCallC((DWORD)D2Game::D2GameGetObject, (DWORD)caller_updateItem_10);

	Memory::SetCursor(UpdateItemOffset2);
	Memory::ChangeCallC((DWORD)D2Game::D2GameGetObject, (DWORD)caller_updateItem_10);

	// Update client on loading
	Memory::SetCursor(UpdateClientOnLoadingOffset);
	Memory::ChangeByte(0x5F, 0xE8);
	Memory::ChangeCallA(0xC0335D5E, (DWORD)caller_updateClientPlayerOnLoading);

	// Free custom data.
	Memory::SetCursor(FreeCustomDataOffset);
	Memory::ChangeCallB((DWORD)Fog::D2FreeMem, (DWORD)free_PlayerCustomData);

	// Free item in Stash (Server-side)
	Memory::SetCursor(FreeItemInStashServerSideOffset);
	Memory::ChangeCallB((DWORD)D2Common::D2UnitGetNextItem, (DWORD)callerServer_getNextItemToFree);

	// Free item in Stash (Client-side)
	Memory::SetCursor(FreeItemInStashClientSideOffset);
	Memory::ChangeCallB((DWORD)D2Common::D2UnitGetNextItem, (DWORD)callerClient_getNextItemToFree);

	// Test if it's already removed from inventory
	Memory::SetCursor(TestIfAlreadyRemovedFromInventoryOffset);
	Memory::ChangeByte(0x0D, 0x07);

	if (active_logFileMemory) log_msg("\n");
	isInstalled = true;
}