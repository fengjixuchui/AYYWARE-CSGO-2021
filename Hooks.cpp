// Don't take credits for this ;) Joplin / Manhhao are the first uploaders ;)

#include "Hooks.h"
#include "Hacks.h"
#include "Chams.h"
#include "Menu.h"

#include "Interfaces.h"
#include "RenderManager.h"
#include "MiscHacks.h"
#include "CRC32.h"
#include "Resolver.h"
#include <intrin.h>
#include "C_CSGameRules.h"
#include "MiscClasses.h"

Vector LastAngleAA;

enum class WeaponId : short {
	Deagle = 1,
	Elite,
	Fiveseven,
	Glock,
	Ak47 = 7,
	Aug,
	Awp,
	Famas,
	G3SG1,
	GalilAr = 13,
	M249,
	M4A1 = 16,
	Mac10,
	P90 = 19,
	ZoneRepulsor,
	Mp5sd = 23,
	Ump45,
	Xm1014,
	Bizon,
	Mag7,
	Negev,
	Sawedoff,
	Tec9,
	Taser,
	Hkp2000,
	Mp7,
	Mp9,
	Nova,
	P250,
	Shield,
	Scar20,
	Sg553,
	Ssg08,
	GoldenKnife,
	Knife,
	Flashbang = 43,
	HeGrenade,
	SmokeGrenade,
	Molotov,
	Decoy,
	IncGrenade,
	C4,
	Healthshot = 57,
	KnifeT = 59,
	M4a1_s,
	Usp_s,
	Cz75a = 63,
	Revolver,
	TaGrenade = 68,
	Axe = 75,
	Hammer,
	Spanner = 78,
	GhostKnife = 80,
	Firebomb,
	Diversion,
	FragGrenade,
	Snowball,
	BumpMine,
	Bayonet = 500,
	ClassicKnife = 503,
	Flip = 505,
	Gut,
	Karambit,
	M9Bayonet,
	Huntsman,
	Falchion = 512,
	Bowie = 514,
	Butterfly,
	Daggers,
	Paracord,
	SurvivalKnife,
	Ursus = 519,
	Navaja,
	NomadKnife,
	Stiletto = 522,
	Talon,
	SkeletonKnife = 525,
	GloveStuddedBrokenfang = 4725,
	GloveStuddedBloodhound = 5027,
	GloveT,
	GloveCT,
	GloveSporty,
	GloveSlick,
	GloveLeatherWrap,
	GloveMotorcycle,
	GloveSpecialist,
	GloveHydra
};


namespace ClientStates{

extern DWORD offset_m_nChokedCommands;
extern DWORD offset_m_nLastOutgoingCommand;

}
// Funtion Typedefs
typedef void(__thiscall* DrawModelEx_)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);
typedef void(__thiscall* PaintTraverse_)(PVOID, unsigned int, bool, bool);
typedef bool(__thiscall* InPrediction_)(PVOID);
typedef void(__stdcall *FrameStageNotifyFn)(ClientFrameStage_t);
typedef void(__thiscall* RenderViewFn)(void*, CViewSetup&, CViewSetup&, int, int);
typedef bool (__fastcall* WriteUsercmdDeltaToBufferFn)(void* ecx, void*, int slot, bf_write* buf, int from, int to, bool isnewcommand);

using OverrideViewFn = void(__fastcall*)(void*, void*, CViewSetup*);
typedef float(__stdcall *oGetViewModelFOV)();

typedef void (__stdcall *WriteUsercmdFn)(bf_write* buf, CInput::CUserCmd* to, CInput::CUserCmd* from);

// Function Pointers to the originals
PaintTraverse_ oPaintTraverse;
DrawModelEx_ oDrawModelExecute;
FrameStageNotifyFn oFrameStageNotify;
OverrideViewFn oOverrideView;
RenderViewFn oRenderView;
WriteUsercmdDeltaToBufferFn oWriteUsercmdDeltaToBuffer;


