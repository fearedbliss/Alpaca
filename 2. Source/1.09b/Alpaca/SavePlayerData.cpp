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

#include "InfinityStash.h"
#include "ExtendedSaveFile.h"
#include "Common.h"

void __stdcall SaveSPPlayerCustomData(Unit* ptChar)
{
	log_msg("\n--- Start SaveSPPlayerCustomData ---\n");

	if (PCPY->selfStashIsOpened)
	{
		DWORD curSizeExt = 0;
		DWORD maxSizeExt = 0x4000;
		BYTE* dataExt = (BYTE *)Fog::D2AllocMem(PCGame->memoryPool, maxSizeExt,__FILE__,__LINE__,0);
		d2_assert(!dataExt, "Error : Memory allocation Extended SaveFile", __FILE__, __LINE__);
		saveExtendedSaveFile(ptChar, &dataExt, &maxSizeExt, &curSizeExt);
		writeExtendedSaveFile(PCPlayerData->name, dataExt, curSizeExt);
		Fog::D2FreeMem(PCGame->memoryPool, dataExt,__FILE__,__LINE__,0);
	}
	
	log_msg("End saving.\n\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
typedef struct {
	BYTE	packID;
	BYTE	packSize;
	BYTE	init;
	DWORD	finalSize:31;
	DWORD	isCustom:1;
	BYTE	type;
	BYTE	data[0xFF];
} t_rcvMsg;
#pragma pack()

struct s_MPSaveFile
{
	int		sizeExtended;
	int		curExtended;
	BYTE*	dataExtended;
};
static s_MPSaveFile receivedSaveFiles;
static BYTE customPackID = 0xB2;

struct s_dataToSend
{
	s_dataToSend*	next;
	int		clientID;
	int		sizeExtended;
	int		curExtended;
	BYTE*	dataExtended;
	bool	init;
};
s_dataToSend* ptDataToSend=NULL;

void __stdcall SendSaveFilesToSave(Unit* ptChar)
{
	log_msg("\n--- Start SendSaveFilesToSave ---\n");

	DWORD curSizeExt = 0;
	BYTE* dataExt = NULL;

	if (PCPY->selfStashIsOpened)
	{
		DWORD maxSizeExt = 0x4000;
		dataExt = (BYTE *)Fog::D2AllocMem(PCGame->memoryPool, maxSizeExt,__FILE__,__LINE__,0);
		d2_assert(!dataExt, "Error : Memory allocation Extended SaveFile", __FILE__, __LINE__);
		saveExtendedSaveFile(ptChar, &dataExt, &maxSizeExt, &curSizeExt);
	}

	NetClient* ptClient = D2Game::D2GetClient(ptChar,__FILE__,__LINE__);
	s_dataToSend* dataToSend = ptDataToSend;
	while (dataToSend && (dataToSend->clientID != ptClient->clientID))
		dataToSend = dataToSend->next;
	if (!dataToSend)
	{
		dataToSend = (s_dataToSend*)Fog::D2AllocMem(PCGame->memoryPool, sizeof(s_dataToSend), __FILE__, __LINE__, 0);
		ZeroMemory(dataToSend, sizeof(s_dataToSend));
		dataToSend->next = ptDataToSend;
		ptDataToSend = dataToSend;
	}

	//Sending savefiles
	dataToSend->clientID = ptClient->clientID;
	dataToSend->init = 1;
	dataToSend->sizeExtended = curSizeExt;
	dataToSend->curExtended = 0;
	dataToSend->dataExtended = dataExt;

	log_msg("clientID=%d\t init=%d\t sizeExtended=%X\t curExtended=%X\t dataExtended=%X\n", 
		dataToSend->clientID, dataToSend->init, dataToSend->sizeExtended, dataToSend->curExtended, dataToSend->dataExtended);

	log_msg("End SendSaveFilesToSave.\n\n");
}

DWORD __stdcall ManageNextPacketToSend(NetClient* ptClient)
{
	log_msg("ManageNextPacketToSend: ");
	s_dataToSend* dataToSend = ptDataToSend;
	while (dataToSend && (dataToSend->clientID != ptClient->clientID))
		dataToSend = dataToSend->next;
	if (!dataToSend) return 1;

	log_msg("clientID=%d\t init=%d\t sizeExtended=%X\t curExtended=%X\t dataExtended=%X\n", 
		dataToSend->clientID, dataToSend->init, dataToSend->sizeExtended, dataToSend->curExtended, dataToSend->dataExtended);

	if (dataToSend->sizeExtended && dataToSend->dataExtended && (dataToSend->curExtended < dataToSend->sizeExtended))
	{
		DWORD remainingData = dataToSend->sizeExtended - dataToSend->curExtended;
		t_rcvMsg* msg = (t_rcvMsg*)Fog::D2AllocMem(PClientGame->memoryPool, sizeof(t_rcvMsg),__FILE__,__LINE__,0);
		msg->packID = customPackID;
		msg->init = dataToSend->init;
		msg->finalSize = dataToSend->sizeExtended;
		msg->isCustom = true;
		msg->packSize = remainingData > 0xFE ? 0xFF : (BYTE)remainingData + 1;
		CopyMemory(msg->data, &dataToSend->dataExtended[dataToSend->curExtended], msg->packSize);
		log_msg("Saving Send Packet: init=%d\t finalSize=%X\t packSize=%02X\t data=%08X\n", msg->init, msg->finalSize, msg->packSize, msg->data);
		D2Net::D2SendToClient(0, dataToSend->clientID, msg, msg->packSize+7);
		dataToSend->init = false;
		dataToSend->curExtended += msg->packSize - 1;
		Fog::D2FreeMem(PClientGame->memoryPool, msg,__FILE__,__LINE__,0);
		if (dataToSend->curExtended == dataToSend->sizeExtended)
		{
			dataToSend->sizeExtended = 0;
			dataToSend->curExtended = 0;
			Fog::D2FreeMem(PClientGame->memoryPool, dataToSend->dataExtended,__FILE__,__LINE__,0);
			dataToSend->dataExtended = NULL;
		}
		log_msg("End Send Packet\n");
		return 0;
	}

	log_msg("End Send Packet\n");
	return 1;
}


DWORD __stdcall ReceiveSaveFilesToSave(t_rcvMsg* msg)
{
	if( (msg->packID != customPackID) || !msg->isCustom) return 0;

	log_msg("Saving Receive Packet: init=%d\t finalSize=%X\t packSize=%02X\t data=%08X\n", msg->init, msg->finalSize, msg->packSize, msg->data);

	if (msg->init)
	{
		Fog::D2FogMemDeAlloc(receivedSaveFiles.dataExtended,__FILE__,__LINE__,0);
		ZeroMemory(&receivedSaveFiles, sizeof(receivedSaveFiles));
	}

	log_msg("receivedSaveFiles: sizeExtended=%d\t curExtended=%d\t dataExtended=%08X\n", receivedSaveFiles.sizeExtended, receivedSaveFiles.curExtended, receivedSaveFiles.dataExtended);

	DWORD size = msg->packSize - 1;
	
	if (receivedSaveFiles.sizeExtended == 0)
		receivedSaveFiles.sizeExtended = msg->finalSize;

	if (!receivedSaveFiles.dataExtended)
		receivedSaveFiles.dataExtended = (BYTE *)Fog::D2FogMemAlloc(receivedSaveFiles.sizeExtended,__FILE__,__LINE__,0);

	CopyMemory(&receivedSaveFiles.dataExtended[receivedSaveFiles.curExtended], msg->data, size);
	receivedSaveFiles.curExtended += size;
	
	log_msg("End Save Receive Packet\n");
	return 1;
}


void __stdcall SaveMPPlayerCustomData(BYTE* dataD2Savefile)
{
	log_msg("[SaveMPPlayerCustomData] Function Started\n");

	Unit* ptChar = D2Client::D2GetClientPlayer();
	if (ptChar)
	{
		if (receivedSaveFiles.sizeExtended && (receivedSaveFiles.sizeExtended == receivedSaveFiles.curExtended))
		{
			log_msg("[SaveMPPlayerCustomData] Saving Received Save File ...\n");

			writeExtendedSaveFile(PCPlayerData->name, receivedSaveFiles.dataExtended, receivedSaveFiles.sizeExtended);
			Fog::D2FogMemDeAlloc(receivedSaveFiles.dataExtended, __FILE__, __LINE__, 0);
			ZeroMemory(&receivedSaveFiles, sizeof(receivedSaveFiles));

			log_msg("[SaveMPPlayerCustomData] Received Save File Successfully Saved!\n\n");
		}
		else
		{
			log_box("[SaveMPPlayerCustomData] Connection error. Unable to save the received save file.");
		}
	}

	log_msg("[SaveMPPlayerCustomData] Function Completed\n\n");
}

FCT_ASM(caller_SaveSPPlayerCustomData_9)
	CALL Fog::D2FogGetSavePath
	PUSH ESI
	CALL SaveSPPlayerCustomData
	RETN
}}

