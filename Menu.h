/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "GUI.h"
#include "Controls.h"

class CRageBotTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	// Aimbot Settings
	CGroupBox AimbotGroup;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CSlider	  AimbotFov;
	CCheckBox AimbotSilentAim;
	CCheckBox AimbotPerfectSilentAim;
	CCheckBox AimbotAutoPistol;
	CCheckBox AimbotAimStep;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CKeyBind  AimbotStopKey;

	// Target Selection Settings
	CGroupBox TargetGroup;
	CComboBox TargetSelection;
	CCheckBox TargetFriendlyFire;
	CComboBox TargetHitbox;
	CComboBox TargetHitscan;
	CCheckBox TargetMultipoint;
	CSlider   TargetPointscale;

	// Accuracy Settings
	CGroupBox AccuracyGroup;
	CCheckBox AccuracyRecoil;
	CCheckBox AccuracyAutoWall;
	CSlider	  AccuracyMinimumDamage;
	CCheckBox DoubleTap;
	CCheckBox AccuracyAutoCrouch;
	CCheckBox AccuracyAutoScope;
	CSlider   AccuracyHitchance;
	CComboBox AccuracyResolver;
	CCheckBox AccuracyPositionAdjustment;
	CSlider   AccuracySmart;

	// Anti-Aim Settings
	CGroupBox AntiAimGroup;
	CCheckBox AntiAimEnable;
	CComboBox AntiAimPitch;
	CComboBox AntiAimYaw;
	CComboBox AntiAimEdge;
	CCheckBox ChamsDesyAA;
	CCheckBox AntiAimKnife;
	CCheckBox AntiAimTarget;
};

class CLegitBotTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	// Aimbot Settings
	CGroupBox AimbotGroup;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CCheckBox AimbotFriendlyFire;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CCheckBox AimbotAutoPistol;
	CSlider   AimbotInaccuracy;
	CKeyBind  AimbotDisableVis;

	// Main
	CGroupBox TriggerGroup;
	CCheckBox TriggerEnable;
	CCheckBox TriggerKeyPress;
	CKeyBind  TriggerKeyBind;
	CSlider   TriggerDelay;

	// Main
	CGroupBox WeaponMainGroup;
	CSlider   WeaponMainSpeed;
	CSlider   WeaponMainFoV;
	CCheckBox WeaponMainRecoil;
	CCheckBox WeaponMainPSilent;
	CSlider   WeaponMainInacc;
	CComboBox WeaponMainHitbox;

	// Pistol
	CGroupBox WeaponPistGroup;
	CSlider   WeaponPistSpeed;
	CSlider   WeaponPistFoV;
	CCheckBox WeaponPistRecoil;
	CCheckBox WeaponPistPSilent;
	CSlider   WeaponPistInacc;
	CComboBox WeaponPistHitbox;

	// Sniper
	CGroupBox WeaponSnipGroup;
	CSlider   WeaponSnipSpeed;
	CSlider   WeaponSnipFoV;
	CCheckBox WeaponSnipRecoil;
	CCheckBox WeaponSnipPSilent;
	CSlider   WeaponSnipInacc;
	CComboBox WeaponSnipHitbox;
};

class CVisualTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	// Options Settings
	CGroupBox OptionsGroup;
	CCheckBox OptionsBox;
	CCheckBox OptionsName;
	CCheckBox OptionsHealth;
	CCheckBox OptionsWeapon;
	CCheckBox OptionsInfo;
	CCheckBox OptionsHelmet;
	CCheckBox OptionsKit;
	CCheckBox OptionsDefuse;
	//CCheckBox OptionsGlow;
	CComboBox OptionsChams;
	CCheckBox OptionsSkeleton;
	CCheckBox OptionsAimSpot;

	// Filters Settings
	CGroupBox FiltersGroup;
	CCheckBox FiltersAll;
	CCheckBox FiltersPlayers;
	CCheckBox FiltersEnemiesOnly;
	CCheckBox FiltersWeapons;
	CCheckBox FiltersChickens;
	CCheckBox FiltersC4;

	// Other Settings
	CGroupBox OtherGroup;
	CCheckBox OtherCrosshair;
	CComboBox OtherRecoilCrosshair;
	CCheckBox OtherHitmarker;
	CCheckBox OtherRadar;
	CCheckBox OtherNoVisualRecoil;
	CCheckBox OtherNoSky; 
	CCheckBox OtherNoFlash; 
	CCheckBox OtherNoSmoke;
	CCheckBox OtherAsusWalls;
	CComboBox OtherNoHands;
	CSlider OtherViewmodelFOV;
	CSlider OtherFOV;
};

class CMiscTab : public CTab
{
public:
	void Setup();

	// Knife Changer
	CGroupBox KnifeGroup;
	CCheckBox KnifeEnable;
	CComboBox KnifeModel;
	CComboBox KnifeSkin;
	CButton   KnifeApply;

	// Other Settings
	CGroupBox OtherGroup;
	CComboBox OtherAutoJump;
	CKeyBind OtherSlowWalk;
	CComboBox OtherAutoStrafe;
	CCheckBox OtherSafeMode;
	CComboBox OtherChatSpam;
	CCheckBox OtherTeamChat;
	CSlider	  OtherChatDelay;
	CKeyBind  OtherAirStuck;
	CKeyBind  OtherLagSwitch;
	CCheckBox OtherSpectators;
	CKeyBind OtherThirdperson;
	//CCheckBox OtherAutoAccept;
	CCheckBox OtherWalkbot;
	CComboBox OtherClantag;

	// Fake Lag Settings
	CGroupBox FakeLagGroup;
	CCheckBox FakeLagEnable;
	CSlider   FakeLagChoke;
	CSlider	  FakeLagSend;
	CCheckBox ChokeRandomize;
	CCheckBox SendRandomize;
	//CCheckBox FakeLagWhileShooting;

	// EventListenr 
	CGroupBox EventListner;
	CCheckBox FireEvent;
	CCheckBox FireBulletTrace;

	
};

class AyyWareWindow : public CWindow
{
public:
	void Setup();

	CRageBotTab RageBotTab;
	CLegitBotTab LegitBotTab;
	CVisualTab VisualsTab;
	CMiscTab MiscTab;

	CButton SaveButton;
	CButton LoadButton;
};

namespace Menu
{
	void SetupMenu();
	void DoUIFrame();
	void UICheatStatus();
	void InitConfig();

	extern AyyWareWindow Window;
};