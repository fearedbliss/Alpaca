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

#include "InfinityStash.h"
#include "UpdateClient.h"
#include "InterfaceStash.h"
#include "Common.h"

#define STASH_TAG 0x5453			//"ST"
#define JM_TAG 0x4D4A 				//"JM"

const DWORD maxSelfPages = 999;
const DWORD nbPagesPerIndex = 5;
const DWORD nbPagesJump = 10;

typedef int (*TchangeToSelectedStash)(Unit* ptChar, Stash* newStash, DWORD bOnlyItems, DWORD bIsClient);
TchangeToSelectedStash changeToSelectedStash;

Unit* firstClassicStashItem(Unit* ptChar)
{
	Unit* ptItem = D2Common::D2InventoryGetFirstItem(PCInventory);
	while (ptItem)
	{
		if (D2Common::D2ItemGetPage(D2Common::D2GetRealItem(ptItem)) == 4)
			return ptItem;
		ptItem = D2Common::D2UnitGetNextItem(ptItem);
	}
	return NULL;
}

DWORD endStashList(Unit* ptChar, Stash* ptStash)
{
	Stash* stash = ptStash;
	
	while (stash)
	{
		if (stash->ptListItem || ((stash == PCPY->currentStash) && firstClassicStashItem(ptChar))) return 0;
		if (stash->isIndex || (stash->name != NULL && stash->name[0])) return 0;
		stash = stash->nextStash;
	}
	return 1;
}

Stash* newStash(DWORD id)
{
	d2_assert( id == 0xFFFFFFFF , "Too much stash", __FILE__, __LINE__);
	
	Stash* stash = (Stash*)malloc(sizeof(Stash));
	d2_assert(!stash , "Error on stash allocation.", __FILE__, __LINE__);
	ZeroMemory(stash, sizeof(Stash));
	stash->id = id;
	
	return stash;
}

Stash* getStash(Unit* ptChar, DWORD id)
{
	Stash* ptStash = PCPY->selfStash;

	while (ptStash)
	{
		if (ptStash->id == id) return ptStash;
		ptStash = ptStash->nextStash;
	}

	return NULL;
}

Stash* addStash(Unit* ptChar)
{
	Stash* previous;
	Stash* stash;

	previous = getStash(ptChar, PCPY->nbSelfPages - 1);
	stash = newStash(PCPY->nbSelfPages++);
	
	stash->previousStash = previous;
	if (previous)
	{
		previous->nextStash = stash;
	}
	else
	{
		PCPY->selfStash = stash;
	}
	
	log_msg("AddStash: stash->id=%d\tstash->previous=%08X\tnbSelf=%d\n", stash->id, stash->previousStash, PCPY->nbSelfPages);
	
	return stash;
}

// Jumps to the page that is 'nbPagesJump' backwards from the current stash.
void selectPreviousStashJump(Unit* ptChar)
{
	Stash* selStash = PCPY->currentStash;
	if (selStash)
	{
		int targetPage = PCPY->currentStash->id - nbPagesJump;
		if (targetPage < 0)
		{
			targetPage = 0;
		}
		jumpToPage(ptChar, targetPage);
	}
}

// Jumps to the page that is 'nbPagesJump' forward from the current stash.
void selectNextStashJump(Unit* ptChar)
{
	Stash* selStash = PCPY->currentStash;
	if (selStash)
	{
		int targetPage = PCPY->currentStash->id + nbPagesJump;

		// Adjust target page if starting at Page 1 (So that it goes beautifully by 5s)
		if (PCPY->currentStash->id == 0)
		{
			targetPage--;
		}

		if (targetPage > maxSelfPages)
		{
			targetPage = maxSelfPages;
		}
		jumpToPage(ptChar, targetPage);
	}
}

