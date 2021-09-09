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

#include "Common.h"

s_shifting shifting;

void setImage(sDrawImageInfo* data, void* image){((void**)data)[shifting.ptImage/4]=image;} //0x4 0x8 0x3C
void setFrame(sDrawImageInfo* data, DWORD frame){((DWORD*)data)[shifting.ptFrame/4]=frame;} //0x8 0x44 0x40

void InitializeDiabloFunctions()
{	
	// TODO: All the stubs in this function have been removed.
	// All we need to do is move the remaining two functions and the
	// below structure into a more appropriate location.

	// Structure Management
	shifting.ptPYPlayerData = *(DWORD*)((DWORD)D2Common::D2InitPlayerData + D2Common::ptPYPlayerDataOffset);
	shifting.ptSpecificData = D2Common::ptSpecificDataOffset;
	shifting.ptGame = D2Common::ptGameOffset;
	shifting.ptClientGame = D2Common::ptClientGameOffset;
	shifting.ptInventory = D2Common::ptInventoryOffset;
	shifting.ptSkills = D2Common::ptSkillsOffset;
	shifting.ptImage = D2Common::ptImageOffset;
	shifting.ptFrame = D2Common::ptFrameOffset;
}