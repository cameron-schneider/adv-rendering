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
	
	a3_app_console.c
	Implementation for console.

	**DO NOT MODIFY THIS FILE**
*/

#if (defined _WINDOWS || defined _WIN32)

#include "a3_app_utils/Win32/a3_app_console.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>


//-----------------------------------------------------------------------------
// printing utilities (debug only)

// print something to Visual Studio debugger output
a3ret a3printf_vsdebug(const a3byte *const format, ...)
{
	a3i32 ret = -1;
#if A3_DEBUG
	static a3byte buff[256];
	if (format)
	{
		// copy formatted arguments to a string
		va_list args;
		va_start(args, format);
		ret = _vsnprintf(buff, 256, format, args);
		buff[ret] = 0;
		va_end(args);

		// output string to VS debug console
		OutputDebugStringA(buff);
	}
#endif	// A3_DEBUG
	return ret;
}


// allocate and deallocate console window
a3ret a3stdoutConsoleCreate()
{
	a3i32 status = 0;
#if A3_DEBUG
	HANDLE handle = GetConsoleWindow();
	if (!handle)
	{
		status = AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}
#endif	// A3_DEBUG
	return status;
}

a3ret a3stdoutConsoleRelease()
{
#if A3_DEBUG
	return FreeConsole();
#else	// !A3_DEBUG
	return 0;
#endif	// A3_DEBUG
}

a3ret a3stdoutConsoleIsOpen()
{
#if A3_DEBUG
	HANDLE handle = GetConsoleWindow();
	return (handle != 0);
#else	// !A3_DEBUG
	return 0;
#endif	// A3_DEBUG
}

a3ret a3stdoutConsoleClear()
{
	a3i32 status = 0;
#if A3_DEBUG
	status = a3stdoutConsoleIsOpen();
	if (status)
		system("cls");
#endif	// A3_DEBUG
	return status;
}


//-----------------------------------------------------------------------------


#endif	// (defined _WINDOWS || defined _WIN32)