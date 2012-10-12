/*
 *      Copyright (C) 2012 Team XBMC
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <sstream>
#include "AEDeviceInfo.h"
#include "AEUtil.h"

#if defined(TARGET_WINDOWS)
  #include "Sinks/AESinkWASAPI.h"
  #include "Sinks/AESinkDirectSound.h"
#elif defined(TARGET_ANDROID)
  #include "Sinks/AESinkAUDIOTRACK.h"
#elif defined(TARGET_LINUX) || defined(TARGET_FREEBSD)
  #if defined(HAS_ALSA)
    #include "Sinks/AESinkALSA.h"
  #endif
  #include "Sinks/AESinkOSS.h"
#endif



CAEDeviceInfo::operator std::string()
{
  std::stringstream ss;
  ss << "m_deviceName      : " << m_deviceName << '\n';
  ss << "m_displayName     : " << m_displayName << '\n';
  ss << "m_displayNameExtra: " << m_displayNameExtra << '\n';
  ss << "m_deviceType      : " << DeviceTypeToString(m_deviceType) + '\n';
  ss << "m_channels        : " << (std::string)m_channelsFormats[0] << '\n'; // HACK: do it properly!

  ss << "m_sampleRates     : ";
  for (AESampleRateList::iterator itt = m_sampleRates.begin(); itt != m_sampleRates.end(); ++itt)
  {
    if (itt != m_sampleRates.begin())
      ss << ',';
    ss << *itt;
  }
  ss << '\n';

  ss << "m_dataFormats     : ";
  for (AEDataFormatList::iterator itt = m_dataFormats.begin(); itt != m_dataFormats.end(); ++itt)
  {
    if (itt != m_dataFormats.begin())
      ss << ',';
    ss << CAEUtil::DataFormatToStr(*itt);
  }
  ss << '\n';

  return ss.str();
}

std::string CAEDeviceInfo::DeviceTypeToString(enum AEDeviceType deviceType)
{
  switch (deviceType)
  {
    case AE_DEVTYPE_PCM   : return "AE_DEVTYPE_PCM"   ; break;
    case AE_DEVTYPE_IEC958: return "AE_DEVTYPE_IEC958"; break;
    case AE_DEVTYPE_HDMI  : return "AE_DEVTYPE_HDMI"  ; break;
    case AE_DEVTYPE_DP    : return "AE_DEVTYPE_DP"    ; break;
  }
  return "INVALID";
}

CAEDeviceInfo::CAEDeviceInfo(AESinkType sinkType):
  m_sinkType(sinkType)
{
  switch(sinkType)
  {
  case AE_SINK_NULL:
  case AE_SINK_PROFILER:
    m_ptrSinkSpecificInfo = NULL;
    return;
#if defined(TARGET_WINDOWS)
  case AE_SINK_WASAPI:
    m_ptrSinkSpecificInfo = new CWASAPISpecificDeviceInfo();
    return;
  case AE_SINK_DIRECTSOUND:
    m_ptrSinkSpecificInfo = new CDirectSoundSpecificDeviceInfo();
    return;
#elif defined(TARGET_ANDROID)
  case AE_SINK_AUDIOTRACK:
    m_ptrSinkSpecificInfo = new CAUDIOTRACKSpecificDeviceInfo();
    return;
#elif defined(TARGET_LINUX) || defined(TARGET_FREEBSD)
#if defined(HAS_ALSA)
  case AE_SINK_ALSA:
    m_ptrSinkSpecificInfo = new CALSASpecificDeviceInfo();
    return;
  #endif
  case AE_SINK_OSS:
    m_ptrSinkSpecificInfo = new COSSSpecificDeviceInfo();
    return;
#endif
  }
  /* Should be unreachable */
  ASSERT(0);
}

CAEDeviceInfo::~CAEDeviceInfo()
{
  if (m_ptrSinkSpecificInfo)
    delete m_ptrSinkSpecificInfo;
}