int changeToSelectedStash_110(Unit* ptChar, Stash* newStash, DWORD bOnlyItems, DWORD bIsClient)
{
	if (!newStash) return 0;

	log_msg("changeToSelectedStash ID:%d\tonlyItems:%d\tclient:%d\n", newStash->id, bOnlyItems, bIsClient);

	Stash* currentStash = PCPY->currentStash;
	if (currentStash == newStash) return 0;

	d2_assert(currentStash && currentStash->ptListItem, "ERROR : currentStash isn't empty (ptListItem != NULL)", __FILE__, __LINE__);

	// Remove items from current page
	Unit* ptNextItem;
	Unit* ptItem = D2Common::D2InventoryGetFirstItem(PCInventory);
	while (ptItem)
	{
		ptNextItem = D2Common::D2UnitGetNextItem(ptItem);
		if (D2Common::D2ItemGetPage(ptItem) == 4)
		{
			BYTE tmp = ptItem->ptItemData->ItemData2;
			ptItem = D2Common::D2InvRemoveItem(PCInventory, ptItem);
			ptItem->ptItemData->ItemData2 = tmp;
			if (currentStash)
			{
				ptItem->ptItemData->ptNextItem = currentStash->ptListItem;
				currentStash->ptListItem = ptItem;
			}
		}
		ptItem = ptNextItem;
	}

	// add items of new stash
	ptItem = newStash->ptListItem;
	while (ptItem)
	{
		D2Common::D2InvAddItem(PCInventory, ptItem, ptItem->path->x, ptItem->path->y, 0xC, bIsClient, 4);
		ptItem = D2Common::D2UnitGetNextItem(ptItem);
	}
	if (bOnlyItems)
		newStash->ptListItem = PCPY->currentStash->ptListItem;
	else
		PCPY->currentStash = newStash;
	PCPY->currentStash->ptListItem = NULL;

	return 1;
}

DWORD loadStash(Unit* ptChar, Stash* ptStash, BYTE data[], DWORD startSize, DWORD maxSize, DWORD* retSize)
{
	DWORD curSize = startSize;
	DWORD nbBytesRead;

	log_msg("loadStash\n");

	*retSize = curSize;
	if( *(WORD *)&data[curSize] != STASH_TAG )//"ST"
	{
		log_msg("loadStash -> Bad tag of stash of character %s : %04X\n",PCPlayerData->name,*(WORD *)&data[curSize]);
		return 0x7; //Unable to enter game. Bad inventory data
	}
	curSize += 2;

	// Read flags
	int len = strlen((char*)&data[curSize]);
	if (*(WORD*)&data[curSize + len + 1] != JM_TAG)
	{
		ptStash->flags = *(DWORD *)&data[curSize];
		curSize += 4;
	}

	// Read Name
	if (strlen((char *)&data[curSize]))
		ptStash->name = (char*)malloc(strlen((char *)&data[curSize]));
	if (ptStash->name)
		strcpy(ptStash->name, (char *)&data[curSize]);
	curSize += strlen((char *)&data[curSize]) + 1;

	// Read inventory
	DWORD ret = D2Game::D2LoadInventory(PCGame, ptChar, (saveBitField*)&data[curSize], 0x60, maxSize-curSize, 0, &nbBytesRead);
	if (ret) log_msg("loadStash -> D2Game::D2LoadInventory failed\n");
	log_msg("Stash loaded (%d : %s)\n", ptStash->id ,ptStash->name);

	*retSize=curSize + nbBytesRead;
	return ret;
}

DWORD loadStashList(Unit* ptChar, BYTE* data, DWORD maxSize, DWORD* curSize)
{
	DWORD curStash = 0;
	Stash* newStash;

	DWORD nbStash = *(DWORD*)&data[*curSize];
	*curSize += 4;

	while (curStash < nbStash)
	{
		newStash = addStash(ptChar);
		changeToSelectedStash(ptChar, newStash, 0, false);
		DWORD ret = loadStash(ptChar, newStash, data, *curSize, 10000000, curSize);
		if (ret) return ret;
		curStash++;
	}

	if (!PCPY->selfStash)
	{
		newStash = addStash(ptChar);
		PCPY->currentStash = newStash;
	}

	return 0;
}

#define DATA (*data + *curSize)
#define ADDDATA(T)		(T*)DATA;		*curSize += sizeof(T)
#define SETDATA(T,V)	*(T*)DATA = V;	*curSize += sizeof(T)

