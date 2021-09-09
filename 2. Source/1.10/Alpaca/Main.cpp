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

#include "Commands.h"
#include "MainScreen.h"
#include "BigStash.h"
#include "InfinityStash.h"
#include "Common.h"

void InstallAlpacaFunctions();

// Ensure that even though we have/allow both DllMain and Init Loading Systems,
// they usually will both be triggered, and thus we need a way to avoid
// an unnecessary second load.
bool isAlpacaLoaded = false;
bool isAlpacaExited = false;

void OnEntry()
{
	if (isAlpacaLoaded) return;

	// If you want to debug all of the initialization code
	// you can uncomment the below MessageBox and put a breakpoint after it.
	// Reason for this is that this code happens quickly and very early before D2
	// fully starts, thus by the time you can attach your debugger to Game.exe,
	// all of this code already finished. So we can use the MessageBox trick.
	// Thanks to Necrolis @ PhrozenKeep for bringing this trick up.
	//MessageBox(GetActiveWindow(), "[Alpaca] The Alpacas have arrived!", "Alpaca", MB_APPLMODAL);

	LibraryLoader::Init();
	InitializeDiabloFunctions();
	LoadParameters();
	InstallAlpacaFunctions();

	log_msg("Entering Diablo II\n");
	log_msg("====================================\n");
	active_logFile = active_logFileIniOriginal;
	isAlpacaLoaded = true;
}

void OnExit()
{
	if (isAlpacaExited) return;

	active_logFile = true;
	log_msg("\nExiting Diablo II\n");
	log_msg("====================================\n");
	isAlpacaExited = true;
}

// Modern Loading (D2GFEx, Standard Windows LoadLibrary)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OnEntry();
		break;
	case DLL_PROCESS_DETACH:
		OnExit();
		break;
	}
	return TRUE;
}

// Old System (Non-D2GFEx, Injected w/ Alpaca Launcher)
extern "C" __declspec(dllexport) void* __stdcall Init(LPSTR IniName)
{
	OnEntry();
	return NULL;
}

extern "C" __declspec(dllexport) bool __stdcall Release()
{
	OnExit();
	return true;
}

void InstallAlpacaFunctions()
{
	LibraryLoader::HookLibraries();

	log_msg("Alpaca Patches\n");
	log_msg("====================================\n");

	Install_Commands();

	if (active_DiabloVersionTextChange)
	{
		Install_VersionChange();
	}
	
	if (active_PrintAlpacaVersion)
	{
		Install_PrintAlpacaVersion();
	}
	
	Install_BigStash();
	Install_MultiPageStash();

	log_msg("\n");

	LibraryLoader::UnhookLibraries();
}