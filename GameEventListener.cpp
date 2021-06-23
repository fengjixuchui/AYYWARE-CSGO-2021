#include "GameEventListener.h"
#include "MiscDefinitions.h"
#include "ChatLog.h"
#include "Hacks.h"
#include "Entities.h"

#include <string>

extern HackManager hackManager;


//////////////////////////////////////
//listener cpp file
////////////////////////////////

namespace Interfaces{

	IGameEventManager2* GameEventManager = nullptr;

}


bool IGameEventManager2::AddListener(IGameEventListener2* listener, std::string name, bool serverSide)
{
	typedef bool(__thiscall* OriginalFn)(void*, IGameEventListener2*, const char*, bool);
	return GetVFunc<OriginalFn>(this, 3)(this, listener, name.c_str(), serverSide);
}

void IGameEventManager2::RemoveListener(IGameEventListener2* listener)
{
	typedef void(__thiscall* OriginalFn)(void*, IGameEventListener2*);
	GetVFunc<OriginalFn>(this, 5)(this, listener);
}

bool IGameEventManager2::FireEventClientSide(CGameEvent* gameEvent)
{
	typedef bool(__thiscall* OriginalFn)(void*, CGameEvent*);
	return GetVFunc<OriginalFn>(this, 8)(this, gameEvent);
}

CGameEventListener::CGameEventListener(const char* eventName, EventFunction gameEventFunction, bool serverSide)
{
	this->eventName = (char*)eventName;
	this->gameEventFunction = gameEventFunction;
	Interfaces::GameEventManager->AddListener(this, this->eventName, serverSide);
}


constexpr auto hash_s1 = shash::compute_hash("player_hurt");
void GameEvent_PlayerHurt(CGameEvent* gameEvent)
{
	//string cant behide switch lable,so use hash
	/*switch (shash::compute_hash(gameEvent->GetName()))
	{
	case hash_s1:
		ChatLog("fire game event");
		break;
	}*/

	auto LocalPlayer = hackManager.pLocal();
	if (!LocalPlayer->IsAlive() || !gameEvent || !Interfaces::Engine->IsConnected())
		return;
	
	GameEvent_PlayerHurt_t ev;

	ev.targetuserid = gameEvent->GetInt(("userid"));
	ev.attackeruserid = gameEvent->GetInt(("attacker"));
	ev.hitgroup = gameEvent->GetInt(("hitgroup"), -1);
	ev.dmg_health = gameEvent->GetInt(("dmg_health"), -1);
	ev.health = gameEvent->GetInt(("health"), -1);
	ev.weapon = gameEvent->GetString(("weapon"), "");

				//should be i < cl.m_nMaxClients
	//for (int i = 0; i < 64; i++) {
	//	player_info_t pi;
	//	(player_info_t*)Interfaces::Engine->GetPlayerInfo(i,&pi);

	//	if (pi.) continue;

	//	// Fixup from network order (little endian)
	//	if ((pi->userId) == userID) {
	//		// return as entity index
	//		return (i + 1);
	//	}
	//}

	const int user_id = gameEvent->GetInt(("userid"));
	const int attacker_id = gameEvent->GetInt(("attacker"));

	const int killed = Interfaces::Engine->GetPlayerForUserID(user_id);
	const int killer = Interfaces::Engine->GetPlayerForUserID(attacker_id);

	if(killer != LocalPlayer->GetIndex())
		return;

	IClientEntity* killed_entity = Interfaces::EntList->GetClientEntity(killed);
	IClientEntity* killer_entity = Interfaces::EntList->GetClientEntity(killer);
	player_info_t killed_entity_info;
	player_info_t killer_entity_info;
	Interfaces::Engine->GetPlayerInfo(killed, &killed_entity_info);
	Interfaces::Engine->GetPlayerInfo(killed, &killer_entity_info);

	//"Hit Name xx hitgroup xx HP , Left xx HP"
	char logBuffer[64] = {0};
	char hittedName[64] = {0};
	memcpy_s(hittedName,sizeof(hittedName) ,&killed_entity_info.name,64);
	sprintf_s(logBuffer,sizeof(logBuffer),
		"Hit  \x01%s \x02%s  \x03%d  HP , Left \x04%d HP",
		hittedName,HitGroupToString(ev.hitgroup),ev.dmg_health,ev.health);
	ChatLog(logBuffer);

}