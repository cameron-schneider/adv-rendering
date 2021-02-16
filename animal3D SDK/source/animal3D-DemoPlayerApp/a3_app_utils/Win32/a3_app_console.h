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
	
	a3_app_console.h
	Interface for creating a console to redirect output from stdio.

	**DO NOT MODIFY THIS FILE**
*/

#if (defined _WINDOWS || defined _WIN32)
#ifndef __ANIMAL3D_APP_CONSOLE_H
#define __ANIMAL3D_APP_CONSOLE_H


#include "a3types_integer.h"


//-----------------------------------------------------------------------------

// A3: Print text to Visual Studio's output window using printf's syntax.
//	return: number of characters printed (max 255)
//	return: -1 if invalid params (null pointer)
a3ret a3printf_vsdebug(const a3byte *const format, ...);

// A3: Create console with write access; stdio's printf will display here.
//	return: 1 if success (console created)
//	return: 0 if fail
a3ret a3stdoutConsoleCreate();

// A3: Release console created using above function.
//	return: 1 if success (console released)
//	return: 0 if fail
a3ret a3stdoutConsoleRelease();

// A3: Check if console window is open.
//	return: 1 if console is open
//	return: 0 if console is not open
a3ret a3stdoutConsoleIsOpen();

// A3: Clear console.
//	return: 1 if success
//	return: 0 if fail
a3ret a3stdoutConsoleClear();


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_APP_CONSOLE_H
#endif	// (defined _WINDOWS || defined _WIN32)