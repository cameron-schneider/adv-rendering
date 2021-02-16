/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_app_application.c
	Implementation for window creation and event loop.

	**DO NOT MODIFY THIS FILE**
*/

#if (defined _WINDOWS || defined _WIN32)
#include "a3_app_utils/Win32/a3_app_application.h"


#include <stdlib.h>


//-----------------------------------------------------------------------------
// application management

// if an instance of this application is already running, switch to it
a3ret a3appStartSingleInstanceSwitchExisting(const a3byte *windowClassName, const a3byte *windowName)
{
	if (windowClassName || windowName)
	{
		HWND existing = FindWindowA(windowClassName, windowName);
		if (existing)
		{
			// set foreground window to the existing one, quit this instance
			SetForegroundWindow(existing);
			exit(0);
		}
		return 1;
	}
	return -1;
}

// do not start if an instance of this application is running
a3ret a3appStartSingleInstance(HANDLE *handle_out, const a3byte *instanceName)
{
	if (handle_out && instanceName)
	{
		if (!*handle_out)
		{
			*handle_out = CreateMutexA(0, 0, instanceName);
			if (*handle_out && GetLastError() == ERROR_ALREADY_EXISTS)
			{
				// do not switch, quit
				exit(0);
			}
			return 1;
		}
		return 0;
	}
	return -1;
}

// start counted instance of the app
a3ret a3appStartMultipleInstance(HANDLE *handle_out, const a3byte *instanceName, const a3ui32 limit)
{
	if (handle_out && instanceName && limit)
	{
		if (!*handle_out)
		{
			*handle_out = CreateSemaphoreA(0, limit, limit, instanceName);
			if (*handle_out && WaitForSingleObject(*handle_out, 0) == WAIT_TIMEOUT)
			{
				// too many instances, quit
				exit(0);
			}
			return 1;
		}
		return 0;
	}
	return -1;
}

// stop single instance
a3ret a3appStopSingleInstance(HANDLE *handle)
{
	if (handle)
	{
		if (*handle)
		{
			ReleaseMutex(*handle);
			CloseHandle(*handle);
			return 1;
		}
		return 0;
	}
	return -1;
}

// stop multiple instance
a3ret a3appStopMultipleInstance(HANDLE *handle)
{
	if (handle)
	{
		if (*handle)
		{
			ReleaseSemaphore(*handle, 1, 0);
			CloseHandle(*handle);
			return 1;
		}
		return 0;
	}
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// (defined _WINDOWS || defined _WIN32)