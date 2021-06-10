
#include "Dumping.h"


void Dump::DumpClassIds()
{

	ClientClass* cClass = Interfaces::Client->GetAllClasses();
	while (cClass)
	{
		Utilities::Log("%s = %d,", cClass->m_pNetworkName, cClass->m_ClassID);
		cClass = cClass->m_pNext;
	}
}