void saveStash(Unit* ptChar, Stash* ptStash, BYTE** data, DWORD* maxSize, DWORD* curSize)
{
	//write "ST"
	SETDATA(WORD, STASH_TAG);

	//Write flags.
	SETDATA(DWORD, ptStash->flags);

	//save name
	if (ptStash->name)
	{
		int size = strlen(ptStash->name);
		if (size > 15) size = 15;
		strncpy((char*)DATA, ptStash->name, size);
		*curSize += size;
	}
	SETDATA(char, NULL);

	//Write "JM" of inventory
	SETDATA(WORD, JM_TAG);

	//Get position of counter of items in inventory
	WORD* ptNBItem = ADDDATA(WORD);
	*ptNBItem = 0;

	//Get first item
	Unit* ptItem;
	if ((ptStash->id == PCPY->currentStash->id))
		ptItem = D2Common::D2InventoryGetFirstItem(PCInventory);
	else
		ptItem = ptStash->ptListItem;

	//Write all items
	while (ptItem)
	{
		if (D2Common::D2ItemGetPage(D2Common::D2GetRealItem(ptItem)) == 4)
		{
			int nbBytes = D2Common::D2SaveItem(D2Common::D2GetRealItem(ptItem), (saveBitField*)DATA, *maxSize - *curSize, 1, 1, 0);
			d2_assert(!nbBytes, "!\"Character has too many items\"", __FILE__, __LINE__ );
			*curSize += nbBytes;
			(*ptNBItem)++;
		}
		ptItem = D2Common::D2UnitGetNextItem(ptItem);
	}
}


void saveStashList(Unit* ptChar, Stash* ptStash, BYTE** data, DWORD* maxSize, DWORD* curSize)
{
	DWORD curSizeNbStash = *curSize;
	*curSize += sizeof(DWORD);

	DWORD nbStash=0;

	while(ptStash)
	{
		if (*curSize + 0x2000 > *maxSize)
		{
			BYTE* oldData = *data;
			*maxSize *= 2;
			*data = (BYTE *)Fog::D2AllocMem(PCGame->memoryPool, *maxSize,__FILE__,__LINE__,0);
			d2_assert(!*data, "Error : Memory allocation", __FILE__, __LINE__);
			CopyMemory(*data, oldData, *curSize);
			Fog::D2FreeMem(PCGame->memoryPool, oldData,__FILE__,__LINE__,0);
		}
		saveStash(ptChar, ptStash, data, maxSize, curSize);
		nbStash++;
		ptStash = endStashList(ptChar, ptStash->nextStash) ? NULL : ptStash->nextStash;
	}
	*(DWORD*)(*data + curSizeNbStash) = nbStash;
}

/////// client
void updateSelectedStashClient(Unit* ptChar)
{
	Stash* newStash = PCPY->currentStash;
	if (!newStash)
		return;
	updateClient(ptChar, UC_SELECT_STASH, newStash->id, newStash->flags, PCPY->flags);
	updateClient(ptChar, UC_PAGE_NAME, newStash->name);
}

void setSelectedStashClient(DWORD stashId, DWORD stashFlags, DWORD flags, bool bOnlyItems)
{
	log_msg("setSelectedStashClient ID:%d, stashFlags:%d, flags:%08X\n", stashId, stashFlags, flags);
	Unit* ptChar = D2Client::D2GetClientPlayer();
	Stash* newStash = getStash(ptChar, stashId);
	if (!newStash) do
		newStash = addStash(ptChar);
	while (newStash->id < stashId);
	newStash->flags = stashFlags;
	changeToSelectedStash(ptChar, newStash, bOnlyItems, 1);
	PCPY->flags = flags;
}

void selectStash(Unit* ptChar, Stash* newStash, bool isRunningDuringInit)
{
	if (!newStash) return;
	changeToSelectedStash(ptChar, newStash, 0, 0);
	updateSelectedStashClient(ptChar);
	rememberLastSelectedStash(ptChar, newStash, isRunningDuringInit);
}

// Jumps to the target page
void jumpToPage(Unit* ptChar, DWORD targetPageIndex)
{
	Stash* targetStash = getStash(ptChar, targetPageIndex);

	if (PCPY->currentStash == targetStash) return;

	if (targetStash == NULL)
	{
		targetStash = createStashesUpToPageIndex(ptChar, PCPY->selfStash, targetPageIndex);
	}

	selectStash(ptChar, targetStash);
}


