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

#include "Fog.h"

void Fog::Init()
{
	Name = "Fog.dll";
	Offset = LoadDiabloLibrary();
	SetFunctions();
}

void Fog::SetFunctions()
{
	D2FogAssert = (TD2FogAssert)GetAddress(0xED30);
	D2FogMemAlloc = (TD2FogMemAlloc)GetAddress(0x8F50);
	D2FogMemDeAlloc = (TD2FogMemDeAlloc)GetAddress(0x8F90);
	D2AllocMem = (TD2AllocMem)GetAddress(0x8FF0);
	D2FreeMem = (TD2FreeMem)GetAddress(0x9030);
	D2MPQOpenFile = (TD2MPQOpenFile)GetAddress(0x11600);
	D2MPQCloseFile = (TD2MPQCloseFile)GetAddress(0x11610);
	D2MPQReadFile = (TD2MPQReadFile)GetAddress(0x11620);
	D2MPQGetSizeFile = (TD2MPQGetSizeFile)GetAddress(0x11650);
	D2FogGetSavePath = (TD2FogGetSavePath)GetAddress(0x11900);
}

Fog::TD2FogAssert Fog::D2FogAssert;
Fog::TD2FogMemAlloc Fog::D2FogMemAlloc;
Fog::TD2FogMemDeAlloc Fog::D2FogMemDeAlloc;
Fog::TD2AllocMem Fog::D2AllocMem;
Fog::TD2FreeMem Fog::D2FreeMem;
Fog::TD2MPQOpenFile Fog::D2MPQOpenFile;
Fog::TD2MPQCloseFile Fog::D2MPQCloseFile;
Fog::TD2MPQReadFile Fog::D2MPQReadFile;
Fog::TD2MPQGetSizeFile Fog::D2MPQGetSizeFile;
Fog::TD2FogGetSavePath Fog::D2FogGetSavePath;