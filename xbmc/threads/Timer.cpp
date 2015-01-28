/*
 *      Copyright (C) 2012-2013 Team XBMC
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

#include <algorithm>

#include "Timer.h"
#include "SystemClock.h"

CTimer::CTimer(ITimerCallback *callback)
  : CThread("Timer"),
    m_timeOut(),
    m_callback(callback),
    m_interval(false)
{ }

CTimer::~CTimer()
{
  Stop(true);
}

bool CTimer::Start(uint32_t timeout, bool interval /* = false */)
{
  if (m_callback == NULL || timeout == 0 || IsRunning())
    return false;

  m_timeOut.Set(timeout);
  m_interval = interval;

  Create();
  return true;
}

bool CTimer::Stop(bool wait /* = false */)
{
  if (!IsRunning())
    return false;

  m_bStop = true;
  m_eventTimeout.Set();
  StopThread(wait);

  return true;
}

bool CTimer::Restart()
{
  if (!IsRunning())
    return false;

  Stop(true);
  return Start((uint32_t)m_timeOut.GetInitialTimeoutValue(), m_interval);
}

float CTimer::GetElapsedSeconds() const
{
  return GetElapsedMilliseconds() / 1000.0f;
}

float CTimer::GetElapsedMilliseconds() const
{
  if (!IsRunning())
    return 0.0f;

  return (float)(m_timeOut.GetElapsedMilliseconds());
}

void CTimer::Process()
{
  while (!m_bStop)
  {
    // wait the necessary time
    if (!m_eventTimeout.WaitMSec(m_timeOut.MillisLeft()))
    {
      if (m_timeOut.IsTimePast())
      {
        // execute OnTimeout() callback
        m_callback->OnTimeout();

        // stop if this is not an interval timer
        if (!m_interval)
          break;

        m_timeOut.Restart();
      }
    }
  }
}