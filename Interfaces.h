/*
Syn's AyyWare Framework
*/

#pragma once

// Includes
#include "CommonIncludes.h"
#include "SDK.h"
#include "RenderBeams.h"

class IGameEventManager2;

// Namespace to contain all the valve interfaces
namespace Interfaces
{
	// Gets handles to all the interfaces needed
	void Initialise();

	extern IBaseClientDLL* Client;
	extern IVEngineClient* Engine;
	extern IPanel* Panels;
	extern IClientEntityList* EntList;
	extern ISurface* Surface;
	extern IVDebugOverlay* DebugOverlay;
	extern IClientModeShared* ClientMode;

	extern CGlobalVarsBase *Globals;
	extern DWORD *Prediction;
	extern CMaterialSystem* MaterialSystem;
	extern CVRenderView* RenderView;
	extern IVModelRender* ModelRender;
	extern CModelInfo* ModelInfo;
	extern IEngineTrace* Trace;
	extern IPhysicsSurfaceProps* PhysProps;
	extern ICVar *CVar;
	extern CInput* pInput;
	extern IInputSystem* InputSystem;
	extern IViewRenderBeams* g_pViewRenderBeams;
};