#pragma once
#include "MiscClasses.h"
#include "UTIL Functions.h"


CBaseHudChat* IClientModeShared::GetChatElement()
{
	
		/*
.text:00267859 50                                      push    eax
.text:0026785A 8B CE                                   mov     ecx, esi
.text:0026785C E8 4F E2 6F 00                          call    sub_965AB0
.text:00267861 8B 4F 1C                                mov     ecx, [edi+1Ch]  1C -> offset
		*/
	static auto m_chat_offset = *(char*)(GameUtils::FindPattern1("client.dll",
		"50 8B CE E8 ?? ?? ?? ?? 8B 4F ?? 85 C9 74 ?? 51 E8 ?? ?? ?? ??") + 10);

		if (!m_chat_offset)
			return nullptr;

		return *(CBaseHudChat**)((char*)this + m_chat_offset);

}



