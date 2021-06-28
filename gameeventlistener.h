/// listener header file
#pragma once
#include <string>
#include <Windows.h>
#include "Interfaces.h"
#include "RenderBeams.h"

class IGameEventVisitor2;
class CGameEventDescriptor;
class IGameEventVisitor2;

namespace Interfaces {

	extern IGameEventManager2* GameEventManager;

}

struct GameEvent_PlayerHurt_t
{
	int targetuserid;
	int attackeruserid;
	int hitgroup;
	int dmg_health;
	int health;
	std::string weapon;
};

class CGameEvent
{
public:
	virtual ~CGameEvent();
	virtual const char* GetName(void)const;//index 1
	virtual bool  IsReliable(void)const;
	virtual bool  IsLocal(void)const;
	virtual bool  IsEmpty(const char *keyName = NULL)const;
	virtual bool  GetBool(const char *keyName = NULL, bool defaultValue = false)const;
	virtual int  GetInt(const char *keyName = NULL, int defaultValue = 0)const;
	virtual unsigned long long  GetUint64(const char *keyName = NULL, unsigned long long defaultValue = 0)const;
	virtual float GetFloat(const char *keyName = NULL, float defaultValue = 0.0f)const;
	virtual const char* GetString(const char *keyName = NULL, const char *defaultValue = "")const;
	virtual const wchar_t* GetWString(const char *keyName = NULL, const wchar_t *defaultValue = L"")const;
	virtual void* GetPtr(const char *keyName = NULL)const;
	virtual void SetBool(const char *keyName, bool value);
	virtual void SetInt(const char *keyName, int value);
	virtual void SetUint64(char const*keyName, unsigned long long value);
	virtual void SetFloat(const char *keyName, float value);
	virtual void SetString(const char *keyName, char const* value);
	virtual void SetWString(const char *keyName, wchar_t const* value);
	virtual void SetPtr(const char *keyName, void const* value);
	virtual bool ForEventData(IGameEventVisitor2 * value)const;

	CGameEventDescriptor	*m_pDescriptor;
	KeyValues				*m_pDataKeys;
};

class IGameEventListener2
{
public:
	virtual ~IGameEventListener2(void) {}

	virtual void FireGameEvent(CGameEvent*) = 0;

	virtual int IndicateEventHandling(void) {
		return 0x2A;
	}
};

class IGameEventManager2
{
public:
	bool AddListener(IGameEventListener2* listener, std::string name, bool serverSide);
	void RemoveListener(IGameEventListener2* listener);
	bool FireEventClientSide(CGameEvent* gameEvent);
};


typedef void(*EventFunction)(CGameEvent*);
class CGameEventListener : public IGameEventListener2
{
private:
	char* eventName;
	EventFunction gameEventFunction;
public:
	CGameEventListener(const char* eventName, EventFunction gameEventFunction, bool serverSide);

	
	virtual void FireGameEvent(CGameEvent* gameEvent)
	{
		//if (LevelIsLoaded && LocalPlayer && LocalPlayer.Entity->GetAlive() && Interfaces::EngineClient->IsInGame())
		{
			this->gameEventFunction(gameEvent);
		}
	}
};

void GameEvent_PlayerHurt(CGameEvent* gameEvent);
void GameEvent_BulletImpact(CGameEvent* gameEvent);

inline char *HitGroupToString(int hitgroup)
{
#define    HITGROUP_GENERIC    0
#define    HITGROUP_HEAD        1
#define    HITGROUP_CHEST        2
#define    HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_GEAR        10
	switch (hitgroup)
	{
	case 0:
		return "GENERIC";
	case 1:
		return "HEAD";
	case 2:
		return "CHEST";
	case 3:
		return "STOMACH";
	case 4:
		return "LEFTARM";
	case 5:
		return "RIGHTARM";
	case 6:
		return "LEFTLEG";
	case 7:
		return "RIGHTLEG";
	case 10:
		return "GEAR";
	default:
		return "unrecognized";
		break;
	}

}
