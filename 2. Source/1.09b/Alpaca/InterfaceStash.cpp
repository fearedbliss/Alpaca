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

#include "InterfaceStash.h"
#include "UpdateServer.h"
#include "InfinityStash.h"
#include "AlpacaFiles.h"	
#include "Common.h"
#include <stdio.h>
#include "Memory.h"

static struct
{
	union {
		DWORD all;
		struct {
			DWORD previous:1;
			DWORD next:1;
		};
	};
} isDownBtn;

// Display Set Item Names In Green
// This is so that if you have a set item in any page of your stash
// the game will be able to see your set items (and show you that you
// have the set item), anywhere in any of of your stashes.
bool displaySharedSetItemNameInGreen = true;

const int defaultButtonPositionValue = -1;
const int posXPreviousBtn = defaultButtonPositionValue;
const int posYPreviousBtn = defaultButtonPositionValue;
const int posXNextBtn = defaultButtonPositionValue;
const int posYNextBtn = defaultButtonPositionValue;

// Localization Strings (English Only)
WCHAR* STR_STASH_PREVIOUS_PAGE = L"Previous (Shift: Index Or By %d, Ctrl: Page By %d)";
WCHAR* STR_STASH_NEXT_PAGE = L"Next (Shift: Index Or By %d, Ctrl: Page By %d)";
WCHAR* STR_PERSONAL_PAGE_NUMBER = L"Page - %u";
WCHAR* STR_NO_SELECTED_PAGE = L"No page selected";

DWORD	getXPreviousBtn()		{return RX(posXPreviousBtn<0? D2gfx::D2GetResolution()?0x80:0xAF : posXPreviousBtn);}
#define	getLPreviousBtn()		32
DWORD	getYPreviousBtn()		{return RY(posYPreviousBtn<0 ? 0x40 : posYPreviousBtn);}
#define	getHPreviousBtn()		32

DWORD	getXNextBtn()			{return RX(posXNextBtn<0 ? D2gfx::D2GetResolution()?0xA0:0xCF :posXNextBtn);}
#define	getLNextBtn()			32
DWORD	getYNextBtn()			{return RY(posYNextBtn<0 ? 0x40 : posYNextBtn);}
#define	getHNextBtn()			32

#define isOnButtonNextStash(x,y) isOnRect(x, y, getXNextBtn(), getYNextBtn(), getLNextBtn(), getHNextBtn())
#define isOnButtonPreviousStash(x,y) isOnRect(x, y, getXPreviousBtn(), getYPreviousBtn(), getLPreviousBtn(), getHPreviousBtn())

void* __stdcall printBtns()
{
	Unit* ptChar = D2Client::D2GetClientPlayer();

	sDrawImageInfo data;
	ZeroMemory(&data, sizeof(data));
	setImage(&data, stashBtnsImages);

	setFrame(&data, 0 + isDownBtn.previous);
	D2gfx::D2PrintImage(&data, getXPreviousBtn(), getYPreviousBtn(), -1, 5, 0);

	setFrame(&data, 2 + isDownBtn.next);
	D2gfx::D2PrintImage(&data, getXNextBtn(), getYNextBtn(), -1, 5, 0);

	WCHAR text[100];
	DWORD mx = D2Client::D2GetMouseX();
	DWORD my = D2Client::D2GetMouseY();

	D2Win::D2SetFont(1);

	if (isOnButtonPreviousStash(mx, my))
	{
		_snwprintf(text, sizeof(text), STR_STASH_PREVIOUS_PAGE, nbPagesPerIndex, nbPagesJump);
		D2Win::D2PrintPopup(text, getXPreviousBtn() + getLPreviousBtn() / 2, getYPreviousBtn() - getHPreviousBtn(), WHITE, 1);
	}
	else if (isOnButtonNextStash(mx, my))
	{
		_snwprintf(text, sizeof(text), STR_STASH_NEXT_PAGE, nbPagesPerIndex, nbPagesJump);
		D2Win::D2PrintPopup(text, getXNextBtn() + getLNextBtn() / 2, getYNextBtn() - getHNextBtn(), WHITE, 1);
	}

	return D2Client::D2LoadBuySelBtn();
}

DWORD __stdcall manageBtnDown(sWinMessage* msg)
{
	if (isOnButtonPreviousStash(msg->x, msg->y))
	{
		isDownBtn.previous = 1;
	}
	else if (isOnButtonNextStash(msg->x, msg->y))
	{
		isDownBtn.next = 1;
	}
	else
	{
		return 0;
	}

	D2Client::D2PlaySound(4, 0, 0, 0, 0);
	
	freeMessage(msg);
	return 1;
}

