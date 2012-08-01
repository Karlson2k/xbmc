/*
 *      Copyright (C) 2005-2011 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "PeripheralHID.h"
#include "utils/log.h"
#include "settings/Settings.h"
#include "guilib/LocalizeStrings.h"
#include "input/ButtonTranslator.h"
#if defined(_WIN32) && defined(HAS_SDL_JOYSTICK)
#include "utils/SystemInfo.h"
#include "settings/GUIWindowSettingsCategory.h"
#include "input/windows/WINJoystick.h"
#include "peripherals/Peripherals.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/Key.h"
#endif


using namespace PERIPHERALS;
using namespace std;

CPeripheralHID::CPeripheralHID(const PeripheralType type, const PeripheralBusType busType, const CStdString &strLocation, const CStdString &strDeviceName, int iVendorId, int iProductId) :
  CPeripheral(type, busType, strLocation, strDeviceName.IsEmpty() ? g_localizeStrings.Get(35001) : strDeviceName, iVendorId, iProductId),
  m_bInitialised(false)
{
  m_features.push_back(FEATURE_HID);
#if defined(_WIN32) && defined(HAS_SDL_JOYSTICK)
  if (strDeviceName.Equals("Problematic iMON"))
    m_features.push_back(FEATURE_PROBLEMIMON);
#endif
}

CPeripheralHID::~CPeripheralHID(void)
{
  if (!m_strKeymap.IsEmpty() && !GetSettingBool("do_not_use_custom_keymap"))
  {
    CLog::Log(LOGDEBUG, "%s - switching active keymapping to: default", __FUNCTION__);
    CButtonTranslator::GetInstance().RemoveDevice(m_strKeymap);
  }
#if defined(_WIN32) && defined(HAS_SDL_JOYSTICK)
  if (HasFeature(FEATURE_PROBLEMIMON))
  {
    g_peripherals.DecNumberOfPromlemImons(); 
	if (!g_peripherals.IsProblemImonPresent())
    {
      // Itself instance is already excluded from total feature count
      g_sysinfo.SetProblemImonIsPresent(false);
      CLog::Log(LOGNOTICE, "Problematic iMON hardware was removed. Joystick usage: %s", 
        (g_guiSettings.GetBool("input.enablejoystick")) ? "enabled." : "disabled." );
      g_Joystick.SetEnabled(g_guiSettings.GetBool("input.enablejoystick"));
      CSetting* setting = g_guiSettings.GetSetting("input.disablejoystickwithimon");
      if(setting)
        setting->SetVisible(false);
      CGUIWindowSettingsCategory * window = (CGUIWindowSettingsCategory *)g_windowManager.GetWindow(WINDOW_SETTINGS_SYSTEM);
      if (window && window->IsActive())
		window->Update();
    }
  }
#endif
}

bool CPeripheralHID::InitialiseFeature(const PeripheralFeature feature)
{
  if (feature == FEATURE_HID && !m_bInitialised)
  {
    m_bInitialised = true;

    if (HasSetting("keymap"))
      m_strKeymap = GetSettingString("keymap");

    if (m_strKeymap.IsEmpty())
    {
      m_strKeymap.Format("v%sp%s", VendorIdAsString(), ProductIdAsString());
      SetSetting("keymap", m_strKeymap);
    }

    if (!IsSettingVisible("keymap"))
      SetSettingVisible("do_not_use_custom_keymap", false);

    if (!m_strKeymap.IsEmpty())
    {
      bool bKeymapEnabled(!GetSettingBool("do_not_use_custom_keymap"));
      if (bKeymapEnabled)
      {
        CLog::Log(LOGDEBUG, "%s - adding keymapping for: %s", __FUNCTION__, m_strKeymap.c_str());
        CButtonTranslator::GetInstance().AddDevice(m_strKeymap);
      }
      else if (!bKeymapEnabled)
      {
        CLog::Log(LOGDEBUG, "%s - removing keymapping for: %s", __FUNCTION__, m_strKeymap.c_str());
        CButtonTranslator::GetInstance().RemoveDevice(m_strKeymap);
      }
    }

    CLog::Log(LOGDEBUG, "%s - initialised HID device (%s:%s)", __FUNCTION__, m_strVendorId.c_str(), m_strProductId.c_str());
  }
#if defined(_WIN32) && defined(HAS_SDL_JOYSTICK)
  else if (feature == FEATURE_PROBLEMIMON)
  {
    g_sysinfo.SetProblemImonIsPresent(true);
	g_peripherals.IncNumberOfPromlemImons();
    CLog::Log(LOGNOTICE, "Problematic iMON hardware detected. Joystick usage: %s", 
      (!g_guiSettings.GetBool("input.disablejoystickwithimon") && g_guiSettings.GetBool("input.enablejoystick")) ? "enabled." : "disabled." );
    g_Joystick.SetEnabled(!g_guiSettings.GetBool("input.disablejoystickwithimon") && g_guiSettings.GetBool("input.enablejoystick"));
    CSetting* setting = g_guiSettings.GetSetting("input.disablejoystickwithimon");
    if(setting)
      setting->SetVisible(!setting->IsAdvanced());
    CGUIWindowSettingsCategory * window = (CGUIWindowSettingsCategory *)g_windowManager.GetWindow(WINDOW_SETTINGS_SYSTEM);
    if (window && window->IsActive())
      window->Update();
  }
#endif

  return CPeripheral::InitialiseFeature(feature);
}

void CPeripheralHID::OnSettingChanged(const CStdString &strChangedSetting)
{
  if (m_bInitialised && ((strChangedSetting.Equals("keymap") && !GetSettingBool("do_not_use_custom_keymap")) || strChangedSetting.Equals("keymap_enabled")))
  {
    m_bInitialised = false;
    InitialiseFeature(FEATURE_HID);
  }
}