// Hook function prototypes
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
bool __stdcall Hooked_InPrediction();
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
bool __stdcall CreateMoveClient_Hooked(/*void* self, int edx,*/ float frametime, CUserCmd* pCmd);
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup);
float __stdcall GGetViewModelFOV();
void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);

// VMT Managers
namespace Hooks
{
	// VMT Managers
	Utilities::Memory::VMTManager VMTPanel; // Hooking drawing functions
	Utilities::Memory::VMTManager VMTClient; // Maybe CreateMove
	Utilities::Memory::VMTManager VMTClientMode; // CreateMove for functionality
	Utilities::Memory::VMTManager VMTModelRender; // DrawModelEx for chams
	Utilities::Memory::VMTManager VMTPrediction; // InPrediction for no vis recoil
	Utilities::Memory::VMTManager VMTPlaySound; // Autoaccept 
	Utilities::Memory::VMTManager VMTRenderView;
};


bool bIsThirdPerson = false;


namespace Interfaces{
extern uintptr_t gpClientState;
}

extern CGameRules* g_pGameRules;



//fix: No Need To Unload,dont call this function
void Hooks::UndoHooks()
{
	VMTPanel.RestoreOriginal();
	VMTPrediction.RestoreOriginal();
	VMTModelRender.RestoreOriginal();
	VMTClientMode.RestoreOriginal();
}


// Initialise all our hooks
void Hooks::Initialise()
{
	// Panel hooks for drawing to the screen via surface functions
	VMTPanel.Initialise((DWORD*)Interfaces::Panels);
	oPaintTraverse = (PaintTraverse_)VMTPanel.HookMethod((DWORD)&PaintTraverse_Hooked, Offsets::VMT::Panel_PaintTraverse);

	// No Visual Recoi	l
	VMTPrediction.Initialise((DWORD*)Interfaces::Prediction);
	VMTPrediction.HookMethod((DWORD)&Hooked_InPrediction, 14);

	// Chams
	VMTModelRender.Initialise((DWORD*)Interfaces::ModelRender);
	oDrawModelExecute = (DrawModelEx_)VMTModelRender.HookMethod((DWORD)&Hooked_DrawModelExecute, Offsets::VMT::ModelRender_DrawModelExecute);

//C:\Users\sbb\Desktop\source-sdk-2013-master\mp\src\game\client\clientmode_shared.h
	// Setup ClientMode Hooks
	VMTClientMode.Initialise((DWORD*)Interfaces::ClientMode);
	VMTClientMode.HookMethod((DWORD)CreateMoveClient_Hooked, 24);

	oOverrideView = (OverrideViewFn)VMTClientMode.HookMethod((DWORD)&Hooked_OverrideView, 18);
	VMTClientMode.HookMethod((DWORD)&GGetViewModelFOV, 35);

	// Setup client hooks
	VMTClient.Initialise((DWORD*)Interfaces::Client);
	oFrameStageNotify = (FrameStageNotifyFn)VMTClient.HookMethod((DWORD)&Hooked_FrameStageNotify, 37);
	oWriteUsercmdDeltaToBuffer = (WriteUsercmdDeltaToBufferFn)VMTClient.HookMethod((DWORD)&Hooked_WriteUsercmdDeltaToBuffer,24);



}

void MovementCorrection(CUserCmd* pCmd)
{

}

//---------------------------------------------------------------------------------------------------------
//                                         Hooked Functions
//---------------------------------------------------------------------------------------------------------

