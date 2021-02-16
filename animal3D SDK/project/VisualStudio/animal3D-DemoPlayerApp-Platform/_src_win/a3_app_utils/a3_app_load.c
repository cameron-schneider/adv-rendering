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
	
	a3_app_load.c
	Implementation of demo library loading/unloading/hot build/load.

	**DO NOT MODIFY THIS FILE**
*/


#include <stdio.h>
#include <stdlib.h>

#include "a3_app_utils/a3_app_load.h"
#include "a3_dylib_config_load.h"


//-----------------------------------------------------------------------------
// helper functions

// return buffer at the end of the function
inline a3byte *a3scan(a3byte *text_out, a3byte *buffer, const a3byte begin, const a3byte end)
{
	// scan until begin character
	while (*buffer && *buffer != begin)
		++buffer;
	++buffer;

	// scan and copy until end character
	while (*buffer && *buffer != end)
		*(text_out++) = *(buffer++);
	++buffer;
	*text_out = 0;

	// done
	return buffer;
}


// compare hook names, return pointer to callback
// return null if unknown hook
inline void **a3getCallbackHook(const a3byte *hook, a3_CallbackFunctions *callbacks)
{
	static const a3byte *hookNames[] = {
		"a3callback_load",
		"a3callback_unload",
		"a3callback_display",
		"a3callback_idle",
		"a3callback_windowActivate",
		"a3callback_windowDeactivate",
		"a3callback_windowMove",
		"a3callback_windowResize",
		"a3callback_keyPress",
		"a3callback_keyHold",
		"a3callback_keyRelease",
		"a3callback_keyCharPress",
		"a3callback_keyCharHold",
		"a3callback_mouseClick",
		"a3callback_mouseDoubleClick",
		"a3callback_mouseRelease",
		"a3callback_mouseWheel",
		"a3callback_mouseMove",
		"a3callback_mouseLeave",
	};
	const a3count hookNamesSize = sizeof(hookNames) / sizeof(a3byte *);

	// compare strings with provided hook
	a3index i;
	for (i = 0; i < hookNamesSize; ++i)
	{
		if (!strcmp(hook, hookNames[i]))
			return &(callbacks->callback[i]);
	}
	return 0;
}


//-----------------------------------------------------------------------------
// dummy callback functions

void a3appDummy_voidFunc_ptr(void *p) {}
void a3appDummy_voidFunc_ptrInt(void *p, a3i32 i) {};
void a3appDummy_voidFunc_ptrIntInt(void *p, a3i32 i0, a3i32 i1) {};
void a3appDummy_voidFunc_ptrIntIntInt(void *p, a3i32 i0, a3i32 i1, a3i32 i2) {};
void *a3appDummy_ptrFunc_ptrInt(void *p, a3i32 i) { return p; };
a3ret a3appDummy_intFunc_ptr(void *p) { return 0; }

// set all callbacks in a window structure to the respective dummy functions
a3ret a3appSetDummyCallbacks(a3_CallbackFunctions *callbacks)
{
	if (callbacks)
	{
		callbacks->callback_load
			= callbacks->callback_unload = a3appDummy_ptrFunc_ptrInt;
		callbacks->callback_display
			= callbacks->callback_idle = a3appDummy_intFunc_ptr;
		callbacks->callback_windowActivate
			= callbacks->callback_windowDeactivate = a3appDummy_voidFunc_ptr;
		callbacks->callback_windowMove
			= callbacks->callback_windowResize = a3appDummy_voidFunc_ptrIntInt;
		callbacks->callback_keyPress
			= callbacks->callback_keyHold
			= callbacks->callback_keyRelease
			= callbacks->callback_keyCharPress
			= callbacks->callback_keyCharHold = a3appDummy_voidFunc_ptrInt;
		callbacks->callback_mouseClick
			= callbacks->callback_mouseDoubleClick
			= callbacks->callback_mouseRelease = a3appDummy_voidFunc_ptrIntIntInt;
		callbacks->callback_mouseWheel = a3appDummy_voidFunc_ptrIntIntInt;
		callbacks->callback_mouseMove = a3appDummy_voidFunc_ptrIntInt;
		callbacks->callback_mouseLeave = a3appDummy_voidFunc_ptr;

		// done
		return 1;
	}
	return -1;
}


