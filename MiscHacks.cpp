// Don't take credits for this ;) Joplin / Manhhao are the first uploaders ;)

#define _CRT_SECURE_NO_WARNINGS

#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "resolver.h"

#include <time.h>

bool bIsSlowWalk = false;

template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

inline float bitsToFloat(unsigned long i)
{
	return *reinterpret_cast<float*>(&i);
}

inline float FloatNegate(float f)
{
	return bitsToFloat(FloatBits(f) ^ 0x80000000);
}

Vector AutoStrafeView;

void CMiscHacks::Init()
{
	// Any init
}

void CMiscHacks::Draw()
{
	// Any drawing	
	// Spams
	switch (Menu::Window.MiscTab.OtherChatSpam.GetIndex())
	{
	case 0:
		// No Chat Spam
		break;
	case 1:
		// Namestealer
		ChatSpamName();
		break;
	case 2:
		// Regular
		ChatSpamRegular();
		break;
	case 3:
		// Interwebz
		ChatSpamInterwebz();
		break;
	case 4:
		// Report Spam
		ChatSpamDisperseName();
		break;
	}
}

void CMiscHacks::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	// Any Move Stuff
	
	// Bhop
	switch (Menu::Window.MiscTab.OtherAutoJump.GetIndex())
	{
	case 0:
		break;
	case 1:
		AutoJump(pCmd);
		break;
	}

	// AutoStrafe
	Interfaces::Engine->GetViewAngles(AutoStrafeView);
	switch (Menu::Window.MiscTab.OtherAutoStrafe.GetIndex())
	{
	case 0:
		// Off
		break;
	case 1:
		LegitStrafe(pCmd);
		break;
	case 2:
		RageStrafe(pCmd);
		break;
	}

	//Fake Lag
	if (Menu::Window.MiscTab.FakeLagEnable.GetState())
		Fakelag(pCmd, bSendPacket);

	//SlowWalk
	int Key = Menu::Window.MiscTab.OtherSlowWalk.GetKey();
	EnterKeyJudge(Key);
	bIsSlowWalk = !bIsSlowWalk;
	EndKeyJudge;
	if (bIsSlowWalk)
		SlowWalk(pCmd);


}

static __declspec(naked) void __cdecl Invoke_NET_SetConVar(void* pfn, const char* cvar, const char* value)
{
	__asm 
	{
		push    ebp
			mov     ebp, esp
			and     esp, 0FFFFFFF8h
			sub     esp, 44h
			push    ebx
			push    esi
			push    edi
			mov     edi, cvar
			mov     esi, value
			jmp     pfn
	}
}
void DECLSPEC_NOINLINE NET_SetConVar(const char* value, const char* cvar)
{
	static DWORD setaddr = Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x8D\x4C\x24\x1C\xE8\x00\x00\x00\x00\x56", "xxxxx????x");
	if (setaddr != 0) 
	{
		void* pvSetConVar = (char*)setaddr;
		Invoke_NET_SetConVar(pvSetConVar, cvar, value);
	}
}

void change_name(const char* name)
{
	if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected())
		NET_SetConVar(name, "name");
}

void CMiscHacks::AutoJump(CUserCmd *pCmd)
{
	if (pCmd->buttons & IN_JUMP && GUI.GetKeyState(VK_SPACE))
	{
		int iFlags = hackManager.pLocal()->GetFlags();
		if (!(iFlags & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;

		if (hackManager.pLocal()->GetVelocity().Length() <= 50)
		{
			pCmd->forwardmove = 450.f;
		}
	}
}

void CMiscHacks::LegitStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (!(pLocal->GetFlags() & FL_ONGROUND))
	{
		pCmd->forwardmove = 0.0f;

		if (pCmd->mousedx < 0)
		{
			pCmd->sidemove = -450.0f;
		}
		else if (pCmd->mousedx > 0)
		{
			pCmd->sidemove = 450.0f;
		}
	}
}

void CMiscHacks::RageStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	bool bKeysPressed = true;
	if (GUI.GetKeyState(0x41) || GUI.GetKeyState(0x57) || GUI.GetKeyState(0x53) || GUI.GetKeyState(0x44)) bKeysPressed = false;

	if ((GetAsyncKeyState(VK_SPACE) && !(pLocal->GetFlags() & FL_ONGROUND)) && bKeysPressed)
	{
		if (pCmd->mousedx > 1 || pCmd->mousedx < -1) {
			pCmd->sidemove = pCmd->mousedx < 0.f ? -450.f : 450.f;
		}
		else {
			pCmd->forwardmove = (1800.f * 4.f) / pLocal->GetVelocity().Length2D();
			pCmd->sidemove = (pCmd->command_number % 2) == 0 ? -450.f : 450.f;
			if (pCmd->forwardmove > 450.f)
				pCmd->forwardmove = 450.f;
		}
	}

/*	IClientEntity* pLocal = hackManager.pLocal();
	static bool bDirection = true;


	static float move = 450; //400.f; // move = max(move, (abs(cmd->move.x) + abs(cmd->move.y)) * 0.5f);
	float s_move = move * 0.5065f;


	if ((pCmd->buttons & IN_JUMP) || !(pLocal->GetFlags() & FL_ONGROUND))
	{
		pCmd->forwardmove = move * 0.015f;
		pCmd->sidemove += (float)(((pCmd->tick_count % 2) * 2) - 1) * s_move;

		if (pCmd->mousedx)
			pCmd->sidemove = (float)clamp(pCmd->mousedx, -1, 1) * s_move;

		static float strafe = pCmd->viewangles.y;

		float rt = pCmd->viewangles.y, rotation;
		rotation = strafe - rt;

		if (rotation < FloatNegate(Interfaces::Globals->interval_per_tick))
			pCmd->sidemove = -s_move;

		if (rotation > Interfaces::Globals->interval_per_tick)
			pCmd->sidemove = s_move;

		strafe = rt;
	} */
}

