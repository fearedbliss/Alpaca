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

#include "D2Common.h"
#include "Fog.h"

void D2Common::Init()
{
	Name = "D2Common.dll";
	Offset = LoadDiabloLibrary();
	SetFunctions();
}

void D2Common::SetFunctions()
{
	D2InvAddItem = (TD2InvAddItem)GetAddress(0x4F930);
	D2InvRemoveItem = (TD2InvRemoveItem)GetAddress(0x4E660);
	D2InventoryGetFirstItem = (TD2InventoryGetFirstItem)GetAddress(0x4E7A0);
	D2UnitGetNextItem = (TD2UnitGetNextItem)GetAddress(0x52080);
	D2InitPlayerData = (TD2InitPlayerData)GetAddress(0x80320);
	D2ItemSetPage = (TD2ItemSetPage)GetAddress(0x58900);
	D2ItemGetPage = (TD2ItemGetPage)GetAddress(0x588E0);
	D2SaveItem = (TD2SaveItem)GetAddress(0x62BA0);
	D2ReadFile = (TD2ReadFile)GetAddress(0x84268);
	D2CompileTxtFile = (TD2CompileTxtFile)compileTxtFile_110;
	D2GetRealItem = (TD2GetRealItem)GetAddress(0x520C0);
	D2CompileTxtFileDirect = (TD2CompileTxtFile)GetAddress(0xFD70);
	D2GetItemQuality = (TD2GetItemQuality)GetAddress(0x58550);
	D2GetUniqueID = (TD2GetUniqueID)GetAddress(0x5DEE0);
	D2GetItemsBIN = (TD2GetItemsBIN)GetAddress(0x17680);

	// Variables: Structure Management [Offset]
	ptPYPlayerDataOffset = 0x5D;
	ptSpecificDataOffset = 0x14;
	ptGameOffset = 0x80;
	ptClientGameOffset = 0x1A8;
	ptInventoryOffset = 0x60;
	ptSkillsOffset = 0xA8;
	ptImageOffset = 0x04;
	ptFrameOffset = 0x08;

	SgptDataTablesOffset = GetAddress(0x96A20);
}

DataTables* D2Common::GetDataTables()
{
	return *(DataTables**)D2Common::SgptDataTablesOffset;
}

char* D2Common::CompileTxtFileName = "D2CompileTxtFile";
char* D2Common::ErrorReadTxtFileName = "pbData";

__declspec(naked) void* __stdcall D2Common::compileTxtFile_110(DWORD unused, const char* filename, BINField* ptFields, DWORD* ptRecordCount, DWORD recordLength)
{
	_asm {
		SUB ESP, 0x210
		PUSH EBX
		PUSH EBP
		MOV EBP, DWORD PTR SS : [ESP + 0x220]
		PUSH ESI
		PUSH EDI
		MOV DWORD PTR SS : [ESP + 0x10] , 0
		MOV EBX, wsprintf
		PUSH EBP
		LEA EAX, DWORD PTR SS : [ESP + 0x20]
		PUSH EAX
		CALL EBX
		ADD ESP, 8
		LEA EDX, DWORD PTR SS : [ESP + 0x10]
		PUSH 0
		PUSH D2Common::CompileTxtFileName
		PUSH EDX
		MOV ECX, DWORD PTR SS : [ESP + 0x230]
		LEA EDX, DWORD PTR SS : [ESP + 0x28]
		CALL D2Common::D2ReadFile
		TEST EAX, EAX
		JNZ continue_compileTxtFile
		PUSH 0
		PUSH D2Common::CompileTxtFileName
		PUSH D2Common::ErrorReadTxtFileName
		CALL Fog::D2FogAssert
		PUSH - 1
		CALL exit
		continue_compileTxtFile :
		MOV ECX, D2Common::D2CompileTxtFileDirect
			ADD ECX, 0x2ED
			JMP ECX
	}
}

D2Common::TD2CompileTxtFile D2Common::D2CompileTxtFile;
D2Common::TD2GetRealItem D2Common::D2GetRealItem;

DWORD D2Common::ptPYPlayerDataOffset;
DWORD D2Common::ptSpecificDataOffset;
DWORD D2Common::ptGameOffset;
DWORD D2Common::ptClientGameOffset;
DWORD D2Common::ptInventoryOffset;
DWORD D2Common::ptSkillsOffset;
DWORD D2Common::ptImageOffset;
DWORD D2Common::ptFrameOffset;
DWORD D2Common::SgptDataTablesOffset;

D2Common::TD2InvAddItem D2Common::D2InvAddItem;
D2Common::TD2InvRemoveItem D2Common::D2InvRemoveItem;
D2Common::TD2InventoryGetFirstItem D2Common::D2InventoryGetFirstItem;
D2Common::TD2UnitGetNextItem D2Common::D2UnitGetNextItem;
D2Common::TD2InitPlayerData D2Common::D2InitPlayerData;
D2Common::TD2CompileTxtFile D2Common::D2CompileTxtFileDirect;
D2Common::TD2GetItemQuality D2Common::D2GetItemQuality;
D2Common::TD2GetUniqueID D2Common::D2GetUniqueID;
D2Common::TD2GetItemsBIN D2Common::D2GetItemsBIN;
D2Common::TD2ItemSetPage D2Common::D2ItemSetPage;
D2Common::TD2ItemGetPage D2Common::D2ItemGetPage;
D2Common::TD2SaveItem D2Common::D2SaveItem;
D2Common::TD2ReadFile D2Common::D2ReadFile;