void SetClanTag(const char* tag, const char* name)//190% paste
{
	static auto pSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(((DWORD)Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx")));
	pSetClanTag(tag, name);
}
void NoClantag()
{
	SetClanTag("", "");
}

void ClanTag()
{
	static int counter = 0;
	switch (Menu::Window.MiscTab.OtherClantag.GetIndex())
	{
	case 0:
		// No 
		break;
	case 1:
	{
		static int motion = 0;
		int ServerTime = (float)Interfaces::Globals->intervalPerTick * hackManager.pLocal()->GetTickBase() * 2.5;

		if (counter % 48 == 0)
			motion++;
		int value = ServerTime % 20;
		switch (value) {
		case 0:SetClanTag("         A", "alwayslose"); break;
		case 1:SetClanTag("        Al", "alwayslose"); break;
		case 2:SetClanTag("       Alw", "alwayslose"); break;
		case 3:SetClanTag("      Alwa", "alwayslose"); break;
		case 4:SetClanTag("     Alway", "alwayslose"); break;
		case 5:SetClanTag("    Always", "alwayslose"); break;
		case 6:SetClanTag("   Alwaysl", "alwayslose"); break;
		case 7:SetClanTag("  Alwayswa", "alwayslose"); break;
		case 8:SetClanTag(" Alwayslos", "alwayslose"); break;
		case 9:SetClanTag("Alwayslose.cc", "alwayslose"); break;
		case 10:SetClanTag("Alwayslose.cc ", "alwayslose"); break;
		case 11:SetClanTag("lwayslose.cc ", "alwayslose"); break;
		case 12:SetClanTag("wayslose.cc  ", "alwayslose"); break;
		case 13:SetClanTag("ayslose.cc   ", "alwayslose"); break;
		case 14:SetClanTag("yslose.cc    ", "alwayslose"); break;
		case 15:SetClanTag("slose.cc     ", "alwayslose"); break;
		case 16:SetClanTag("lose.cc      ", "alwayslose"); break;
		case 17:SetClanTag("ose.cc       ", "alwayslose"); break;
		case 18:SetClanTag("se.cc        ", "alwayslose"); break;
		case 19:SetClanTag("s.cc         ", "alwayslose"); break;
		case 20:SetClanTag("          ", "alwayslose");break;
		}
		counter++;
	}
	break;
	case 2:
	{
		//add 'u8'Prefix to support unicode characters
		SetClanTag(u8"Å£±Æ666","alwayslose");
		break;
	}
	break;
	case 3:
		break;
	case 4:
		break;
	}
}

//https://www.unknowncheats.me/forum/1453101-post8.html
//https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/clientmode_shared.cpp#L408
bool __stdcall CreateMoveClient_Hooked(float frametime, CUserCmd* pCmd)
{

	if (!pCmd->command_number)
		return true;

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{

		PVOID pebp;
		__asm mov pebp, ebp;
		bool* pbSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);
		bool& bSendPacket = *pbSendPacket;

		if (Menu::Window.MiscTab.OtherClantag.GetIndex() > 0)
			ClanTag();


			// Backup for safety
		Vector origView = pCmd->viewangles;
		Vector viewforward, viewright, viewup, aimforward, aimright, aimup;
		Vector qAimAngles;
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);

		// Do da hacks
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
			Hacks::MoveHacks(pCmd, bSendPacket);

		Globals::Tick::run(pCmd,bSendPacket);

		//---------------------------------------------------------------------------------------------

		//Movement Fix
		//GameUtils::CL_FixMove(pCmd, origView);
		qAimAngles.Init(0.0f, GetAutostrafeView().y, 0.0f); // if pCmd->viewangles.x > 89, set pCmd->viewangles.x instead of 0.0f on first
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &aimforward, &aimright, &aimup);
		Vector vForwardNorm;		Normalize(viewforward, vForwardNorm);
		Vector vRightNorm;			Normalize(viewright, vRightNorm);
		Vector vUpNorm;				Normalize(viewup, vUpNorm);

		// Original shit for movement correction
		float forward = pCmd->forwardmove;
		float right = pCmd->sidemove;
		float up = pCmd->upmove;
		if (forward > 450) forward = 450;
		if (right > 450) right = 450;
		if (up > 450) up = 450;
		if (forward < -450) forward = -450;
		if (right < -450) right = -450;
		if (up < -450) up = -450;
		pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
		pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
		pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);

		//clamp angles
		GameUtils::NormaliseViewAngle(pCmd->viewangles);

		if (bSendPacket)
			LastAngleAA = pCmd->viewangles;




	}

	return false;
}


