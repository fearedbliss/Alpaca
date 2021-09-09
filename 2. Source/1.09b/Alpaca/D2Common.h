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

#pragma once

#include "Library.h"

class D2Common : public Library<D2Common>
{
public:
	static void Init();

	typedef Unit* (__stdcall* TD2Common10242) (Inventory* ptInventory, Unit* ptItem, DWORD bIsClient);
	typedef DWORD(__stdcall* TD2InvAddItem) (Inventory* ptInventory, Unit* ptItem, DWORD posX, DWORD posY, DWORD vValueC, DWORD bIsClient, BYTE page);
	typedef DWORD(__stdcall* TD2Common10250) (const char* file, DWORD line, Inventory* ptInventory, DWORD posX, DWORD posY, DWORD vValueC, DWORD bIsClient, BYTE page);
	typedef Unit* (__stdcall* TD2InventoryGetFirstItem) (Inventory* ptInventory);
	typedef Unit* (__stdcall* TD2UnitGetNextItem) (Unit* ptItem);
	typedef Unit* (__stdcall* TD2GetRealItem) (Unit* ptUnit);
	typedef DWORD(__stdcall* TD2GetPosX) (Unit* ptUnit);
	typedef DWORD(__stdcall* TD2GetPosY) (Unit* ptUnit);
	typedef PlayerData* (__stdcall* TD2InitPlayerData) (Unit* ptChar);
	typedef void* (__stdcall* TD2CompileTxtFile) (DWORD unused, const char* filename, BINField* ptFields, DWORD* ptRecordCount, DWORD recordLength);
	typedef BYTE(__stdcall* TD2ItemGetPage) (Unit* ptUnit);
	typedef void(__stdcall* TD2SetAnim) (Unit* ptUnit, int anim);
	typedef DWORD(__stdcall* TD2SaveItem) (Unit* ptItem, saveBitField* data, DWORD startSize, DWORD p4, DWORD p5, DWORD p6);
	typedef void* (__fastcall* TD2ReadFile) (DWORD unused, char* filename, DWORD* size, const char*, DWORD);

	static TD2Common10242 D2Common10242;
	static TD2InvAddItem D2InvAddItem;
	static TD2Common10250 D2Common10250;
	static TD2InventoryGetFirstItem D2InventoryGetFirstItem;
	static TD2UnitGetNextItem D2UnitGetNextItem;
	static TD2GetRealItem D2GetRealItem;
	static TD2GetPosX D2GetPosX;
	static TD2GetPosY D2GetPosY;
	static TD2InitPlayerData D2InitPlayerData;

	// Some functions use the base function directly
	static TD2CompileTxtFile D2CompileTxtFileDirect;

	// Some functions use the wrapped version
	static TD2CompileTxtFile D2CompileTxtFile;
	static TD2ItemGetPage D2ItemGetPage;
	static TD2SetAnim D2SetAnim;
	static TD2SaveItem D2SaveItem;
	static TD2ReadFile D2ReadFile;

	// Variables: Structure Management
	static DWORD ptPYPlayerDataOffset;
	static DWORD ptSpecificDataOffset;
	static DWORD ptGameOffset;
	static DWORD ptClientGameOffset;
	static DWORD ptInventoryOffset;
	static DWORD ptSkillsOffset;
	static DWORD ptImageOffset;
	static DWORD ptFrameOffset;
private:
	static void SetFunctions();

	static char* CompileTxtFileName;
	static char* ErrorReadTxtFileName;
	static void* __stdcall compileTxtFile_9(DWORD unused, const char* filename, BINField* ptFields, DWORD* ptRecordCount, DWORD recordLength);
};