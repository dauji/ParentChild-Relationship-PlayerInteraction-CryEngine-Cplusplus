////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2011.
// -------------------------------------------------------------------------
//  File name:   UISettings.cpp
//  Version:     v1.00
//  Created:     10/8/2011 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UISettings.h"
#include "UIManager.h"

#include <ILevelSystem.h>

////////////////////////////////////////////////////////////////////////////
CUISettings::CUISettings()
	: m_pUIEvents(NULL)
	, m_pUIFunctions(NULL)
	, m_currResId(0)
{
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::InitEventSystem()
{
	if (!gEnv->pFlashUI) return;

	// CVars
 	m_pRXVar = gEnv->pConsole->GetCVar("r_Width");
 	m_pRYVar = gEnv->pConsole->GetCVar("r_Height");
 	m_pFSVar = gEnv->pConsole->GetCVar("r_Fullscreen");
	m_pMusicVar = gEnv->pConsole->GetCVar("s_MusicVolume");
	m_pSFxVar = gEnv->pConsole->GetCVar("s_SFXVolume");
	m_pVideoVar = gEnv->pConsole->GetCVar("sys_flash_video_soundvolume");
	m_pVideoVar = m_pVideoVar ? m_pVideoVar : m_pMusicVar; // tmp fix to allow using music var as dummy fallback if GFX_VIDEO is not enabled (FreeSDK)
	m_pMouseSensitivity = gEnv->pConsole->GetCVar("cl_sensitivity");
	m_pInvertMouse = gEnv->pConsole->GetCVar("cl_invertMouse");
	m_pInvertController = gEnv->pConsole->GetCVar("cl_invertController");

	assert(m_pRXVar 
		&& m_pRYVar 
		&& m_pFSVar 
		&& m_pMusicVar 
		&& m_pSFxVar 
		&& m_pVideoVar
		&& m_pMouseSensitivity
		&& m_pInvertMouse
		&& m_pInvertController);

	if (!(m_pRXVar 
		&& m_pRYVar 
		&& m_pFSVar 
		&& m_pMusicVar 
		&& m_pSFxVar 
		&& m_pVideoVar
		&& m_pMouseSensitivity
		&& m_pInvertMouse
		&& m_pInvertController)) return;


	m_Resolutions.push_back(std::make_pair(1024,768));
	m_Resolutions.push_back(std::make_pair(1280,720));
	m_Resolutions.push_back(std::make_pair(1280,1050));
	m_Resolutions.push_back(std::make_pair(1680,1050));
	m_Resolutions.push_back(std::make_pair(1920,1080));


	// events to send from this class to UI flowgraphs
	m_pUIFunctions = gEnv->pFlashUI->CreateEventSystem("Settings", IUIEventSystem::eEST_SYSTEM_TO_UI);
	m_eventSender.Init(m_pUIFunctions);

	{
		SUIEventDesc eventDesc("OnGraphicChanged", "Triggered on graphic settings change");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>("Resolution", "Resolution ID");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>("ResX", "Screen X resolution");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>("ResY", "Screen Y resolution");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>("FullScreen", "Fullscreen");
		m_eventSender.RegisterEvent<eUIE_GraphicSettingsChanged>(eventDesc);
	}

	{
		SUIEventDesc eventDesc("OnSoundChanged", "Triggered if sound volume changed");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>("Music", "Music volume");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>("SFx", "SFx volume");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>("Video", "Video volume");
		m_eventSender.RegisterEvent<eUIE_SoundSettingsChanged>(eventDesc);
	}

	{
		SUIEventDesc eventDesc("OnGameSettingsChanged", "Triggered if game settings changed");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>("MouseSensitivity", "Mouse Sensitivity");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>("InvertMouse", "Invert Mouse");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>("InvertController", "Invert Controller");
		m_eventSender.RegisterEvent<eUIE_GameSettingsChanged>(eventDesc);
	}

	{
		SUIEventDesc eventDesc("OnResolutions", "Triggered if resolutions were requested.");
		eventDesc.SetDynamic("Resolutions", "UI array with all resolutions (x1,y1,x2,y2,...)");
		m_eventSender.RegisterEvent<eUIE_OnGetResolutions>(eventDesc);
	}

	{
		SUIEventDesc eventDesc("OnResolutionItem", "Triggered once per each resolution if resolutions were requested.");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_String>("ResString", "Resolution as string (X x Y)");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>("ID", "Resolution ID");
		m_eventSender.RegisterEvent<eUIE_OnGetResolutionItems>(eventDesc);
	}

	{
		SUIEventDesc eventDesc("OnLevelItem", "Triggered once per level if levels were requested.");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_String>("LevelLabel", "@ui_<level> for localization");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_String>("LevelName", "name of the level");
		m_eventSender.RegisterEvent<eUIE_OnGetLevelItems>(eventDesc);
	}


	// events that can be sent from UI flowgraphs to this class
	m_pUIEvents = gEnv->pFlashUI->CreateEventSystem("Settings", IUIEventSystem::eEST_UI_TO_SYSTEM);
	m_eventDispatcher.Init(m_pUIEvents, this, "UISettings");

	{
		SUIEventDesc eventDesc("SetGraphics", "Call this to set graphic modes");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>("Resolution", "Resolution ID");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>("Fullscreen", "Fullscreen (True/False)");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnSetGraphicSettings);
	}

	{
		SUIEventDesc eventDesc("SetResolution", "Call this to set resolution");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>("ResX", "Screen X resolution");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Int>("ResY", "Screen Y resolution");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>("Fullscreen", "Fullscreen (True/False)");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnSetResolution);
	}

	{
		SUIEventDesc eventDesc("SetSound", "Call this to set sound settings");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>("Music", "Music volume");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>("SFx", "SFx volume");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>("Video", "Video volume");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnSetSoundSettings);
	}

	{
		SUIEventDesc eventDesc("SetGameSettings", "Call this to set game settings");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Float>("MouseSensitivity", "Mouse Sensitivity");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>("InvertMouse", "Invert Mouse");
		eventDesc.AddParam<SUIParameterDesc::eUIPT_Bool>("InvertController", "Invert Controller");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnSetGameSettings);
	}

	{
		SUIEventDesc eventDesc("GetResolutionList", "Execute this node will trigger the \"Events:Settings:OnResolutions\" node.");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnGetResolutions);
	}

	{
		SUIEventDesc eventDesc("GetCurrGraphics", "Execute this node will trigger the \"Events:Settings:OnGraphicChanged\" node.");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnGetCurrGraphicsSettings);
	}

	{
		SUIEventDesc eventDesc("GetCurrSound", "Execute this node will trigger the \"Events:Settings:OnSoundChanged\" node.");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnGetCurrSoundSettings);
	}

	{
		SUIEventDesc eventDesc("GetCurrGameSettings", "Execute this node will trigger the \"Events:Settings:OnGameSettingsChanged\" node.");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnGetCurrGameSettings);
	}

	{
		SUIEventDesc eventDesc("GetLevels", "Execute this node will trigger the \"Events:Settings:OnLevelItem\" node once per level.");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnGetLevels);
	}

	{
		SUIEventDesc eventDesc("LogoutUser", "Execute this node to save settings and logout user");
		m_eventDispatcher.RegisterEvent(eventDesc, &CUISettings::OnLogoutUser);
	}

	gEnv->pFlashUI->RegisterModule(this, "CUISettings");
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::UnloadEventSystem()
{
	gEnv->pFlashUI->UnregisterModule(this);
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::Init()
{
#ifdef _RELEASE
	for (int i = 0; i < m_Resolutions.size(); ++i)
	{
		if (m_Resolutions[i].first == m_pRXVar->GetIVal() && m_Resolutions[i].second == m_pRYVar->GetIVal())
		{
			m_currResId = i;
			SendGraphicSettingsChange();
			break;
		}
	}
	SendSoundSettingsChange();
	SendGameSettingsChange();
#endif
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::Update(float fDeltaTime)
{
#ifndef _RELEASE
 	static int rX = -1;
 	static int rY = -1;
 	if (rX != m_pRXVar->GetIVal() || rY != m_pRYVar->GetIVal())
	{
		rX = m_pRXVar->GetIVal();
		rY = m_pRYVar->GetIVal();
		m_currResId = 0;

		for (int i = 0; i < m_Resolutions.size(); ++i)
		{
			if (m_Resolutions[i].first == rX && m_Resolutions[i].second == rY)
			{
				m_currResId = i;
				SendGraphicSettingsChange();
				break;
			}
		}

	}

	static float music = -1.f;
	static float sfx = -1.f;
	static float video = -1.f;
	if (music != m_pMusicVar->GetFVal() || sfx != m_pSFxVar->GetFVal() || video != m_pVideoVar->GetFVal())
	{
		SendSoundSettingsChange();
		music = m_pMusicVar->GetFVal();
		sfx = m_pSFxVar->GetFVal();
		video = m_pVideoVar->GetFVal();
	}

	static float sensivity = -1.f;
	static bool invertMouse = m_pInvertMouse->GetIVal() == 1;
	static bool invertController = m_pInvertController->GetIVal() == 1;
	if (sensivity != m_pMouseSensitivity->GetFVal() || invertMouse != (m_pInvertMouse->GetIVal() == 1) || invertController != (m_pInvertController->GetIVal() == 1))
	{
		SendGameSettingsChange();
		sensivity = m_pMouseSensitivity->GetFVal();
		invertMouse = m_pInvertMouse->GetIVal() == 1;
		invertController = m_pInvertController->GetIVal() == 1;
	}

#endif
}

////////////////////////////////////////////////////////////////////////////
// ui events
////////////////////////////////////////////////////////////////////////////
void CUISettings::OnSetGraphicSettings( int resIndex, bool fullscreen )
{
#if !defined(PS3) && !defined(XENON)
	if (resIndex >= 0 && resIndex < m_Resolutions.size())
	{
		m_currResId = resIndex;

		m_pRXVar->Set(m_Resolutions[resIndex].first);
		m_pRYVar->Set(m_Resolutions[resIndex].second);
		m_pFSVar->Set(fullscreen);

		SendGraphicSettingsChange();
	}
#endif
}

////////////////////////////////////////////////////////////////////////////
// DEPRECATED: shoud consider to use OnSetGraphicSettings
void CUISettings::OnSetResolution( int resX, int resY, bool fullscreen )
{
#if !defined(PS3) && !defined(XENON)
	m_pRXVar->Set(resX);
	m_pRYVar->Set(resY);
	m_pFSVar->Set(fullscreen);
	return;
#endif
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::OnSetSoundSettings( float music, float sfx, float video )
{
	m_pMusicVar->Set(music);
	m_pSFxVar->Set(sfx);
	if (m_pVideoVar != m_pMusicVar)
		m_pVideoVar->Set(video);
	SendSoundSettingsChange();
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::OnSetGameSettings( float sensitivity, bool invertMouse, bool invertController )
{
	m_pMouseSensitivity->Set(sensitivity);
	m_pInvertMouse->Set(invertMouse);
	m_pInvertController->Set(invertController);
	SendGameSettingsChange();
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::OnGetResolutions()
{
	SendResolutions();
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::OnGetCurrGraphicsSettings()
{
	SendGraphicSettingsChange();
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::OnGetCurrSoundSettings()
{
	SendSoundSettingsChange();
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::OnGetCurrGameSettings()
{
	SendGameSettingsChange();
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::OnGetLevels()
{
	if (gEnv->pGame && gEnv->pGame->GetIGameFramework() && gEnv->pGame->GetIGameFramework()->GetILevelSystem())
	{
		int i = 0;
		while ( ILevelInfo* pLevelInfo = gEnv->pGame->GetIGameFramework()->GetILevelSystem()->GetLevelInfo( i++ ) )
		{
 			m_eventSender.SendEvent<eUIE_OnGetLevelItems>(pLevelInfo->GetDisplayName(), pLevelInfo->GetName());
		}
	}
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::OnLogoutUser()
{
	CUIManager* pManager = CUIManager::GetInstance();
	if (pManager)
		pManager->SaveProfile();

	if (gEnv->pGame && gEnv->pGame->GetIGameFramework() && gEnv->pGame->GetIGameFramework()->GetIPlayerProfileManager())
	{
		IPlayerProfileManager* pProfileManager = gEnv->pGame->GetIGameFramework()->GetIPlayerProfileManager();
		pProfileManager->LogoutUser(pProfileManager->GetCurrentUser());
	}
}


////////////////////////////////////////////////////////////////////////////
// ui functions
////////////////////////////////////////////////////////////////////////////
void CUISettings::SendResolutions()
{
	SUIArguments resolutions;
	for (int i = 0; i < m_Resolutions.size(); ++i)
	{
		string res;
		res.Format("%i x %i", m_Resolutions[i].first, m_Resolutions[i].second);
		m_eventSender.SendEvent<eUIE_OnGetResolutionItems>(res, (int)i);

		resolutions.AddArgument(m_Resolutions[i].first);
		resolutions.AddArgument(m_Resolutions[i].second);
	}
 	m_eventSender.SendEvent<eUIE_OnGetResolutions>(resolutions);
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::SendGraphicSettingsChange()
{
 	m_eventSender.SendEvent<eUIE_GraphicSettingsChanged>(m_currResId, m_Resolutions[m_currResId].first, m_Resolutions[m_currResId].second, m_pFSVar->GetIVal() != 0);
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::SendSoundSettingsChange()
{
 	m_eventSender.SendEvent<eUIE_SoundSettingsChanged>(m_pMusicVar->GetFVal(), m_pSFxVar->GetFVal(), m_pVideoVar->GetFVal());
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::SendGameSettingsChange()
{
 	m_eventSender.SendEvent<eUIE_GameSettingsChanged>(m_pMouseSensitivity->GetFVal(), m_pInvertMouse->GetIVal() != 0, m_pInvertController->GetIVal() != 0);
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::LoadProfile(IPlayerProfile* pProfile)
{
	if (!(m_pRXVar 
		&& m_pRYVar 
		&& m_pFSVar 
		&& m_pMusicVar 
		&& m_pSFxVar 
		&& m_pVideoVar
		&& m_pMouseSensitivity
		&& m_pInvertMouse
		&& m_pInvertController)) return;

	int rx = m_pRXVar->GetIVal();
	int ry = m_pRYVar->GetIVal();
	int fs = m_pFSVar->GetIVal();
	pProfile->GetAttribute( "res_x",  rx, false);
	pProfile->GetAttribute( "res_y",  ry, false);
	pProfile->GetAttribute( "res_fs", fs, false);
	m_pRXVar->Set(rx);
	m_pRYVar->Set(ry);
	m_pFSVar->Set(fs);

	float music = m_pMusicVar->GetFVal();
	float sound = m_pSFxVar->GetFVal();
	float video = m_pVideoVar->GetFVal();
	pProfile->GetAttribute( "sound_music", music, false);
	pProfile->GetAttribute( "sound_sound", sound, false);
	pProfile->GetAttribute( "sound_video", video, false);
	m_pMusicVar->Set(music);
	m_pSFxVar->Set(sound);
	m_pVideoVar->Set(video);

	float sensitivity = m_pMouseSensitivity->GetFVal();
	int invertMouse = m_pInvertMouse->GetIVal();
	int invertController = m_pInvertController->GetIVal();
	pProfile->GetAttribute( "controls_sensitivity", sensitivity, false);
	pProfile->GetAttribute( "controls_invertMouse", invertMouse, false);
	pProfile->GetAttribute( "controls_invertController", invertController, false);
	m_pMouseSensitivity->Set(sensitivity);
	m_pInvertMouse->Set(invertMouse);
	m_pInvertController->Set(invertController);
}

////////////////////////////////////////////////////////////////////////////
void CUISettings::SaveProfile(IPlayerProfile* pProfile) const
{
	if (!(m_pRXVar 
		&& m_pRYVar 
		&& m_pFSVar 
		&& m_pMusicVar 
		&& m_pSFxVar 
		&& m_pVideoVar
		&& m_pMouseSensitivity
		&& m_pInvertMouse
		&& m_pInvertController)) return;

	pProfile->SetAttribute( "res_x",  m_pRXVar->GetIVal());
	pProfile->SetAttribute( "res_y",  m_pRYVar->GetIVal());
	pProfile->SetAttribute( "res_fs", m_pFSVar->GetIVal());

	pProfile->SetAttribute( "sound_music", m_pMusicVar->GetFVal());
	pProfile->SetAttribute( "sound_sound", m_pSFxVar->GetFVal());
	pProfile->SetAttribute( "sound_video", m_pVideoVar->GetFVal());

	pProfile->SetAttribute( "controls_sensitivity", m_pMouseSensitivity->GetFVal());
	pProfile->SetAttribute( "controls_invertMouse", m_pInvertMouse->GetIVal());
	pProfile->SetAttribute( "controls_invertController", m_pInvertController->GetIVal());
}

////////////////////////////////////////////////////////////////////////////
REGISTER_UI_EVENTSYSTEM( CUISettings );