FCT_ASM(caller_ManageNextPacketToSend_9)
	PUSH ESI
	CALL ManageNextPacketToSend
	TEST EAX, EAX
	JNZ end_caller_ManageNextPacketToSend_9
	MOV DWORD PTR DS : [ESI + 0x14C] , 5
	POP ESI
	POP ESI
	RETN
	end_caller_ManageNextPacketToSend_9 :
	MOV EAX, DWORD PTR DS : [ESI + 0x150]
	RETN
}}

FCT_ASM(caller_SendSaveFilesToSave_9)
	PUSH ESI
	CALL SendSaveFilesToSave
	MOV ESI, DWORD PTR SS : [ESP + 0x14]
	TEST ESI, ESI
	RETN
}}

FCT_ASM(caller_ReceivedSaveFilesToSave)
	CMP EDX, -1
	JE continue_rcvFct
	PUSH ECX
	PUSH EDX
	PUSH ECX
	CALL ReceiveSaveFilesToSave
	POP EDX
	POP ECX
	TEST EAX, EAX
	JE continue_rcvFct
	XOR EAX, EAX
	ADD ESP, 4
	RETN
continue_rcvFct:
	POP EAX
	SUB ESP, 0x5F4//5F4
	JMP EAX
}}

FCT_ASM(caller_SaveMPPlayerCustomData)
	PUSH ECX
	PUSH ECX
	CALL SaveMPPlayerCustomData
	POP ECX
	MOV EAX, DWORD PTR DS : [ECX]
	MOV EDX, DWORD PTR DS : [ECX + 4]
	RETN
}}