// Creates all the pages up to a page index
Stash* createStashesUpToPageIndex(Unit* ptChar, Stash* currentStash, DWORD targetPageIndex)
{
	for (DWORD currentPageIndex = 0; currentPageIndex < targetPageIndex; currentPageIndex++)
	{
		if (currentStash->nextStash == NULL)
		{
			addStash(ptChar);
		}
		currentStash = currentStash->nextStash;
	}

	return currentStash;
}

// Swaps the stash metadata
void swapStashMetadata(Stash* currentStash, Stash* targetStash)
{
	// Name
	char* tempName = currentStash->name;
	currentStash->name = targetStash->name;
	targetStash->name = tempName;

	// We are only flipping the individual properties vs setting the "flags".
	// Previously we didn't use "flags" because of personal/shared problems.
	// But it's more specific to flip the individual properties anyways.

	// Index
	DWORD tempIndex = currentStash->isIndex;

	currentStash->isIndex = targetStash->isIndex;

	targetStash->isIndex = tempIndex;
}

void swapStash(Unit* ptChar, Stash* curStash, Stash* swpStash)
{
	if (!ptChar || !curStash || !swpStash || curStash == swpStash)
		return;

	swapStashMetadata(curStash, swpStash);
	changeToSelectedStash(ptChar, swpStash, 1, 0);
	updateClient(ptChar, UC_SELECT_STASH, swpStash->id, swpStash->flags | 8, PCPY->flags);
	updateSelectedStashClient(ptChar);
}

void swapStash(Unit* ptChar, DWORD targetPageIndex)
{
	log_msg("swap stash page = %u\n", targetPageIndex);

	// Get the current stash
	Stash* curStash = PCPY->currentStash;

	// Get the stash we want to swap to. The reference starts at id 0 (aka Page 1).
	Stash* swpStash = PCPY->selfStash;

	// Loop through each page until we get up to the page we want to toggle our items into
	// in the opposite stash type, each stash in between needs to be allocated so that our
	// pointer to the next stash can point to the correct location.
	swpStash = createStashesUpToPageIndex(ptChar, swpStash, targetPageIndex);

	// Now that we've arrived at stash page we want to switch our
	// items to in the opposite stash type, go ahead and do the swap.
	swapStash(ptChar, curStash, swpStash);
}

void renameCurrentStash(Unit* ptChar, char* name)
{
	log_msg("renameCurrentStash : %08X, %s\n", ptChar, name);
	Stash* stash = PCPY->currentStash;
	int len = 0;
	if (name != NULL)
		len = strlen(name);
	log_msg("renameCurrentStash : %d\n", len);
	if (stash->name)
		Fog::D2FogMemDeAlloc(stash->name, __FILE__, __LINE__, 0);
	log_msg("renameCurrentStash 3\n");
	if (len > 0)
	{
		stash->name = (char *)malloc(len);
		strcpy(stash->name, name);
	}
	else
		stash->name = NULL;
	log_msg("renameCurrentStash 4\n");
}

void setCurrentStashIndex(Unit* ptChar, int index)
{
	if (!PCPY->currentStash)
		return;
	PCPY->currentStash->isIndex = index >= 1;
	updateSelectedStashClient(ptChar);
}

void selectPreviousStash(Unit* ptChar)
{
	Stash* selStash = PCPY->currentStash->previousStash;
	if (selStash && (selStash != PCPY->currentStash))
	{
		selectStash(ptChar, selStash);
	}
}

void selectNextStash(Unit* ptChar)
{
	Stash* selStash = PCPY->currentStash;
	if (selStash->id >= maxSelfPages) return;

	selStash = selStash->nextStash ? selStash->nextStash : addStash(ptChar);

	if (selStash && (selStash != PCPY->currentStash))
	{
		selectStash(ptChar, selStash);
	}
}

// Remembers the last selected stash for the particular stash type
// for the duration of the game session.
void rememberLastSelectedStash(Unit* ptChar, Stash* selectedStash, bool isRunningDuringInit)
{
	// It seems the code at the moment while scan all the stashes until it reaches the last stash
	// during game initialization. For this case, we don't want to update it since then the user
	// would see the item in the last page when they open their stash. We want to start on the
	// first pages of both (personal/shared), and only remember the pages within the user's
	// game session.
	if (isRunningDuringInit) return;

	PCPY->lastSelectedSelfStash = selectedStash;
}