Vector GetAutostrafeView()
{
	return AutoStrafeView;
}

//
void CMiscHacks::ChatSpamInterwebz()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;

	if (wasSpamming)
	{
		static bool useSpace = true;
		if (useSpace)
		{
			change_name ("[Name]");
			useSpace = !useSpace;
		}
		else
		{
			change_name("[Name]");
			useSpace = !useSpace;
		}
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamDisperseName()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;

	if (wasSpamming)
	{
		change_name("\n…e…e…e\n");
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamName()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player
		if (entity && hackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != hackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			// If entity is a player
			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}
		}
	}

	static bool wasSpamming = true;

	if(!Names.size())
		return;
	int randomIndex = rand() % Names.size();
	char buffer[128];
	sprintf_s(buffer, "%s ", Names[randomIndex].c_str());

	if (wasSpamming)
	{
		change_name(buffer);
	}
	else
	{
		change_name ("p$i 1337");
	}

	start_t = clock();
}

void CMiscHacks::ChatSpamRegular()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	//SpamDelay
	int spamtime = Menu::Window.MiscTab.OtherChatDelay.GetValue();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < spamtime)
		return;

	static bool holzed = true;

	if (Menu::Window.MiscTab.OtherTeamChat.GetState())
	{
		SayInTeamChat("https://github.com/helloobaby/AYYWARE-CSGO-2021");
	}
	else
	{
		SayInChat("https://github.com/helloobaby/AYYWARE-CSGO-2021");
	}

	start_t = clock();
}

void CMiscHacks::Fakelag(CUserCmd *pCmd, bool &bSendPacket)
{
	int iChoke = Menu::Window.MiscTab.FakeLagChoke.GetValue();

	static int iFakeLag = -1;
	iFakeLag++;

	if (iFakeLag <= iChoke && iFakeLag > -1)
	{
		bSendPacket = false;
	}
	else
	{
		bSendPacket = true;
		iFakeLag = -1;
	}
}

//https://www.unknowncheats.me/forum/cs-go-releases/315876-slow-walk.html
//i think player can press ctrl himself,so i dont want to pCmd->buttons | IN_IN_DUCK
void CMiscHacks::SlowWalk(CUserCmd* pCmd)
{
	auto LocalPlayer = hackManager.pLocal();

	if(!LocalPlayer)
		return ;

	if (!Interfaces::Engine->IsInGame())
		return ;

	//if hold a knife or no weapon
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(LocalPlayer
		->GetActiveWeaponHandle());
	CSWeaponInfo* pWeaponInfo = nullptr;
	if (pWeapon)
	{
		pWeaponInfo = pWeapon->GetCSWpnData();
		if (!GameUtils::IsBallisticWeapon(pWeapon))
		return;
	}
	else
		return;

	//alreay press shift
	if(pCmd->buttons & IN_WALK)
		return;

	const float MaxSpeed =  (LocalPlayer->IsScoped() ? pWeaponInfo->maxSpeedAlt : pWeaponInfo->maxSpeed) / 3;

//just copy paste from Osiris
#define M_SQRT1_2  0.707106781186547524401  // 1/sqrt(2)
	if (pCmd->forwardmove && pCmd->sidemove) {
		const float maxSpeedRoot = MaxSpeed * static_cast<float>(M_SQRT1_2);
		pCmd->forwardmove = pCmd->forwardmove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
		pCmd->sidemove = pCmd->sidemove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
	}
	else if (pCmd->forwardmove) {
		pCmd->forwardmove = pCmd->forwardmove < 0.0f ? -MaxSpeed : MaxSpeed;
	}
	else if (pCmd->sidemove) {
		pCmd->sidemove = pCmd->sidemove < 0.0f ? -MaxSpeed : MaxSpeed;
	}

}

float CGlobalVarsBase::serverTime(UserCmd* cmd) noexcept{

	static int tick;
	static UserCmd* lastCmd;

	auto localPlayer = hackManager.pLocal();

	if (cmd) {
		if (localPlayer && (!lastCmd || lastCmd->hasbeenpredicted))
			tick = localPlayer->GetTickBase();
		else
			tick++;
		lastCmd = cmd;
	}
	return tick * intervalPerTick;

}