// Paint Traverse Hooked function
void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;

	if (!FoundPanel)
	{
		PCHAR szPanelName = (PCHAR)Interfaces::Panels->GetName(vguiPanel);
		if (strstr(szPanelName, "MatSystemTopPanel"))
		{
			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}
	}
	else if (FocusOverlayPanel == vguiPanel)
	{
		Render::Text(10, 10, Color(255, 255, 255, 220), Render::Fonts::Menu, "CHEAT[ON]");
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
			Hacks::DrawHacks();

		//Draw Cheat Status
		Menu::UICheatStatus();

		// Update and draw the menu
		Menu::DoUIFrame();
	}
	Interfaces::Panels->SetMouseInputEnabled(vguiPanel, m_bIsOpen);
	//window->m_bIsOpen
}

// InPrediction Hooked Function
//
bool __stdcall Hooked_InPrediction()
{
	IClientEntity* oldEsi = nullptr;
	float* oldEbx;

	_asm mov oldEsi,esi
	_asm mov oldEbx,ebx

	if(oldEsi != hackManager.pLocal())
		;//DebugBreak();

	bool result;
	//mov     eax, g_Prediction_vtable
	//mov     eax, [eax+38h]  38h/4 = 14
	static InPrediction_ origFunc = (InPrediction_)Hooks::VMTPrediction.GetOriginalFunction(14);
	static DWORD *ecxVal = Interfaces::Prediction;
	result = origFunc(ecxVal);


	// If we are in the right place where the player view is calculated
	// Calculate the change in the view and get rid of it
	if (Menu::Window.VisualsTab.OtherNoVisualRecoil.GetState() && (DWORD)(_ReturnAddress()) == Offsets::Functions::dwCalcPlayerView)//C_BasePlayer::CalcPlayerView
	{
		IClientEntity* pLocalEntity = oldEsi;

		float* m_LocalViewAngles = oldEbx;

		/*
47E7D31D 8B 0D 5C 6F EE 47    mov         ecx,dword ptr [_tls_index (47EE6F5Ch)]
47E7D323 8B 34 88             mov         esi,dword ptr [eax+ecx*4]
		
		*/



		//CNetworkVarEmbedded( CPlayerLocalData, m_Local );

		Vector viewPunch = pLocalEntity->localPlayerExclusive()->GetViewPunchAngle();
		Vector aimPunch = pLocalEntity->localPlayerExclusive()->GetAimPunchAngle();

		m_LocalViewAngles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		m_LocalViewAngles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		m_LocalViewAngles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		return true;
	}

	return result;
}