void Install_SavePlayerData()
{
	static bool isInstalled = false;
	if (isInstalled || !active_PlayerCustomData) return;

	log_msg("[Patch] Save Player Custom Data\n");

	DWORD SaveSinglePlayerCustomDataOffset = D2Game::GetAddress(0x4DF04);
	DWORD SendSaveFilesOffset1 = D2Game::GetAddress(0x4DFFA);
	DWORD SendSaveFilesOffset2 = D2Game::GetAddress(0x7993);
	DWORD ReceivedSaveFilesOffset = D2Client::GetAddress(0x116F0);
	DWORD SaveMultiplayerPlayerCustomDataOffset = D2Client::GetAddress(0x117FC);

	// Save single player custom data.
	Memory::SetCursor(SaveSinglePlayerCustomDataOffset);
	Memory::ChangeCallB((DWORD)Fog::D2FogGetSavePath, (DWORD)caller_SaveSPPlayerCustomData_9);

	// Send SaveFiles
	Memory::SetCursor(SendSaveFilesOffset1);
	Memory::ChangeByte(0x8B, 0x90);
	Memory::ChangeByte(0x74, 0xE8);
	Memory::ChangeCallA(0xF6851024, (DWORD)caller_SendSaveFilesToSave_9);

	Memory::SetCursor(SendSaveFilesOffset2);
	Memory::ChangeByte(0x8B, 0x90);
	Memory::ChangeByte(0x86, 0xE8);
	Memory::ChangeCallA(0x150, (DWORD)caller_ManageNextPacketToSend_9);
	
	// Received SaveFiles
	Memory::SetCursor(ReceivedSaveFilesOffset);
	Memory::ChangeByte(0x81, 0x90);
	Memory::ChangeByte(0xEC, 0xE8);
	Memory::ChangeCallA(0x5F4, (DWORD)caller_ReceivedSaveFilesToSave);

	// Save multiplayer player custom data.
	Memory::SetCursor(SaveMultiplayerPlayerCustomDataOffset);
	Memory::ChangeByte(0x8B, 0xE8);
	Memory::ChangeCallA(0x04518B01, (DWORD)caller_SaveMPPlayerCustomData);

	customPackID = 0xAB;

	if (active_logFileMemory) log_msg("\n");
	isInstalled = true;
}