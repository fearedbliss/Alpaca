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

#include "AlpacaFiles.h"
#include "Common.h"

const char* classicStash_RefFile   = "%s\\bank";
const char* expansionStash_RefFile = "%s\\TradeStash";

const BYTE gridX_BB = 10;
const BYTE gridY_BB = 10;
const DWORD gridLeft_BB = 16;
const DWORD gridRight_BB = 302;
const DWORD gridTop_BB = 82;
const DWORD gridbottom_BB = 371;

void setInventoryValues(InventoryBIN* ptInventoryBin, DWORD offset, BYTE gridX, BYTE gridY,
	DWORD gridLeft, DWORD gridRight, DWORD gridTop, DWORD gridBottom)
{
	InventoryBIN* inventory = ptInventoryBin + offset;
	inventory->gridX = gridX;
	inventory->gridY = gridY;
	inventory->gridLeft = gridLeft;
	inventory->gridRight = gridRight;
	inventory->gridTop = gridTop;
	inventory->gridBottom = gridBottom;
}

InventoryBIN* __stdcall modifStashGrid(InventoryBIN* ptInventoryBin)
{
	// Indexes are 0 based. You can omit the first line in Inventory.txt
	// (class, invLeft, etc), and also ignore the Expansion line. So this
	// means that "Big Bank Page 1" would be index 12.

	DWORD classic_lowResolutionIndex = 8;
	DWORD classic_highResolutionIndex = 24;
	DWORD expansion_lowResolutionIndex = 12;
	DWORD expansion_highResolutionIndex = 28;

	// Bank Page 1 (640x480, Classic)
	setInventoryValues(ptInventoryBin, classic_lowResolutionIndex, gridX_BB, gridY_BB, gridLeft_BB, gridRight_BB, gridTop_BB, gridbottom_BB);

	// Bank Page 2 (800x600, Classic)
	setInventoryValues(ptInventoryBin, classic_highResolutionIndex, gridX_BB, gridY_BB, gridLeft_BB + 80, gridRight_BB + 80, gridTop_BB + 60, gridbottom_BB + 60);

	// Big Bank Page 1 (640x480, Expansion)
	setInventoryValues(ptInventoryBin, expansion_lowResolutionIndex, gridX_BB, gridY_BB, gridLeft_BB, gridRight_BB, gridTop_BB, gridbottom_BB);
	
	// Big Bank Page 2 (800x600, Expansion)
	setInventoryValues(ptInventoryBin, expansion_highResolutionIndex, gridX_BB, gridY_BB, gridLeft_BB + 80, gridRight_BB + 80, gridTop_BB + 60, gridbottom_BB + 60);
	
	return ptInventoryBin;
}

FCT_ASM(caller_modifStashGrid)
	POP ESI
	CALL D2Common::D2CompileTxtFileDirect
	PUSH EAX
	CALL modifStashGrid
	JMP ESI
}}

__declspec(naked) void caller_changeClassicStash()
{
	__asm
	{
		MOV EDX, modDataDirectory
		MOV DWORD PTR SS : [ESP + 0x8] , EDX
		MOV EDX, classicStash_RefFile
		MOV DWORD PTR SS : [ESP + 0x4] , EDX
		POP EDX
		PUSH 0x104
		JMP EDX
	}
}

__declspec(naked) void caller_changeExpansionStash()
{
	__asm
	{
		MOV EAX, modDataDirectory
		MOV DWORD PTR SS : [ESP + 0x8] , EAX
		MOV EAX, expansionStash_RefFile
		MOV DWORD PTR SS : [ESP + 0x4] , EAX
		POP EAX
		PUSH 0x104
		JMP EAX
	}
}

void Install_BigStash()
{
	static bool isInstalled = false;
	if (isInstalled) return;

	Install_AlpacaFiles();

	log_msg("[Patch] Increased Stash (10x10)\n");

	DWORD StashGridOffset = D2Common::GetAddress(0x14ED3);
	DWORD StashBackgroundClassicOffset = D2Client::GetAddress(0x4C5E7);
	DWORD StashBackgroundExpansionOffset = D2Client::GetAddress(0x4C61C);

	// Classic Expanded Stash UI Variables (NOP ALL)
	DWORD ClassicStash_GoldAmountWithdrawButtonOffset = D2Client::GetAddress(0x49909);
	DWORD ClassicStash_WithdrawTextOnHoverOffset = D2Client::GetAddress(0x49956);
	DWORD ClassicStash_WithdrawButtonEventOnClickOffset = D2Client::GetAddress(0x4C28C);
	DWORD ClassicStash_ButtonDownWithdrawEventAnimationOffset = D2Client::GetAddress(0x4BB73);
	DWORD ClassicStash_GoldMaxAmountDisplayOffset = D2Client::GetAddress(0x3F331);

	// Stash Grid Size Modification (Classic/Expansion)
	Memory::SetCursor(StashGridOffset);
	Memory::ChangeCallC((DWORD)D2Common::D2CompileTxtFileDirect, (DWORD)caller_modifStashGrid);

	// Stash Background Modification (Classic)
	Memory::SetCursor(StashBackgroundClassicOffset);
	Memory::ChangeByte(0x68, 0xE8);
	Memory::ChangeCallA(0x00000104, (DWORD)caller_changeClassicStash);

	// Modification of stash background (Expansion)
	Memory::SetCursor(StashBackgroundExpansionOffset);
	Memory::ChangeByte(0x68, 0xE8);
	Memory::ChangeCallA(0x00000104, (DWORD)caller_changeExpansionStash);

	// Classic Expanded Stash UI Controls

	// Gold Amount/Withdraw Button: 49909 [NOP]
	Memory::SetCursor(ClassicStash_GoldAmountWithdrawButtonOffset);
	Memory::ChangeByte(0x74, 0x90);
	Memory::ChangeByte(0x14, 0x90);

	// Withdraw Text on Hover: 49956 [NOP]
	Memory::SetCursor(ClassicStash_WithdrawTextOnHoverOffset);
	Memory::ChangeByte(0x0F, 0x90);
	Memory::ChangeByte(0x84, 0x90);
	Memory::ChangeByte(0x41, 0x90);
	Memory::ChangeByte(0x01, 0x90);
	Memory::ChangeByte(0x00, 0x90);
	Memory::ChangeByte(0x00, 0x90);

	// Withdraw Button Event on Click: 4C28C [NOP]
	Memory::SetCursor(ClassicStash_WithdrawButtonEventOnClickOffset);
	Memory::ChangeByte(0x74, 0x90);
	Memory::ChangeByte(0x2D, 0x90);

	// Button Down Press Animation for Withdraw: 4BB73 [NOP]
	Memory::SetCursor(ClassicStash_ButtonDownWithdrawEventAnimationOffset);
	Memory::ChangeByte(0x74, 0x90);
	Memory::ChangeByte(0x42, 0x90);

	// Gold Max/Amount Display: 3F331 [NOP]
	Memory::SetCursor(ClassicStash_GoldMaxAmountDisplayOffset);
	Memory::ChangeByte(0x74, 0x90);
	Memory::ChangeByte(0x1D, 0x90);

	if (active_logFileMemory) log_msg("\n");
	isInstalled = true;
}