DWORD __stdcall manageBtnUp(sWinMessage* msg)
{
	Unit* ptChar = D2Client::D2GetClientPlayer();

	if (isOnButtonPreviousStash(msg->x,msg->y))
	{
		log_msg("push up left button previous\n");
		if (isDownBtn.previous)
		{
			if (GetKeyState(VK_SHIFT) < 0)
			{
				updateServer(US_SELECT_PREVIOUS_INDEX);
			}
			else if (GetKeyState(VK_CONTROL) < 0)
			{
				updateServer(US_SELECT_PREVIOUS_JUMP);
			}
			else
			{
				updateServer(US_SELECT_PREVIOUS);
			}
		}
	}
	else if (isOnButtonNextStash(msg->x,msg->y))
	{
		log_msg("push up left button next\n");
		if (isDownBtn.next)
		{
			if (GetKeyState(VK_SHIFT) < 0)
			{
				updateServer(US_SELECT_NEXT_INDEX);
			}
			else if (GetKeyState(VK_CONTROL) < 0)
			{
				updateServer(US_SELECT_NEXT_JUMP);
			}
			else
			{
				updateServer(US_SELECT_NEXT);
			}
		}
	}
	else
	{
		return 0;
	}

	return 1;
}

void __fastcall printPageNumber(LPWSTR maxGoldText, DWORD x, DWORD y, DWORD color, DWORD bfalse)
{
	Unit* ptChar = D2Client::D2GetClientPlayer();
	WCHAR popupText[100];

	if (PCPY->currentStash)
	{
		bool isIndex = PCPY->currentStash->isIndex;
		DWORD currentId = PCPY->currentStash->id;

		// Display the appropriate stash information (page)
		if (PCPY->currentStash->name && PCPY->currentStash->name[0])
		{
			const int BUFFER_MAX = 50;

			// Convert the custom page number to a string and concat
			// it at the end of the custom name. This code is ugly,
			// there should be a more efficient/safer C way of doing this.
			char pageAsString[BUFFER_MAX];
			char customNameWithPage[100];

			// Convert the integer to a string
			sprintf(pageAsString, "%d", currentId + 1);

			// Concat page number to the end of the custom stash name
			strcpy(customNameWithPage, PCPY->currentStash->name);
			strcat(customNameWithPage, " - ");
			strcat(customNameWithPage, pageAsString);

			// Display the custom page name with the page number
			mbstowcs(popupText, customNameWithPage, BUFFER_MAX);
		}
		else
		{
			_snwprintf(popupText, sizeof(popupText), STR_PERSONAL_PAGE_NUMBER, currentId + 1);
		}

		int stashNameColor = isIndex ? GREEN : WHITE;
		D2Win::D2PrintString(popupText, x, y, stashNameColor, bfalse);
	}
	else
	{
		_snwprintf(popupText, sizeof(popupText), STR_NO_SELECTED_PAGE);
		D2Win::D2PrintString(popupText, x, y, WHITE, bfalse);
	}

	// printGoldMaxPopup
	DWORD mx = D2Client::D2GetMouseX();
	DWORD my = D2Client::D2GetMouseY();
	if ((RX(0x5E) < mx) && (mx < RX(0xF8)) && (RY(0x1C8) < my) && (my < RY(0x1B6)) )
	{
		_snwprintf(popupText, sizeof(popupText), L"%s", maxGoldText);
		
		DWORD x = D2Win::D2GetPixelLen(maxGoldText);
		DWORD x2 = D2Win::D2GetPixelLen(popupText) - x;
		D2Win::D2PrintPopup(popupText, RX(0xA8 - max(x, x2) / 2), RY(0x1CA), WHITE, 0);
	}
}

static Stash* curStash = NULL;
static DWORD currentSawStash = 0;

Unit* __stdcall getNextItemForSet(Unit* ptItem)
{
	Unit* item = ptItem ? D2Common::D2UnitGetNextItem(ptItem) : NULL;
	if (item) return item;

	if (!curStash)
	{
		Unit* ptChar = D2Client::D2GetClientPlayer();
		switch (currentSawStash)
		{
		case 0:
			curStash = PCPY->selfStash;
			currentSawStash = displaySharedSetItemNameInGreen ? 1 : 2;
			break;
		default:
			return NULL;
		}
	}
	else
	{
		curStash = curStash->nextStash;
	}
	if (curStash)
	{
		item = curStash->ptListItem;
		if (item) return item;
	}
	return getNextItemForSet(item);
}