void selectPreviousIndexStash(Unit* ptChar)
{
	selectPreviousStash(ptChar);
	Stash* selStash = PCPY->currentStash;
	while (selStash && !selStash->isIndex)
		selStash = selStash->previousStash;

	if (selStash == NULL)
	{
		selStash = PCPY->currentStash;
		while (selStash->previousStash && ((selStash->id + 1) % nbPagesPerIndex != 0))
			selStash = selStash->previousStash;
	}

	if (selStash && (selStash != PCPY->currentStash))
		selectStash(ptChar, selStash);
}

void selectNextIndexStash(Unit* ptChar)
{
	selectNextStash(ptChar);
	Stash* selStash = PCPY->currentStash;
	while (selStash && !selStash->isIndex)
		selStash = selStash->nextStash;

	if (selStash == NULL)
	{
		selStash = PCPY->currentStash;
		while ((selStash->id + 1) % nbPagesPerIndex != 0)
		{
			if (selStash->id >= maxSelfPages) break;
			selStash = selStash->nextStash ? selStash->nextStash : addStash(ptChar);;
		}
	}
	if (selStash && (selStash != PCPY->currentStash))
		selectStash(ptChar, selStash);
}

//////////////////////////////////////////////////////////////////////
Stash* curStash2;
DWORD currentSawStash2;

Unit* __stdcall getNextItem(Unit* ptChar, Unit* ptItem)
{
	Unit* item = D2Common::D2UnitGetNextItem(ptItem);
	if (item) return item;

	if (!curStash2)
	{
		switch (currentSawStash2)
		{
		case 0: curStash2 = PCPY->selfStash;
				currentSawStash2 = 1;
				break;
		default: return NULL;
		}
	} else {
		curStash2 = curStash2->nextStash;
	}
	if (curStash2)
	{
		item = curStash2->ptListItem;
		if (item) return item;
	}
	return getNextItem(ptChar,item);
}

Unit* __stdcall initGetNextItem(Unit* ptChar, Unit* ptItem)
{
	if (ptChar->nUnitType != UNIT_PLAYER) return NULL;
	if (!PCPY) return NULL;

	curStash2 = NULL;
	currentSawStash2 = 0;
	if (ptItem) return ptItem;
	return getNextItem(ptChar,ptItem);
}

DWORD __stdcall carry1Limit(Unit* ptChar, Unit* ptItemParam, DWORD itemNum, BYTE page)
{
	if (!ptChar) return 0;
	Unit* ptItem = ptItemParam ? ptItemParam : D2Game::D2GameGetObject(PCGame, UNIT_ITEM, itemNum);
	if ((page != 4) && (D2Common::D2GetItemQuality(ptItem) == 7) && ptChar)
	{
		auto dataTables = D2Common::GetDataTables();
		int uniqueID = D2Common::D2GetUniqueID(ptItem);
		if ((uniqueID >= 0) && (uniqueID < (int)dataTables->nbUniqueItems))
		{
			UniqueItemsBIN*	uniqueItems = dataTables->uniqueItems + uniqueID;
			if (uniqueItems && (uniqueItems->carry1 == 1))
			{
				ItemsBIN* ptItemsBin = D2Common::D2GetItemsBIN(ptItem->nTxtFileNo);
				Unit* ptFirstItem = D2Common::D2InventoryGetFirstItem(PCInventory);
				if (ptItemsBin && ptFirstItem)
					return D2Game::D2VerifIfNotCarry1(ptItem, ptItemsBin, ptFirstItem);
			}
		}
	}
	return 0;
}

bool isStashPageValid(int pageIndex)
{
	return (pageIndex < 0 || pageIndex > maxSelfPages) ? false : true;
}

FCT_ASM(caller_initGetNextItem)
	PUSH DWORD PTR SS : [ESP + 0x20]
	PUSH DWORD PTR SS : [ESP + 0xC]
	CALL initGetNextItem
	MOV EDI, EAX
	TEST EDI, EDI
	RETN
}}

