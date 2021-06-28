#include "GameEventListener.h"
#include "MiscDefinitions.h"
#include "ChatLog.h"
#include "Hacks.h"
#include "Entities.h"
#include "Menu.h"

#include <string>

//all game events in src engine
//https://wiki.alliedmods.net/Counter-Strike:_Global_Offensive_Events

extern HackManager hackManager;

namespace menu{
extern AyyWareWindow Window;
}

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
	if (!LocalPlayer || !gameEvent || !Interfaces::Engine->IsConnected())
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

	if (!Menu::Window.MiscTab.FireEvent.GetState() || (killer != LocalPlayer->GetIndex())) {
			return;

	IClientEntity* killed_entity = Interfaces::EntList->GetClientEntity(killed);
	IClientEntity* killer_entity = Interfaces::EntList->GetClientEntity(killer);
	player_info_t killed_entity_info;
	player_info_t killer_entity_info;
	Interfaces::Engine->GetPlayerInfo(killed, &killed_entity_info);
	Interfaces::Engine->GetPlayerInfo(killed, &killer_entity_info);


	//"Hit Name xx hitgroup xx HP , Left xx HP"
	char logBuffer[128] = {0};
	char hittedName[64] = {0};
	memcpy(hittedName,&killed_entity_info.name,64);
	sprintf(logBuffer,
		"Hit  \x01%s \x02%s  \x03%d  HP , Left \x04%d HP",
		hittedName,HitGroupToString(ev.hitgroup),ev.dmg_health,ev.health);
	ChatLog(logBuffer);
	}


}

struct GameEvent_Impact_t
{
	Vector impactpos;
	int attackeruserid;
};
void GameEvent_BulletImpact(CGameEvent* gameEvent)
{
	if(!Menu::Window.MiscTab.FireBulletTrace.GetState())
		return;

	auto LocalPlayer = hackManager.pLocal();
	if (!LocalPlayer || !gameEvent || !Interfaces::Engine->IsConnected())
		return;



	GameEvent_Impact_t ev;

	ev.impactpos.x = gameEvent->GetFloat(("x"));
	ev.impactpos.y = gameEvent->GetFloat(("y"));
	ev.impactpos.z = gameEvent->GetFloat(("z"));
	ev.attackeruserid = gameEvent->GetInt(("userid"));

	const int killer = Interfaces::Engine->GetPlayerForUserID(ev.attackeruserid);
	IClientEntity* killer_entity = Interfaces::EntList->GetClientEntity(killer);
	
	auto viewAngles = killer_entity->GetEyeAngles();
	Vector forward = {0,0,0};
	AngleVectors(viewAngles,&forward);
	int lenth = 500;
	auto src = forward*lenth;

	DrawBeamd(killer_entity->GetEyePosition(),src, Color(128,42,42, 255));
}