// DrawModelExec for chams and shit
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	Color color;
	float flColor[3] = { 0.f };
	static IMaterial* CoveredLit = CreateMaterial(true);
	static IMaterial* OpenLit = CreateMaterial(false);
	static IMaterial* CoveredFlat = CreateMaterial(true, false);
	static IMaterial* OpenFlat = CreateMaterial(false, false);
	bool DontDraw = false;

	const char* ModelName = Interfaces::ModelInfo->GetModelName((model_t*)pInfo.pModel);
	IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Menu::Window.VisualsTab.Active.GetState())
	{
		// Player Chams
		int ChamsStyle = Menu::Window.VisualsTab.OptionsChams.GetIndex();
		int HandsStyle = Menu::Window.VisualsTab.OtherNoHands.GetIndex();
		if (ChamsStyle != 0 && Menu::Window.VisualsTab.FiltersPlayers.GetState() && strstr(ModelName, "models/player"))
		{
			if (pLocal/* && (!Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() || pModelEntity->GetTeamNum() != pLocal->GetTeamNum())*/)
			{
				IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
				IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;

				IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
				if (pModelEntity)
				{
					IClientEntity *local = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
					if (local)
					{
						if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0 /*&& pModelEntity->GetTeamNum() != local->GetTeamNum()*/)
						{
							float alpha = 1.f;

							if (pModelEntity->HasGunGameImmunity())
								alpha = 0.5f;

							if (pModelEntity->GetTeamNum() == 2)
							{
								flColor[0] = 240.f / 255.f;
								flColor[1] = 30.f / 255.f;
								flColor[2] = 35.f / 255.f;
							}
							else
							{
								flColor[0] = 63.f / 255.f;
								flColor[1] = 72.f / 255.f;
								flColor[2] = 205.f / 255.f;
							}

							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(covered);
							oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

							if (pModelEntity->GetTeamNum() == 2)
							{
								flColor[0] = 247.f / 255.f;
								flColor[1] = 180.f / 255.f;
								flColor[2] = 20.f / 255.f;
							}
							else
							{
								flColor[0] = 32.f / 255.f;
								flColor[1] = 180.f / 255.f;
								flColor[2] = 57.f / 255.f;
							}

							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(open);
						}
						else
						{
							color.SetColor(255, 255, 255, 255);
							ForceMaterial(color, open);
						}
					}
				}
			}
		}
		else if (HandsStyle != 0 && strstr(ModelName, "arms"))
		{
			if (HandsStyle == 1)
			{
				DontDraw = true;
			}
			else if (HandsStyle == 2)
			{
				Interfaces::RenderView->SetBlend(0.3);
			}
			else if (HandsStyle == 3)
			{
				IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
				IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;
				if (pLocal)
				{
					if (pLocal->IsAlive())
					{
						int alpha = pLocal->HasGunGameImmunity() ? 150 : 255;

						if (pLocal->GetTeamNum() == 2)
							color.SetColor(240, 30, 35, alpha);
						else
							color.SetColor(63, 72, 205, alpha);

						ForceMaterial(color, covered);
						oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

						if (pLocal->GetTeamNum() == 2)
							color.SetColor(247, 180, 20, alpha);
						else
							color.SetColor(32, 180, 57, alpha);
					}
					else
					{
						color.SetColor(255, 255, 255, 255);
					}

					ForceMaterial(color, open);
				}
			}
			else
			{
				static int counter = 0;
				static float colors[3] = { 1.f, 0.f, 0.f };

				if (colors[counter] >= 1.0f)
				{
					colors[counter] = 1.0f;
					counter += 1;
					if (counter > 2)
						counter = 0;
				}
				else
				{
					int prev = counter - 1;
					if (prev < 0) prev = 2;
					colors[prev] -= 0.05f;
					colors[counter] += 0.05f;
				}

				Interfaces::RenderView->SetColorModulation(colors);
				Interfaces::RenderView->SetBlend(0.3);
				Interfaces::ModelRender->ForcedMaterialOverride(OpenLit);
			}
		}
		else if (ChamsStyle != 0 && Menu::Window.VisualsTab.FiltersWeapons.GetState() && strstr(ModelName, "_dropped.mdl"))
		{
			IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
			color.SetColor(255, 255, 255, 255);
			ForceMaterial(color, covered);
		}
	}

	if (!DontDraw)
		oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	Interfaces::ModelRender->ForcedMaterialOverride(NULL);
}


