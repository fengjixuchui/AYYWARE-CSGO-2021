#pragma once

#include "Interfaces.h"
#include "Hacks.h"

namespace R
{
	void Resolve();
};

#define gamecurtime Interfaces::Globals->currenttime

#define TICK_INTERVAL			(Interfaces::Globals->intervalPerTick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		(-1)


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

	extern int m_nTickbaseShift;

}