#pragma once

#include "Interfaces.h"
#include "Hacks.h"

namespace R
{
	void Resolve();
};

#define key_press_interval \
static float time1 = 0;\
float time2 = Interfaces::Globals->realtime;\
if (time2 - time1 < 0.2f)\
	break;\
time1 = time2;


//Macro example see slowwalk
#define EnterKeyJudge(Key)\
	bool KeyState = GUI.GetKeyState(Key);\
	static auto OriKeyState = false;\
	do {\
		if (OriKeyState == KeyState)\
			break;\
		if (Key >= 0 && KeyState) {\

#define EndKeyJudge \
		}\
	OriKeyState = KeyState;\
	} while (0);


namespace Globals
{
	extern CUserCmd* UserCmd;
	extern IClientEntity* Target;
	extern int Shots;
	extern bool change;
	extern int TargetID;
	extern Vector g_vFakeAngle;

}