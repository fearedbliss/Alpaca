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

enum D2Color
{
	WHITE=0,
	RED=1,
	GREEN=2,
	BLUE=3,
	GOLD=4,
	DARK_GRAY=5,
//	INVISIBLE=6,
	DARK_YELLOW=7,
	ORANGE=8,
	YELLOW=9,
	DARKEST_GREEN=0xA,
	PURPLE=0xB,
	DARK_GREEN=0xC,
//	WHITE_BIS=0xD,
//	INVISIBLE2=0xE,
	LIGHT_GRAY=0xF,
	GRAY=0x10,
//	VERY_WIERD=0x11,
	BRIGHT_WHITE=0x12,
	CRYSTAL_RED=0x13,
//	WIERD_YELLOW=0x14
};

enum D2UnitType
{
	UNIT_PLAYER = 0,
	UNIT_MONSTER,
	UNIT_OBJECT,
	UNIT_MISSILE,
	UNIT_ITEM,
	UNIT_ROOMTILE
};