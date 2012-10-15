#pragma once
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

#include <string>
#include <vector>
#include "AEAudioFormat.h"
#include "Utils/AEChannelInfo.h"

#if defined(TARGET_WINDOWS)
  class CWASAPISpecificDeviceInfo; 
  class CDirectSoundSpecificDeviceInfo;
#elif defined(TARGET_ANDROID)
  class CAUDIOTRACKSpecificDeviceInfo;
#elif defined(TARGET_LINUX) || defined(TARGET_FREEBSD)
  #if defined(HAS_ALSA)
    class CALSASpecificDeviceInfo;
  #endif
  class COSSSpecificDeviceInfo;
#endif

typedef std::vector<unsigned int     > AESampleRateList;
typedef std::vector<enum AEDataFormat> AEDataFormatList;
typedef std::vector<CAEChannelInfo   > AEChannelInfoList;

enum AESinkType {
  AE_SINK_NULL,
  AE_SINK_PROFILER,
  AE_SINK_WASAPI,
  AE_SINK_DIRECTSOUND,
  AE_SINK_ALSA,
  AE_SINK_OSS,
  AE_SINK_AUDIOTRACK
};

enum AEDeviceType {
  AE_DEVTYPE_PCM,
  AE_DEVTYPE_IEC958,
  AE_DEVTYPE_HDMI,
  AE_DEVTYPE_DP
};

/**
 * Sinks must store specific to sink device information in derived class
 * Information can be modified on each sink call and preserved between sink calls
 */
class CSinkSpecificDeviceInfo
{
public:
  CSinkSpecificDeviceInfo() { }
  virtual ~CSinkSpecificDeviceInfo() { }
  virtual AESinkType GetSinkType() = 0;
};

/**
 * This classt provides the details of what the audio output hardware is capable of
 */
class CAEDeviceInfo
{
public:
  const AESinkType  m_sinkType;         /* type of sink */
  std::string       m_deviceName;	    /* the driver device name */
  std::string       m_displayName;	    /* the friendly display name */
  std::string       m_displayNameExtra;	/* additional display name info, ie, monitor name from ELD */
  enum AEDeviceType m_deviceType;	    /* the device type, PCM, IEC958 or HDMI */
  AEChannelInfoList m_channelsFormats;  /* the channels the device is capable of rendering */
  AESampleRateList  m_sampleRates;	    /* the basic PCM samplerates the device is capable of rendering */
  AEDataFormatList  m_dataFormats;	    /* the dataformats the device is capable of rendering */

  bool SupportsRaw() const;             /* returns ability to support RAW formats. m_dataFormats must be sorted!*/
  void SortLists();                     /* sort vector members. Called automatically when CAEDeviceInfo object is pushed to vector */
  std::string GetAEDeviceName() const;  

#if defined(TARGET_WINDOWS)
  CWASAPISpecificDeviceInfo&      WASAPIDeviceInfo();       /* access to WASAPI specific device information */
  CDirectSoundSpecificDeviceInfo& DirectSoundDeviceInfo();  /* access to DirectSound specific device information */
#elif defined(TARGET_ANDROID)
  CAUDIOTRACKSpecificDeviceInfo&  AUDIOTRACKDeviceInfo();   /* access to AUDIOTRACK specific device information */
#elif defined(TARGET_LINUX) || defined(TARGET_FREEBSD)
#if defined(HAS_ALSA)
  CALSASpecificDeviceInfo&        ALSADeviceInfo();         /* access to ALSA specific device information */
#endif
  COSSSpecificDeviceInfo&         OSSDeviceInfo();          /* access to OSS specific device information */
#endif

  CAEDeviceInfo(AESinkType sinkType);
  CAEDeviceInfo(const CAEDeviceInfo& deviceInfo);
  CAEDeviceInfo(CAEDeviceInfo&& deviceInfo);
  ~CAEDeviceInfo();
  operator std::string();
  static std::string DeviceTypeToString(enum AEDeviceType deviceType);
  static std::string SinkTypeToString(enum AESinkType sinkType);
private:
  CSinkSpecificDeviceInfo* m_ptrSinkSpecificInfo;
};

typedef std::vector<CAEDeviceInfo> AEDeviceInfoList;