CAEDeviceInfo::CAEDeviceInfo(const CAEDeviceInfo& deviceInfo):
  m_sinkType(deviceInfo.m_sinkType)
{
  m_deviceName        = deviceInfo.m_deviceName;	
  m_displayName       = deviceInfo.m_displayName;
  m_displayNameExtra  = deviceInfo.m_displayNameExtra;
  m_deviceType        = deviceInfo.m_deviceType;
  m_channelsFormats   = deviceInfo.m_channelsFormats;	  
  m_sampleRates       = deviceInfo.m_sampleRates;
  m_dataFormats       = deviceInfo.m_dataFormats;
  if (m_ptrSinkSpecificInfo)
  {
    switch(m_sinkType)
    {
    case AE_SINK_NULL:
    case AE_SINK_PROFILER:
      m_ptrSinkSpecificInfo = NULL;
      return;
  #if defined(TARGET_WINDOWS)
    case AE_SINK_WASAPI:
      m_ptrSinkSpecificInfo = new CWASAPISpecificDeviceInfo(*static_cast<CWASAPISpecificDeviceInfo*>(m_ptrSinkSpecificInfo));
      return;
    case AE_SINK_DIRECTSOUND:
      m_ptrSinkSpecificInfo = new CDirectSoundSpecificDeviceInfo(*static_cast<CDirectSoundSpecificDeviceInfo*>(m_ptrSinkSpecificInfo));
      return;
  #elif defined(TARGET_ANDROID)
    case AE_SINK_AUDIOTRACK:
      m_ptrSinkSpecificInfo = new CAUDIOTRACKSpecificDeviceInfo(*static_cast<CAUDIOTRACKSpecificDeviceInfo*>(m_ptrSinkSpecificInfo));
      return;
  #elif defined(TARGET_LINUX) || defined(TARGET_FREEBSD)
  #if defined(HAS_ALSA)
    case AE_SINK_ALSA:
      m_ptrSinkSpecificInfo = new CALSASpecificDeviceInfo(*static_cast<CALSASpecificDeviceInfo*>(m_ptrSinkSpecificInfo));
      return;
    #endif
    case AE_SINK_OSS:
      m_ptrSinkSpecificInfo = new COSSSpecificDeviceInfo(*static_cast<COSSSpecificDeviceInfo*>(m_ptrSinkSpecificInfo));
      return;
  #endif
    }
    /* Should be unreachable */
    ASSERT(0);
  }
  else
  {
    /* Just a precaution, shouldn't be used in real life */
    switch(m_sinkType)
    {
    case AE_SINK_NULL:
    case AE_SINK_PROFILER:
      m_ptrSinkSpecificInfo = NULL;
      return;
  #if defined(TARGET_WINDOWS)
    case AE_SINK_WASAPI:
      m_ptrSinkSpecificInfo = new CWASAPISpecificDeviceInfo();
      return;
    case AE_SINK_DIRECTSOUND:
      m_ptrSinkSpecificInfo = new CDirectSoundSpecificDeviceInfo();
      return;
  #elif defined(TARGET_ANDROID)
    case AE_SINK_AUDIOTRACK:
      m_ptrSinkSpecificInfo = new CAUDIOTRACKSpecificDeviceInfo();
      return;
  #elif defined(TARGET_LINUX) || defined(TARGET_FREEBSD)
  #if defined(HAS_ALSA)
    case AE_SINK_ALSA:
      m_ptrSinkSpecificInfo = new CALSASpecificDeviceInfo();
      return;
    #endif
    case AE_SINK_OSS:
      m_ptrSinkSpecificInfo = new COSSSpecificDeviceInfo();
      return;
  #endif
    }
    /* Should be unreachable */
    ASSERT(0);
  }
}

#if defined(TARGET_WINDOWS)
CWASAPISpecificDeviceInfo& CAEDeviceInfo::WASAPIDeviceInfo()
{
  ASSERT(m_sinkType == AE_SINK_WASAPI);
  return *static_cast<CWASAPISpecificDeviceInfo*>(m_ptrSinkSpecificInfo);
}

CDirectSoundSpecificDeviceInfo& CAEDeviceInfo::DirectSoundDeviceInfo()
{
  ASSERT(m_sinkType == AE_SINK_DIRECTSOUND);
  return *static_cast<CDirectSoundSpecificDeviceInfo*>(m_ptrSinkSpecificInfo);
}
#elif defined(TARGET_ANDROID)
CAUDIOTRACKSpecificDeviceInfo& CAEDeviceInfo::AUDIOTRACKDeviceInfo()
{
  ASSERT(m_sinkType == AE_SINK_AUDIOTRACK);
  return *static_cast<CAUDIOTRACKSpecificDeviceInfo*>(m_ptrSinkSpecificInfo);
}
#elif defined(TARGET_LINUX) || defined(TARGET_FREEBSD)
#if defined(HAS_ALSA)
CALSASpecificDeviceInfo& CAEDeviceInfo::ALSADeviceInfo();
{
  ASSERT(m_sinkType == AE_SINK_ALSA);
  return *static_cast<CALSASpecificDeviceInfo*>(m_ptrSinkSpecificInfo);
}
#endif
COSSSpecificDeviceInfo& CAEDeviceInfo::OSSDeviceInfo();
{
  ASSERT(m_sinkType == AE_SINK_OSS);
  return *static_cast<COSSSpecificDeviceInfo*>(m_ptrSinkSpecificInfo);
}
#endif