// Hooked FrameStageNotify for removing visual recoil
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	DWORD eyeangles = NetVar.GetNetVar(0xBFEA4E7B);
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_RENDER_START)
	{
			//to see fake-ange
		if (pLocal->IsAlive())
		{	
			if (*(bool*)((DWORD)Interfaces::pInput + 0xAD))//A5->AD 
				*(Vector*)((DWORD)pLocal + 0x31D8) = LastAngleAA;//31C8->31D8
		}

		int Key = Menu::Window.MiscTab.OtherThirdperson.GetKey();

		EnterKeyJudge(Key);
		bIsThirdPerson = !bIsThirdPerson;
		EndKeyJudge;

		if ((KeyState && (Key >=0 )))
		{
			static bool rekt = false;
			if (!rekt)
			{
				ConVar* sv_cheats = Interfaces::CVar->FindVar("sv_cheats");
				SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
				sv_cheats_spoofed->SetInt(1);
				rekt = true;
			}
		}

		if(pLocal->IsAlive())
		{
			if(bIsThirdPerson)
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
			else
				Interfaces::Engine->ClientCmd_Unrestricted("firstperson");
		}

	}

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		if (Menu::Window.MiscTab.KnifeEnable.GetState() && pLocal)
		{
			IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
			CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)WeaponEnt;

			int iBayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
			int iButterfly = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
			int iFlip = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
			int iGut = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
			int iKarambit = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
			int iM9Bayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
			int iHuntsman = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
			int iFalchion = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
			int iDagger = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
			int iBowie = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");

			int Model = Menu::Window.MiscTab.KnifeModel.GetIndex();
			int Skin = Menu::Window.MiscTab.KnifeSkin.GetIndex();

			if (Weapon)
			{
				if (WeaponEnt->GetClientClass()->m_ClassID == (int)CSGOClassID::CKnife)
				{
					if (Model == 0) // Karambit
					{
						*Weapon->ModelIndex() = iKarambit; // m_nModelIndex
						*Weapon->ViewModelIndex() = iKarambit;
						*Weapon->WorldModelIndex() = iKarambit + 1;
						*Weapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 507;

						if (Skin == 0)
							*Weapon->FallbackPaintKit() = 416; // Doppler Sapphire
						else if (Skin == 1)
							*Weapon->FallbackPaintKit() = 415; // Doppler Ruby
						else if (Skin == 2)
							*Weapon->FallbackPaintKit() = 409; // Tiger Tooth
						else if (Skin == 3)
							*Weapon->FallbackPaintKit() = 558; // Lore
					}
					else if (Model == 1) // Bayonet
					{
						*Weapon->ModelIndex() = iBayonet; // m_nModelIndex
						*Weapon->ViewModelIndex() = iBayonet;
						*Weapon->WorldModelIndex() = iBayonet + 1;
						*Weapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 500;

						if (Skin == 0)
							*Weapon->FallbackPaintKit() = 416; // Doppler Sapphire
						else if (Skin == 1)
							*Weapon->FallbackPaintKit() = 415; // Doppler Ruby
						else if (Skin == 2)
							*Weapon->FallbackPaintKit() = 409; // Tiger Tooth
						else if (Skin == 3)
							*Weapon->FallbackPaintKit() = 558; // Lore
					}
					else if (Model == 2) // butter
					{
						*Weapon->ModelIndex() = iButterfly; // m_nModelIndex
						*Weapon->ViewModelIndex() = iButterfly;
						*Weapon->WorldModelIndex() = iButterfly + 1;
						*Weapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() = 515;

						if (Skin == 0)
							*Weapon->FallbackPaintKit() = 416; // Doppler Sapphire
						else if (Skin == 1)
							*Weapon->FallbackPaintKit() = 415; // Doppler Ruby
						else if (Skin == 2)
							*Weapon->FallbackPaintKit() = 409; // Tiger Tooth
						else if (Skin == 3)
							*Weapon->FallbackPaintKit() = 558; // Lore
					}

					*Weapon->OwnerXuidLow() = 0;
					*Weapon->OwnerXuidHigh() = 0;
					*Weapon->FallbackWear() = 0.001f;
					*Weapon->m_AttributeManager()->m_Item()->ItemIDHigh() = 1;
				}
			}
		}
		if (pLocal->IsAlive())
			R::Resolve();
	}

	oFrameStageNotify(curStage);
}

void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup)
{
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{
		if (Menu::Window.VisualsTab.Active.GetState() && pLocal->IsAlive() && !pLocal->IsScoped())
		{
			if (pSetup->fov = 90)
				pSetup->fov = Menu::Window.VisualsTab.OtherFOV.GetValue();
		}

		oOverrideView(ecx, edx, pSetup);
	}

}

void GetViewModelFOV(float& fov)
{
	IClientEntity* localplayer = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{

		if (!localplayer)
			return;


		if (Menu::Window.VisualsTab.Active.GetState())
		fov += Menu::Window.VisualsTab.OtherViewmodelFOV.GetValue();
	}
}

