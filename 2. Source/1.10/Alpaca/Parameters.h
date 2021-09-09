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

#define PROGRAM_NAME "Alpaca"
#define PROGRAM_VERSION "4.2.0"
#define PROGRAM_AUTHOR_NAME "Jonathan Vasquez"
#define PROGRAM_AUTHOR_ALIAS "fearedbliss"
#define PROGRAM_BUILD_DATE "September 9, 2021 @ 15:30 ET"

extern char* modDataDirectory;
extern bool active_plugin;
extern bool active_CheckMemory;

void LoadParameters();
void SetDiabloVersionTextDynamically(char* versionBuffer);