// allocate and correctly initialize demo info descriptors
a3ret a3appAllocDemoInfo(a3_DemoDescriptor **records_out, const a3ui32 count)
{
	const a3ui32 sz = count * sizeof(a3_DemoDescriptor);
	if (records_out && !*records_out && count)
	{
		*records_out = (a3_DemoDescriptor *)malloc(sz);
		memset(*records_out, 0, sz);
		return count;
	}
	return -1;
}

// load descriptors
a3ret a3appLoadDemoInfo(a3_DemoDescriptor **records_out, const a3byte *filePath, a3ui32 numRecordsDefined)
{
	if (records_out && filePath)
	{
		a3byte line[2048] = { 0 }, *linePtr;
		a3ui32 numRecordsLoaded = 0;
		a3ui32 i;
		a3_DemoDescriptor tmp;
		FILE *fp;

		if (*records_out && !numRecordsDefined)
		{
			a3appReleaseDemoInfo(records_out);
			*records_out = 0;
		}

		// open file
		fp = fopen(filePath, "r");
		if (fp)
		{
			while (linePtr = fgets(line, 1023, fp))
			{
				// skip all comments
				if (*linePtr != '#')
				{
					// scan for @ which marks start of data entry
					while (*linePtr && *linePtr != '@')
						++linePtr;

					// either end of line or entry, if entry do something
					if (*(linePtr++))
					{
						// if no records have been found yet, 
						//	the first value describes this
						if (numRecordsDefined <= 0)
						{
							sscanf(linePtr, "%d", &numRecordsDefined);

							// allocate space
							a3appAllocDemoInfo(records_out, numRecordsDefined);
						}

						// if we are searching for records, 
						//	parse record line
						else if (numRecordsLoaded < numRecordsDefined)
						{
							// scan for demo name
							linePtr = a3scan(tmp.name, linePtr, '[', ']');

							// scan for library file
							linePtr = a3scan(tmp.lib, linePtr, '[', ']');

							// scan for callback count
							sscanf(linePtr, "%d", &tmp.numCallbacks);
						
							// scan for function mappings
							for (i = 0; i < tmp.numCallbacks; ++i)
							{
								while (*(linePtr++) != ':');
								sscanf(linePtr, "%s %s", tmp.callbackHook[i], tmp.callbackName[i]);
							}
							(*records_out)[numRecordsLoaded++] = tmp;
						}

						// we are done
						else
							break;
					}
				}
			}

			// done
			fclose(fp);
			return numRecordsLoaded;
		}
	}
	return -1;
}

// unload descriptors
a3ret a3appReleaseDemoInfo(a3_DemoDescriptor **records)
{
	if (records && *records)
	{
		free(*records);
		*records = 0;
		return 1;
	}
	return -1;
}


// load and link callbacks
a3ret a3appLoadCallbacks(a3_DemoWindowInterface *demoWindow, const a3_DemoDescriptor *record)
{
	if (demoWindow && !demoWindow->handle && record)
	{
		a3byte libPath[64] = "./animal3D-demos/";
		a3byte *lib = strcat(libPath, record->lib);
		A3DYLIBHANDLE libHandle = a3dylibLoad(libPath);
		A3DYLIBSYMBOL *hookPtr;

		a3i32 callbacksLoaded = 0;
		a3ui32 i;

		if (libHandle)
		{
			// match hooks in the descriptor to the correct callback
			// load callback that goes by the provided name
			for (i = 0; i < record->numCallbacks; ++i)
			{
				hookPtr = (A3DYLIBSYMBOL *)a3getCallbackHook(record->callbackHook[i], demoWindow->callbacks);
				if (hookPtr)
				{
					*hookPtr = a3dylibGetSymbol(libHandle, record->callbackName[i]);
					if (*hookPtr)
						++callbacksLoaded;
					else
						printf("\n A3 ERROR: Failed to load callback: %s", record->callbackName[i]);
				}
				else
					printf("\n A3 ERROR: Unrecognized callback type: %s", record->callbackHook[i]);
			}

			// done
			demoWindow->handle = libHandle;
			return callbacksLoaded;
		}
		else
		{
			printf("\n A3 ERROR: Failed to load library: %s", libPath);
			return 0;
		}
	}
	return -1;
}

a3ret a3appReleaseCallbacks(a3_DemoWindowInterface *demoWindow)
{
	if (demoWindow && demoWindow->handle)
	{
		a3i32 status = a3dylibUnload((A3DYLIBHANDLE)demoWindow->handle);
		if (status)
		{
			demoWindow->handle = 0;
			a3appSetDummyCallbacks(demoWindow->callbacks);
		}
		return status;
	}
	return -1;
}


//-----------------------------------------------------------------------------