float __stdcall GGetViewModelFOV()
{
	float fov = Hooks::VMTClientMode.GetMethod<oGetViewModelFOV>(35)();

	GetViewModelFOV(fov);

	return fov;
}

void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	static DWORD oRenderView = Hooks::VMTRenderView.GetOriginalFunction(6);

	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	__asm
	{
		PUSH whatToDraw
		PUSH nClearFlags
		PUSH hudViewSetup
		PUSH setup
		MOV ECX, ecx
		CALL oRenderView
	}
} //hooked for no reason yay

//https://www.unknowncheats.me/forum/counterstrike-global-offensive/418290-hooking-writeusercmddeltatobuffer.html

//char __fastcall CInput__WriteUsercmdDeltaToBuffer(_DWORD *a1, int a2, int a3, int a4, int a5, int a6, int a7)
bool __fastcall Hooks::Hooked_WriteUsercmdDeltaToBuffer(void* ecx,
	[[maybe_unused]] void*nouse,
	int slot,
	bf_write* buf,
	int from, 
	int to,
	bool isnewcommand)
{
	//retn should not be zero
	static auto retn = GameUtils::FindPattern1("engine.dll", "84 C0 74 04 B0 01 EB 02 32 C0 8B FE 46 3B F3 7E C9 84 C0 0F 84 ? ? ? ?");


	if(retn != 0 && _ReturnAddress() != (void*)retn && oWriteUsercmdDeltaToBuffer)
	return oWriteUsercmdDeltaToBuffer(ecx, nouse,slot, buf, from, to, isnewcommand);

	if(Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{
		if(Globals::Tick::tickshift <= 0)
		return oWriteUsercmdDeltaToBuffer(ecx, nouse, slot, buf, from, to, isnewcommand);
		
		if(from != -1)
			return true;

		int* pNumBackupCommands = (int*)((uintptr_t)buf - 0x30);
		int* pNumNewCommands = (int*)((uintptr_t)buf - 0x2C);
		int32_t new_commands = *pNumNewCommands;

		//IDA CL_Move
		//if ( *(gClientStates + 0x108) == 6 )
		//nextcommandnr = *(gClientStates + 0x4D2C) + 1 + *(gClientStates + 0x4D30);
		auto m_nChokedCommands = *(int*)(Interfaces::gpClientState + ClientStates::offset_m_nChokedCommands);
		auto m_nLastOutgoingCommand = *(int*)(Interfaces::gpClientState + ClientStates::offset_m_nLastOutgoingCommand);

		int32_t next_cmdnr = m_nLastOutgoingCommand + m_nChokedCommands + 1;
		//maxUsercmdProcessticks is 8 ticks on valve servers
		int32_t total_new_commands = min(Globals::Tick::tickshift, Globals::Tick::maxUsercmdProcessticks);
		//m_TickbaseShift = 0
		Globals::Tick::tickshift = 0;

		from = -1;
		*pNumBackupCommands = total_new_commands;
		*pNumBackupCommands = 0;

		for (to = next_cmdnr - new_commands + 1; to <= next_cmdnr; to++) {
			if (!oWriteUsercmdDeltaToBuffer(ecx, nouse,slot, buf, from, to, true))
				return false;

			from = to;
		}

		CInput::CUserCmd* last_realCmd = Interfaces::pInput->GetUserCmd(slot, from);
		CInput::CUserCmd fromCmd;

		if (last_realCmd)
			fromCmd = *last_realCmd;

		CInput::CUserCmd toCmd = fromCmd;
		toCmd.command_number++;
		toCmd.tick_count += 200;

		static WriteUsercmdFn pWriteUsercmdFn = (WriteUsercmdFn)GameUtils::FindPattern1("client.dll",
			"55 8B EC 83 E4 F8 51 53 56 8B D9");

		for (int i = new_commands; i <= total_new_commands; i++) {
			pWriteUsercmdFn(buf, &toCmd, &fromCmd);
			fromCmd = toCmd;
			toCmd.command_number++;
			toCmd.tick_count++;
		}

		return true;
	}
	else
	return oWriteUsercmdDeltaToBuffer(ecx, nouse, slot, buf, from, to, isnewcommand);

}

void Globals::Tick::shiftTicks(int ticks, CUserCmd* cmd, bool shiftAnyways = false)
{
	using namespace Globals::Tick;
	auto localPlayer = hackManager.pLocal();
	if (!localPlayer || !localPlayer->IsAlive())
		return;

	if (!canShift(ticks, shiftAnyways))
		return;

	commandNumber = cmd->command_number;
	tickbase = localPlayer->GetTickBase();
	tickshift = ticks;
	lastShift = cmd->command_number;

}

void Globals::Tick::recalculateTicks()
{
	using namespace Globals::Tick;
	chokedPackets = std::clamp(chokedPackets, 0, maxUsercmdProcessticks);
	ticksAllowedForProcessing = maxUsercmdProcessticks - chokedPackets;
	ticksAllowedForProcessing = std::clamp(ticksAllowedForProcessing, 0, maxUsercmdProcessticks);
}

bool Globals::Tick::canShift(int ticks, bool shiftAnyways = false)
{
	using namespace Globals::Tick;
	
	auto localPlayer = hackManager.pLocal();
	CBaseCombatWeapon* activeWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(localPlayer->GetActiveWeaponHandle());
	if (!localPlayer || !localPlayer->IsAlive() || ticks <= 0)
		return false;

	if (shiftAnyways)
		return true;

	if ((ticksAllowedForProcessing - ticks) < 0)
		return false;

	if (activeWeapon->GetNextPrimaryAttack() > Interfaces::Globals->realtime)
		return false;

	if (!activeWeapon || !activeWeapon->GetAmmoInClip())// 
		return false;

	if (!GameUtils::IsBallisticWeapon(activeWeapon)
		|| *activeWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == (short)WeaponId::Revolver
		|| *activeWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == (short)WeaponId::Awp
		|| *activeWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == (short)WeaponId::Ssg08
		|| *activeWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == (short)WeaponId::Taser
		|| *activeWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == (short)(WeaponId::Revolver))
		return false;

	float shiftTime = (localPlayer->GetTickBase() - ticks) * Interfaces::Globals->intervalPerTick;

	if (shiftTime < activeWeapon->GetNextPrimaryAttack())
		return false;

	return true;


}

void Globals::Tick::run(CUserCmd* cmd, bool& sendPacket)
{
	static void* oldNetwork = nullptr;
	auto localPlayer = hackManager.pLocal();

	if (cmd->buttons & IN_ATTACK) {
		if (Menu::Window.RageBotTab.DoubleTap.GetState())
		{
			static void* oldNetwork = nullptr;

			NetworkChannel* network = Interfaces::Engine->getNetworkChannel();

			if(g_pGameRules->IsValveServer())
				Globals::Tick::maxUsercmdProcessticks = 8;

			if (network && oldNetwork != network)
			{
				oldNetwork = network;
				Globals::Tick::ticksAllowedForProcessing = Globals::Tick::maxUsercmdProcessticks;
				Globals::Tick::chokedPackets = 0;
			}

			if (network && network->chokedPackets > Globals::Tick::chokedPackets)
				Globals::Tick::chokedPackets = network->chokedPackets;


			recalculateTicks();

			ticks = cmd->tick_count;

			if (!localPlayer || !localPlayer->IsAlive())
				return;

			//speed
			auto ticksspedd = 6;

			shiftTicks(ticksspedd, cmd);

			if (tickshift <= 0 && ticksAllowedForProcessing < (maxUsercmdProcessticks - fakeLag) && (cmd->command_number - lastShift) >= maxUsercmdProcessticks)
			{
				sendPacket = true;
				cmd->tick_count = INT_MAX; //recharge
				chokedPackets--;
			}

			recalculateTicks();
		}
	}
}