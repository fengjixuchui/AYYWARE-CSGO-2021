// Credits to Valve and Shad0w


//hellobaby
/*

what is NetworkVars?

such as 

mp/game/client/c_baseplayer.cpp/135 line
// -------------------------------------------------------------------------------- //
// RecvTable for CPlayerState.
// -------------------------------------------------------------------------------- //

	BEGIN_RECV_TABLE_NOBASE(CPlayerState, DT_PlayerState)
		RecvPropInt		(RECVINFO(deadflag)),
	END_RECV_TABLE()


//how do these code do?

namespace DT_PlayerState

{
RecvProp ret;
ret.m_pVarName = "deadflag";
ret.SetOffset( offset(deadflag,CPlayerState) );

static RecvProp RecvProps[]{dummy,ret1,ret2,ret3,};


}

you see in IDA is do it
.text:0000BCB1                 mov     dword_52E1EA4, offset aDeadflag ; "deadflag"
.text:0000BCBB                 mov     dword_52E1ED0, 4
.text:0000BCC5                 mov     dword_52E1EA8, 0
.text:0000BCCF                 mov     dword_52E1EAC, 0
.text:0000BCD9                 mov     dword_52E1EC4, offset sub_295320
.text:0000BCE3
.text:0000BCE3 loc_BCE3:                               ; CODE XREF: sub_BBD0+7��j
.text:0000BCE3                 mov     dword_4D982DC, offset dword_52E1EA4
.text:0000BCED                 mov     dword_4D982E0, 1
.text:0000BCF7                 mov     dword_4D982E4, 0
.text:0000BD01                 mov     dword_4D982E8, offset aDtPlayerstate ; "DT_PlayerState"
.text:0000BD0B                 mov     word_4D982EC, 0


So we can find these DT_xx table to find some offsets
*/





#include "NetVars.h"
#include "ClientRecvProps.h"
#include "CRC32.h"
#include "Utilities.h"

#include "SDK.h"

using namespace std;

CNetVar NetVar;

const char* AlignText(int align)
{
	static char buffer[256];
	int i = 0;
	for (i = 0; i < align; i++)
	{
		buffer[i] = ' ';
	}
	buffer[i + 1] = 0;
	return buffer;
}

void CNetVar::RetrieveClasses()
{

	ClientClass *clientClass = Interfaces::Client->GetAllClasses();

	if (!clientClass)
		return;

	//Clear netvar vector incase of another call, not necesarry as it doesnt add duplicates

	vars.clear();

	while (clientClass != 0)
	{
		if (clientClass == 0)
			break;

		LogNetVar(clientClass->m_pRecvTable, 0);

		clientClass = clientClass->m_pNext;
	}
}

void CNetVar::LogNetVar(RecvTable *table, int align)
{
	if (table->m_nProps < 0)
		return;

#ifdef DUMP_NETVARS_TO_FILE
	if (align)
		Utilities::Log("%s===%s===", AlignText(20 + align), table->m_pNetTableName);
	else
		Utilities::Log(table->m_pNetTableName);
#endif

	for (auto i = 0; i < table->m_nProps; ++i)
	{
		RecvProp *prop = &table->m_pProps[i];

		if (!prop)
			continue;

		char szCRC32[150];

		sprintf_s(szCRC32, "%s%s", table->m_pNetTableName, prop->m_pVarName);

		DWORD_PTR dwCRC32 = CRC32((void*)szCRC32, strlen(szCRC32));

#ifdef DUMP_NETVARS_TO_FILE
		Utilities::Log("%s%s [0x%X] [CRC32::0x%X]", AlignText(15 + align), prop->m_pVarName, prop->m_Offset, dwCRC32);
#endif

		//Dont add duplicates

		bool bAddNetVar = true;

		for (auto netvar = 0; netvar < (int)vars.size(); ++netvar)
		{
			netvar_info_s *netvars = &vars[netvar];

			if (netvars->dwCRC32 == dwCRC32)
				bAddNetVar = false;

#ifdef DUMP_NETVARS_TO_FILE

			if (netvars->dwCRC32 == dwCRC32 && netvars->dwOffset != prop->m_Offset) //just a test if any crc collide with another (didnt happen obviously)
			{
				Utilities::Log("^^^^ ERROR HASH %s%s::%s [0x%X] [CRC32::0x%X] ^^^^", AlignText(15 + align), table->m_pNetTableName, prop->m_pVarName, prop->m_Offset, dwCRC32);
				Utilities::Log("^^^^ CONFLICT %s%s::%s [0x%X] [CRC32::0x%X] ^^^^", AlignText(15 + align), netvars->szTableName, netvars->szPropName, netvars->dwOffset, netvars->dwCRC32);
			}
#endif
		}

		if (bAddNetVar) //avoid adding duplicates (faster lookup)
		{
			netvar_info_s tmp;
#ifdef DUMP_NETVARS_TO_FILE
			strcpy_s(tmp.szTableName, table->m_pNetTableName);
			strcpy_s(tmp.szPropName, prop->m_pVarName);
#endif
			tmp.dwCRC32 = dwCRC32;

			tmp.dwOffset = prop->m_Offset;

			vars.push_back(tmp);
		}

		if (prop->m_pDataTable)
			LogNetVar(prop->m_pDataTable, 5);
	}
}

DWORD_PTR CNetVar::GetNetVar(DWORD_PTR dwCRC32) //returns 0xFFFFFFFF (-1) if not found (ex: if(GetNetVar(0xD34DB33F)==-1) return false;
{
	for (auto i = 0; i < (int)vars.size(); ++i)
	{
		if (vars[i].dwCRC32 == dwCRC32)
			return vars[i].dwOffset;
	}

	return 0xFFFFFFFF;
}