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

#include "Common.h"

extern const DWORD maxSelfPages;
extern const DWORD nbPagesPerIndex;
extern const DWORD nbPagesJump;

extern bool displaySharedSetItemNameInGreen;
extern const int posXPreviousBtn;
extern const int posYPreviousBtn;
extern const int posXNextBtn;
extern const int posYNextBtn;

void setCurrentStashIndex(Unit* ptChar, int index);
void selectPreviousStash(Unit* ptChar);
void selectNextStash(Unit* ptChar);
void selectPreviousIndexStash(Unit* ptChar);
void selectNextIndexStash(Unit* ptChar);
void selectPreviousStashJump(Unit* ptChar);
void selectNextStashJump(Unit* ptChar);

void swapStash(Unit* ptChar, DWORD targetPageIndex);
void renameCurrentStash(Unit* ptChar, char* name);
void jumpToPage(Unit* ptChar, DWORD targetPageIndex);
bool isStashPageValid(int pageIndex);

void rememberLastSelectedStash(Unit* ptChar, Stash* selectedStash, bool isRunningDuringInit);
void selectStash(Unit* ptChar, Stash* newStash, bool isRunningDuringInit = false);
void setSelectedStashClient(DWORD stashId, DWORD stashFlags, DWORD flags, bool bOnlyItems);

Stash* addStash(Unit* ptChar);
DWORD loadStashList(Unit* ptChar, BYTE* data, DWORD maxSize, DWORD* curSize);
void saveStashList(Unit* ptChar, Stash* ptStash, BYTE** data, DWORD* maxSize, DWORD* curSize);

Stash* createStashesUpToPageIndex(Unit* ptChar, Stash* currentStash, DWORD targetPageIndex);

void Install_MultiPageStash();
