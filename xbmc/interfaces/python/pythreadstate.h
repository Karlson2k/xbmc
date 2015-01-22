#pragma once

/*
 *      Copyright (C) 2005-2013 Team XBMC
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

#include <Python.h>
#include "threads/SingleLock.h"

//WARNING: since this will unlock/lock the python global interpreter lock,
//         it will not work recursively

//this is basically a scoped version of a Py_BEGIN_ALLOW_THREADS .. Py_END_ALLOW_THREADS block
class CPyThreadState
{
  public:
    CPyThreadState(bool allowOtherPyThreads = true) : m_threadState(NULL)
    {
      if (allowOtherPyThreads)
        BeginAllowOtherPyThreads();
    }

    ~CPyThreadState()
    {
      EndAllowOtherPyThreads();
    }

    void BeginAllowOtherPyThreads()
    {
      if (!m_threadState)
        m_threadState = PyEval_SaveThread(); //same as Py_BEGIN_ALLOW_THREADS
    }

    void EndAllowOtherPyThreads()
    {
      if (m_threadState)
      {
        PyEval_RestoreThread(m_threadState); //same as Py_END_ALLOW_THREADS
        m_threadState = NULL;
      }
    }

  private:
    PyThreadState* m_threadState;
};