FCT_ASM(caller_getNextItem)
	PUSH DWORD PTR SS : [ESP + 4]
	PUSH DWORD PTR SS : [ESP + 0x10]
	CALL getNextItem
	RETN 4
}}

FCT_ASM(caller_carry1Limit)
	PUSH DWORD PTR SS : [ESP + 0x08]//page
	PUSH 0//EBP
	PUSH DWORD PTR SS : [ESP + 0x0C]
	PUSH DWORD PTR SS : [ESP + 0x28]//ptChar
	CALL carry1Limit
	TEST EAX, EAX
	JNZ	end_carry1Limit
	JMP D2Common::D2ItemSetPage
end_carry1Limit:
	ADD ESP, 0xC
	POP EDI
	POP ESI
	POP EBP
	XOR EAX, EAX
	POP EBX
	ADD ESP, 0x24
	RETN 8
}}

FCT_ASM(caller_carry1LimitWhenDrop)
	PUSH EAX
	PUSH 0
	PUSH 0
	PUSH ESI//ptItem
	PUSH EDI//ptChar
	CALL carry1Limit
	TEST EAX, EAX
	POP EAX
	JNZ END_carry1LimitWhenDrop
	MOV EDX, 0x806
	RETN
END_carry1LimitWhenDrop:
	ADD DWORD PTR SS : [ESP] , 0x1F
	RETN
}}

FCT_ASM(caller_carry1LimitSwap)
	PUSH EAX
	PUSH DWORD PTR SS : [ESP + 0x20]
	PUSH 0
	PUSH EBP//ptChar
	CALL carry1Limit
	TEST EAX, EAX
	JNZ	end_carry1Limit
	JMP D2Common::D2ItemGetPage
end_carry1Limit:
	ADD ESP, 8
	POP EDI
	POP ESI
	POP EBP
	XOR EAX, EAX
	POP EBX
	ADD ESP, 0x4C
	RETN 8
}}

FCT_ASM(caller_carry1OutOfStash)
	PUSH ESI
	CALL D2Common::D2ItemGetPage
	CMP AL, 4
	JNZ continue_carry1OutOfStash
	ADD DWORD PTR SS : [ESP] , 0x1AF
	RETN
continue_carry1OutOfStash:
	MOV EAX, DWORD PTR SS : [ESP + 0x14]
	TEST EAX, EAX
	RETN
}}

void Install_MultiPageStash()
{
	static bool isInstalled = false;
	if (isInstalled) return;

	Install_PlayerCustomData();
	Install_InterfaceStash();

	log_msg("[Patch] Multi Page Stash\n");

	changeToSelectedStash = changeToSelectedStash_110;

	DWORD PutCarryInventoryOffset = D2Game::GetAddress(0x55050);
	DWORD PutCarryInventorySwapOffset = D2Game::GetAddress(0x558D9);
	DWORD DropCarryInventoryDropCubeOffset = D2Game::GetAddress(0x14341);
	DWORD VerifyStashPagePickupOffset = D2Game::GetAddress(0x1299E);

	// Cannot put 2 items carry1 in inventory
	Memory::SetCursor(PutCarryInventoryOffset);
	Memory::ChangeCallB((DWORD)D2Common::D2ItemSetPage, (DWORD)caller_carry1Limit);

	// Cannot put 2 items carry1 in inventory by swapping
	Memory::SetCursor(PutCarryInventorySwapOffset);
	Memory::ChangeCallB((DWORD)D2Common::D2ItemGetPage, (DWORD)caller_carry1LimitSwap);

	// Cannot put 2 items carry1 in inventory when drop cube
	Memory::SetCursor(DropCarryInventoryDropCubeOffset);
	Memory::ChangeByte(0xBA, 0xE8);
	Memory::ChangeCallA(0x806, (DWORD)caller_carry1LimitWhenDrop);

	// Verif only carry1 out of stash page when pick up an item
	Memory::SetCursor(VerifyStashPagePickupOffset);
	Memory::ChangeByte(0x8B, 0xE8);
	Memory::ChangeCallA(0x85102444, (DWORD)caller_carry1OutOfStash);
	Memory::ChangeByte(0xC0, 0x90);

	if (active_logFileMemory) log_msg("\n");
	isInstalled = true;
}