Unit* __stdcall initGetNextItemForSet(Inventory* ptInventory)
{
	Unit* ptChar = D2Client::D2GetClientPlayer();
	if (ptChar->nUnitType != UNIT_PLAYER) return NULL;
	if (!PCPY) return NULL;

	curStash = NULL;
	currentSawStash = 0;
	Unit* item = D2Common::D2InventoryGetFirstItem(ptInventory);
	if (item) return item;
	return getNextItemForSet(item);
}

FCT_ASM(caller_manageBtnDown)
	PUSH EDI
	CALL manageBtnDown
	TEST EAX, EAX
	JE IS_NOT_ON_BUTTON
	POP EDX
	MOV EDX, DWORD PTR DS : [EDX + 0xF3]
	MOV DWORD PTR DS : [EDX] , 1
	POP EDI
	POP ESI
	POP EBP
	POP EBX
	RETN 4
IS_NOT_ON_BUTTON:
		JMP D2Client::IsExpansion
}}

FCT_ASM(caller_manageBtnUp)
	PUSH EBP
	CALL manageBtnUp
	MOV isDownBtn.all, 0
	TEST EAX, EAX
	JE IS_NOT_ON_BUTTON
	POP EDX
	MOV EDX, DWORD PTR DS : [EDX + 0xEA]
	MOV DWORD PTR DS : [EDX] , 0
	SUB EDX, 8
	MOV DWORD PTR DS : [EDX] , 0
	ADD EDX, 4
	MOV DWORD PTR DS : [EDX] , 0
	ADD EDX, 0x68
	MOV DWORD PTR DS : [EDX] , 0
	POP EDI
	POP ESI
	POP EBP
	POP EBX
	RETN 4
IS_NOT_ON_BUTTON:
		JMP D2Client::IsExpansion
}}

FCT_ASM(initBtnsStates)
	MOV isDownBtn.all, 0
	ADD ESP, 0x104
	RETN
}}

void Install_InterfaceStash()
{
	static bool isInstalled = false;
	if (isInstalled) return;

	Install_UpdateServer();
	Install_AlpacaImagesFiles();

	log_msg("[Patch] Stash Interface\n");

	DWORD ButtonImagesOffset = D2Client::GetAddress(0x39060);
	DWORD PageNumberOffset = D2Client::GetAddress(0x3903C);
	DWORD MouseDownSoundOffset = D2Client::GetAddress(0x45091);
	DWORD MouseUpSoundOffset = D2Client::GetAddress(0x455F9);
	DWORD OpenStashPageInitStateOffset = D2Client::GetAddress(0x45B3A);
	DWORD GreenSetItemSearchOffset = D2Client::GetAddress(0x3F098);
	DWORD GreenSetItemNextSearchOffset = D2Client::GetAddress(0x3F0FA);
	
	// Print button images
	Memory::SetCursor(ButtonImagesOffset);
	Memory::ChangeCallC((DWORD)D2Client::D2LoadBuySelBtn, (DWORD)printBtns);

	// print page number
	Memory::SetCursor(PageNumberOffset);
	Memory::ChangeCallB((DWORD)D2Win::D2PrintString, (DWORD)printPageNumber);

	// Manage mouse down (Play sound)
	Memory::SetCursor(MouseDownSoundOffset);
	Memory::ChangeCallC((DWORD)D2Client::IsExpansion, (DWORD)caller_manageBtnDown);

	// Manage mouse up (Play sound)
	Memory::SetCursor(MouseUpSoundOffset);
	Memory::ChangeCallC((DWORD)D2Client::IsExpansion, (DWORD)caller_manageBtnUp);

	// init state of button on open stash page
	Memory::SetCursor(OpenStashPageInitStateOffset);
	Memory::ChangeByte(0x81, 0xE9);
	Memory::ChangeCallA(0x104C4, (DWORD)initBtnsStates);
	Memory::ChangeByte(0, 0x90);

	// init the search of print in green the item set name we have in stash
	Memory::SetCursor(GreenSetItemSearchOffset);
	Memory::ChangeCallB((DWORD)D2Common::D2InventoryGetFirstItem, (DWORD)initGetNextItemForSet);

	// Get next item for print in green the item set name we have in stash
	Memory::SetCursor(GreenSetItemNextSearchOffset);
	Memory::ChangeCallB((DWORD)D2Common::D2UnitGetNextItem, (DWORD)getNextItemForSet);

	if (active_logFileMemory) log_msg("\n");
	isInstalled = true;
}
