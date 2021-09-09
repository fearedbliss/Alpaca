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

#include "LibraryLoader.h"

struct s_shifting {
	DWORD ptInventory;
	DWORD ptSpecificData;
	DWORD ptPYPlayerData;
	DWORD ptGame;
	DWORD ptClientGame;
	DWORD ptSkills;
	DWORD ptImage;
	DWORD ptFrame;
};
extern s_shifting shifting;

#define FCT_ASM(N) __declspec(naked) void N() {__asm{
#define RANDOM(V) ((int)(rand()/(RAND_MAX+1.0)*(V)))

#define PCPlayerData (*(PlayerData**)((DWORD)(ptChar)+shifting.ptSpecificData))
#define PCGame (*(GameStruct**)((DWORD)(ptChar)+shifting.ptGame))
#define PClientGame (*(GameStruct**)((DWORD)(ptClient)+shifting.ptClientGame))
#define PCInventory (*(Inventory**)((DWORD)(ptChar)+shifting.ptInventory))
#define PCPY ((PYPlayerData*)((DWORD)PCPlayerData+shifting.ptPYPlayerData))
#define PCSkills (*(Skills**)((DWORD)(ptChar)+shifting.ptSkills))

#define RX(v) (*D2Client::ptWindowStartX + (v))
#define RY(v) (D2Client::ResolutionY() + *D2Client::ptNegWindowStartY - (v))

extern DataTables* SgptDataTables;
void setImage(sDrawImageInfo* data, void* image);
void setFrame(sDrawImageInfo* data, DWORD frame);

void InitializeDiabloFunctions();