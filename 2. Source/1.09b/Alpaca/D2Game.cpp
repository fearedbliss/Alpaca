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

#include "D2Game.h"
#include "D2Common.h"

void D2Game::Init()
{
	Name = "D2Game.dll";
	Offset = LoadDiabloLibrary();
	SetFunctions();
}

void D2Game::SetFunctions()
{
	D2GetClient = (TD2GetClient)GetAddress(0x7C2C0);
	D2SendPacket = (TD2SendPacket)GetAddress(0xC380);
	D2LoadInventory = (TD2LoadInventory)GetAddress(0x4F100);
	D2GameGetObject = (TD2GameGetObject)GetAddress(0x7BAE0);
	ptClientTable = (NetClient**)GetAddress(0xF2A80);
}

D2Game::TD2GetClient D2Game::D2GetClient;
D2Game::TD2SendPacket D2Game::D2SendPacket;
D2Game::TD2LoadInventory D2Game::D2LoadInventory;
D2Game::TD2GameGetObject D2Game::D2GameGetObject;
NetClient** D2Game::ptClientTable;