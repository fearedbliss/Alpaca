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

#include "D2Client.h"
#include "D2Common.h"
#include "Fog.h"

void D2Client::Init()
{
	Name = "D2Client.dll";
	Offset = LoadDiabloLibrary();
	SetFunctions();
}

void D2Client::SetFunctions()
{
	D2LoadBuySelBtn = (TD2LoadBuySelBtn)GetAddress(0x897B0);
	D2GetClientPlayer = (TD2GetClientPlayer)GetAddress(0x8DC40);
	D2LoadImage = (TD2LoadImage)GetAddress(0x1000);
	D2PlaySound = (TD2PlaySound)GetAddress(0xB4360);
	IsExpansion = (TD2isLODGame)GetAddress(0xBA00);
	D2FreeImage = (TD2FreeImage)GetAddress(0x1150);
	D2SendToServer3 = (TD2SendToServer3)GetAddress(0xD210);
	D2GetMouseX = (TD2GetMouseX)GetAddress(0xB6670);
	D2GetMouseY = (TD2GetMouseY)GetAddress(0xB6680);

	// Variables
	ptResolutionY = (DWORD*)GetAddress(0xD50E8);
	ptResolutionX = (DWORD*)GetAddress(0xD50EC);
	ptNegWindowStartY = (DWORD*)GetAddress(0x125AD8);
	ptWindowStartX = (DWORD*)GetAddress(0x125AD4);
}

DWORD D2Client::ResolutionX()
{
	return *ptResolutionX;
}

DWORD D2Client::ResolutionY()
{
	return *ptResolutionY;
}

D2Client::TD2isLODGame D2Client::IsExpansion;
D2Client::TD2LoadImage D2Client::D2LoadImage;
D2Client::TD2FreeImage D2Client::D2FreeImage;
D2Client::TD2PlaySound D2Client::D2PlaySound;
D2Client::TD2SendToServer3 D2Client::D2SendToServer3;
D2Client::TD2GetMouseX D2Client::D2GetMouseX;
D2Client::TD2GetMouseY D2Client::D2GetMouseY;
D2Client::TD2GetClientPlayer D2Client::D2GetClientPlayer;
D2Client::TD2LoadBuySelBtn D2Client::D2LoadBuySelBtn;

DWORD* D2Client::ptResolutionY;
DWORD* D2Client::ptResolutionX;
DWORD* D2Client::ptNegWindowStartY;
DWORD* D2Client::ptWindowStartX;