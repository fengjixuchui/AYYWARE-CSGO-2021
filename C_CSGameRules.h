#include"NetVars.h"

class CGameRules
{
public:

	//m_bIsValveDS [0x7C] [CRC32::0x10123ABB]
	bool IsValveServer(){
		return *(bool*)((DWORD)this+NetVar.GetNetVar(0x10123ABB));
	}




};