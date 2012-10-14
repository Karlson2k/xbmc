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

std::string CAEDeviceInfo::GetAEDeviceName() const
{
  return SinkTypeToString(m_sinkType) + ":" + m_deviceName;
}

bool CAEDeviceInfo::SupportsRaw() const
{
  if (m_dataFormats.empty())
    return false;
  return AE_IS_RAW(m_dataFormats.back());
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

std::string CAEDeviceInfo::SinkTypeToString(enum AESinkType sinkType)
{
  switch (sinkType)
  {
  case AE_SINK_NULL:        return "NULL";        break;
  case AE_SINK_PROFILER:    return "PROFILER";    break;
  case AE_SINK_WASAPI:      return "WASAPI";      break;
  case AE_SINK_DIRECTSOUND: return "DIRECTSOUND"; break;
  case AE_SINK_ALSA:        return "ALSA";        break;
  case AE_SINK_OSS:         return "OSS";         break;
  case AE_SINK_AUDIOTRACK:  return "AUDIOTRACK";  break;
  }
  return "INVALID";
}

void CAEDeviceInfo::SortLists()
{
  /* Sort PCM samlerates */
  std::sort(m_sampleRates.begin(), m_sampleRates.end());
  /* Remove duplicates */
  std::unique(m_sampleRates.begin(), m_sampleRates.end());

  /* Sort channels formats by channels count */
  std::sort(m_channelsFormats.begin(), m_channelsFormats.end());

  /* Sort data formats */
  /* First is PCM formats, sorted by number of bits */
  /* Second is RAW formats */
  /* Third is HD RAW formats */
  bool hasU8 = false, hasS8 = false, hasS16BE = false, hasS16LE = false, hasS16NE = false,
    hasS32BE = false, hasS32LE = false, hasS32NE = false, hasS24BE4 = false, hasS24LE4 = false, hasS24NE4 = false,
    hasS24BE3 = false, hasS24LE3 = false, hasS24NE3 = false, hasDouble = false, hasFloat = false,
    hasAAC = false, hasAC3 = false, hasDTS = false, hasEAC3 = false, hasTRUEHD = false, hasDTSHD = false, hasLPCM = false;

  for(AEDataFormatList::const_iterator iter = m_dataFormats.cbegin(); iter != m_dataFormats.cend(); iter++)
  {
    switch(*iter)
    {
    case AE_FMT_U8:     hasU8     = true; break;
    case AE_FMT_S8:     hasS8     = true; break;
    case AE_FMT_S16BE:  hasS16BE  = true; break;
    case AE_FMT_S16LE:  hasS16LE  = true; break;
    case AE_FMT_S16NE:  hasS16NE  = true; break;
    case AE_FMT_S32BE:  hasS32BE  = true; break;
    case AE_FMT_S32LE:  hasS32LE  = true; break;
    case AE_FMT_S32NE:  hasS32NE  = true; break;
    case AE_FMT_S24BE4: hasS24BE4 = true; break;
    case AE_FMT_S24LE4: hasS24LE4 = true; break;
    case AE_FMT_S24NE4: hasS24NE4 = true; break;
    case AE_FMT_S24BE3: hasS24BE3 = true; break;
    case AE_FMT_S24LE3: hasS24LE3 = true; break;
    case AE_FMT_S24NE3: hasS24NE3 = true; break;
    case AE_FMT_DOUBLE: hasDouble = true; break;
    case AE_FMT_FLOAT:  hasFloat  = true; break;
    case AE_FMT_AAC:    hasAAC    = true; break;
    case AE_FMT_AC3:    hasAC3    = true; break;
    case AE_FMT_DTS:    hasDTS    = true; break;
    case AE_FMT_EAC3:   hasEAC3   = true; break;
    case AE_FMT_TRUEHD: hasTRUEHD = true; break;
    case AE_FMT_DTSHD:  hasDTSHD  = true; break;
    case AE_FMT_LPCM:   hasLPCM   = true; break;
    default:
      ASSERT(0); // Wrong data format!
      break;
    }
  }
  m_dataFormats.clear();

  if(hasU8)
    m_dataFormats.push_back(AE_FMT_U8);
  if(hasS8)
    m_dataFormats.push_back(AE_FMT_S8);
  if(hasS16BE)
    m_dataFormats.push_back(AE_FMT_S16LE);
  if(hasS16LE)
    m_dataFormats.push_back(AE_FMT_S16BE);
  if(hasS16NE)
    m_dataFormats.push_back(AE_FMT_S16NE);
  if(hasS24BE3)
    m_dataFormats.push_back(AE_FMT_S24BE3);
  if(hasS24LE3)
    m_dataFormats.push_back(AE_FMT_S24LE3);
  if(hasS24NE3)
    m_dataFormats.push_back(AE_FMT_S24NE3);
  if(hasS24BE4)
    m_dataFormats.push_back(AE_FMT_S24BE4);
  if(hasS24LE4)
    m_dataFormats.push_back(AE_FMT_S24LE4);
  if(hasS24NE4)
    m_dataFormats.push_back(AE_FMT_S24NE4);
  if(hasS32BE)
    m_dataFormats.push_back(AE_FMT_S32BE);
  if(hasS32LE)
    m_dataFormats.push_back(AE_FMT_S32LE);
  if(hasS32NE)
    m_dataFormats.push_back(AE_FMT_S32NE);
  if(hasFloat)
    m_dataFormats.push_back(AE_FMT_FLOAT);
  if(hasDouble)
    m_dataFormats.push_back(AE_FMT_DOUBLE);
  if(hasAAC)
    m_dataFormats.push_back(AE_FMT_AAC);
  if(hasAC3)
    m_dataFormats.push_back(AE_FMT_AC3);
  if(hasDTS)
    m_dataFormats.push_back(AE_FMT_DTS);
  if(hasEAC3)
    m_dataFormats.push_back(AE_FMT_EAC3);
  if(hasDTSHD)
    m_dataFormats.push_back(AE_FMT_DTSHD);
  if(hasTRUEHD)
    m_dataFormats.push_back(AE_FMT_TRUEHD);
  if(hasLPCM)
    m_dataFormats.push_back(AE_FMT_LPCM);
}

CAEDeviceInfo::CAEDeviceInfo(AESinkType sinkType) :
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

CAEDeviceInfo::CAEDeviceInfo(const CAEDeviceInfo& deviceInfo) :
  m_sinkType(deviceInfo.m_sinkType)
{
  m_deviceName        = deviceInfo.m_deviceName;	
  m_displayName       = deviceInfo.m_displayName;
  m_displayNameExtra  = deviceInfo.m_displayNameExtra;
  m_deviceType        = deviceInfo.m_deviceType;
  m_channelsFormats   = deviceInfo.m_channelsFormats;	  
  m_sampleRates       = deviceInfo.m_sampleRates;
  m_dataFormats       = deviceInfo.m_dataFormats;
  SortLists();
  if (deviceInfo.m_ptrSinkSpecificInfo)
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
    switch(m_sinkType)
    {
    case AE_SINK_NULL:
    case AE_SINK_PROFILER:
      m_ptrSinkSpecificInfo = NULL;
      return;
    /* Just a precaution, shouldn't be used in real life */
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

CAEDeviceInfo::CAEDeviceInfo(CAEDeviceInfo&& deviceInfo) :
  m_sinkType(deviceInfo.m_sinkType)
{
  m_deviceName          = deviceInfo.m_deviceName;	
  m_displayName         = deviceInfo.m_displayName;
  m_displayNameExtra    = deviceInfo.m_displayNameExtra;
  m_deviceType          = deviceInfo.m_deviceType;
  m_channelsFormats.swap( deviceInfo.m_channelsFormats);	  
  m_sampleRates.swap(     deviceInfo.m_sampleRates);
  m_dataFormats.swap(     deviceInfo.m_dataFormats);
  SortLists();
  if (deviceInfo.m_ptrSinkSpecificInfo)
  {
    switch(m_sinkType)
    {
    case AE_SINK_NULL:
    case AE_SINK_PROFILER:
      m_ptrSinkSpecificInfo = NULL;
      return;
    case AE_SINK_WASAPI:
    case AE_SINK_DIRECTSOUND:
    case AE_SINK_AUDIOTRACK:
    case AE_SINK_ALSA:
    case AE_SINK_OSS:
      m_ptrSinkSpecificInfo = deviceInfo.m_ptrSinkSpecificInfo;
      deviceInfo.m_ptrSinkSpecificInfo = NULL;
      return;
    }
    /* Should be unreachable */
    ASSERT(0);
  }
  else
  {
    switch(m_sinkType)
    {
    case AE_SINK_NULL:
    case AE_SINK_PROFILER:
      m_ptrSinkSpecificInfo = NULL;
      return;
    /* Just a precaution, shouldn't be used in real life */
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
