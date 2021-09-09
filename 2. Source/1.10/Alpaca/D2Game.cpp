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
	D2GetClient = (TD2GetClient)GetAddress(0x8C2E0);
	D2SendPacket = (TD2SendPacket)GetAddress(0xC710);
	D2LoadInventory = (TD2LoadInventory)GetAddress(0x5B8A0);
	D2GameGetObject = (TD2GameGetObject)GetAddress(0x8BB00);
	ptClientTable = (NetClient**)GetAddress(0x113FB8);
	D2VerifIfNotCarry1 = (TD2VerifIfNotCarry1)GetAddress(0x128F0);
}

D2Game::TD2GetClient D2Game::D2GetClient;
D2Game::TD2SendPacket D2Game::D2SendPacket;
D2Game::TD2LoadInventory D2Game::D2LoadInventory;
D2Game::TD2GameGetObject D2Game::D2GameGetObject;
NetClient** D2Game::ptClientTable;
D2Game::TD2SendPacket D2Game::D2SendPacketDirect;
D2Game::TD2GameGetObject D2Game::D2GameGetObjectDirect;
D2Game::TD2LoadInventory D2Game::D2LoadInventoryDirect;
D2Game::TD2VerifIfNotCarry1 D2Game::D2VerifIfNotCarry1;