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

#include "Error.h"
#include "IniFile.h"
#include "Parameters.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "MainScreen.h"
#include "BigStash.h"
#include "InfinityStash.h"
#include "Commands.h"

const int BUFFER_SIZE = 1024;

char* modDataDirectory = "Alpaca";
char* parametersFileName = "Alpaca.ini";
bool active_plugin = true;

// Active Check Memory is on by default. It should only be disabled by a developer.
// Users should report bugs and/or the users environment/configuration should be inspected.
bool active_CheckMemory = true;

const char* S_GENERAL = "GENERAL";
const char* S_active_logFile = "ActiveLogFile";
const char* S_active_logFilePatches = "ActiveLogFilePatches";

const char* S_MAIN_SCREEN = "MAIN SCREEN";
const char* S_active_DiabloVersionTextChange = "ActiveDiabloVersionTextChange";
const char* S_DiabloVersionText = "DiabloVersionText";
const char* S_active_PrintAlpacaVersion = "ActivePrintAlpacaVersion";
const char* S_DiabloVersionColor = "ColorOfDiabloVersionText";
const char* S_AlpacaVersionColor = "ColorOfAlpacaVersionText";

const char* S_STASH = "STASH";
const char* S_nbPagesPerIndex = "NumberOfPagesPerIndex";

// Convert 4 char code in a DWORD code
#define BIN(A,B,C,D) ((DWORD)A) + (((DWORD)B) << 8) + (((DWORD)C) << 16) + (((DWORD)D) << 24)

bool IsEnabled(char* symbol)
{
	return atoi(symbol) == 1;
}

void GetValueFromIni(INIFile* iniFile, const char* areaName, const char* optionName, const char* defaultValue, char* buffer, DWORD maxSize)
{
	if (!iniFile->GetPrivateProfileString(areaName, optionName, defaultValue, buffer, maxSize))
	{
		log_msg("[Error] Failed to load \"%s\" from configuration file. Defaulting to %s ...", optionName, defaultValue);
	}
}

void LogParameterBooleanValue(const char* parameterName, bool parameterValue)
{
	log_msg("%s = %u\n", parameterName, parameterValue);
}

void LogParameterIntegerValue(const char* parameterName, DWORD parameterValue)
{
	log_msg("%s = %d\n", parameterName, parameterValue);
}

void LogParameterStringValue(const char* parameterName, const char* parameterValue)
{
	log_msg("%s = %s\n", parameterName, parameterValue);
}

void init_General(INIFile* iniFile, char* buffer, DWORD maxSize)
{
	// Active Log File
	GetValueFromIni(iniFile, S_GENERAL, S_active_logFile, "0", buffer, maxSize);

	// Temporarily save the log value into this value since we are still logging data
	// during initialization. We will turn off logging at the end before we go into D2 if needed.
	active_logFileIniOriginal = IsEnabled(buffer);

	LogParameterBooleanValue(S_active_logFile, active_logFileIniOriginal);

	// Active Log File [Patches]
	GetValueFromIni(iniFile, S_GENERAL, S_active_logFilePatches, "0", buffer, maxSize);
	active_logFileMemory = IsEnabled(buffer);
	LogParameterBooleanValue(S_active_logFilePatches, active_logFileMemory);
}

void init_MainMenu(INIFile* iniFile, char* buffer, DWORD maxSize)
{
	GetValueFromIni(iniFile, S_MAIN_SCREEN, S_active_DiabloVersionTextChange, "0", buffer, maxSize);
	active_DiabloVersionTextChange = IsEnabled(buffer);
	LogParameterBooleanValue(S_active_DiabloVersionTextChange, active_DiabloVersionTextChange);
	if (active_DiabloVersionTextChange)
	{
		GetValueFromIni(iniFile, S_MAIN_SCREEN, S_DiabloVersionText, DiabloVersionText, buffer, maxSize);
		SetDiabloVersionTextDynamically(buffer);
		LogParameterStringValue(S_DiabloVersionText, DiabloVersionText);
		GetValueFromIni(iniFile, S_MAIN_SCREEN, S_DiabloVersionColor, "0", buffer, maxSize);
		DiabloVersionColor = atoi(buffer);
		LogParameterIntegerValue(S_DiabloVersionColor, DiabloVersionColor);
	}
	GetValueFromIni(iniFile, S_MAIN_SCREEN, S_active_PrintAlpacaVersion, "1", buffer, maxSize);
	active_PrintAlpacaVersion = IsEnabled(buffer);
	LogParameterBooleanValue(S_active_PrintAlpacaVersion, active_PrintAlpacaVersion);
	if (active_PrintAlpacaVersion)
	{
		GetValueFromIni(iniFile, S_MAIN_SCREEN, S_AlpacaVersionColor, "4", buffer, maxSize);
		AlpacaVersionColor = atoi(buffer);
		LogParameterIntegerValue(S_AlpacaVersionColor, AlpacaVersionColor);
	}
	LogParameterBooleanValue(S_active_logFile, active_logFileIniOriginal);
}

void SetDiabloVersionTextDynamically(char* versionBuffer)
{
	if (!versionBuffer[0])
	{
		strcpy(versionBuffer, "v ");
		strcat(versionBuffer, VersionUtility::GetVersionAsString());
	}
	DiabloVersionText = (char*)Fog::D2FogMemAlloc(strlen(versionBuffer) + 1, __FILE__, __LINE__, 0);
	strcpy(DiabloVersionText, versionBuffer);
}

void LoadParameters()
{
	char buffer[BUFFER_SIZE];
	INIFile* iniFile = new INIFile;

	srand((UINT)time(NULL));

	bool wasFileSuccessfullyOpened = iniFile->InitReadWrite(parametersFileName, INIFILE_READ, 0);

	if (wasFileSuccessfullyOpened)
	{
		log_msg("Parameters\n");
		log_msg("====================================\n");

		init_General(iniFile, buffer, BUFFER_SIZE);
		init_MainMenu(iniFile, buffer, BUFFER_SIZE);

		log_msg("\n");
	}
	else
	{
		// Set Diablo Version Fallback Text Dynamically
		char tempDiabloVersionBuffer[BUFFER_SIZE];
		SetDiabloVersionTextDynamically(tempDiabloVersionBuffer);
	}

	if (iniFile)
	{
		iniFile->close();
	}

	delete iniFile;
}