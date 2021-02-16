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
	
	a3_app_window.c
	Implementation for window creation and event loop.

	**DO NOT MODIFY THIS FILE**

	I designed this system entirely out of my own imagination, but later 
	found advanced tutorials and people working on similar problems.
	The A3 implementation is actually quite simple compared to others.

	Advanced C++ tutorial example: http://www.codinglabs.net/tutorial_CppRuntimeCodeReload.aspx
	Preprocessor tips: https://stackoverflow.com/questions/48514341/how-to-escape-backslash-in-solutiondir-in-a-vs-project-file
*/

#if (defined _WINDOWS || defined _WIN32)

#include "a3_app_utils/Win32/a3_app_window.h"
#include "a3_app_utils/Win32/a3_app_console.h"

#include <stdio.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------

#if A3_DEBUG
// override printf so that it doesn't do anything
#define printf(fmt, ...) (0)

// demo project name, used in hotbuild
#define A3DEMOPLUGINNAME "animal3D-DemoPlugin"
#endif	// A3_DEBUG


//-----------------------------------------------------------------------------
// internal and external windowing utility declarations

// window message processing routine prototype
LRESULT CALLBACK a3windowInternalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// pixel format selector
extern a3i32 a3rendererInternalChooseDefaultPixelFormat(const a3i32 flag, const HDC dc);

// check if context is current
extern a3boolean a3rendererInternalContextIsCurrent(const a3_RenderingContext renderingContext);

// set render context
extern void a3rendererInternalSetContext(HDC deviceContext, a3_RenderingContext renderingContext);

// set vertical sync
extern void a3rendererInternalSetVsync(a3i32 interval);


//-----------------------------------------------------------------------------
// windowing utilities

// custom switch constants
enum A3_MESSAGE
{
	A3_MESSAGE_BEGIN = WM_USER, 
	A3_HOTLOAD_COMPLETE,
	A3_USER_EXIT_DEMO,

	A3_MENU_ITEM = 1000, 
};

// menu item switch constants
enum A3_MENU_ITEM_MESSAGE
{
	A3_MENU_IEM_BEGIN = A3_MENU_ITEM,
	A3_MENU_EXIT_PROG,
	A3_MENU_EXIT_DEMO,
	A3_MENU_RELOAD_DEMO,
	A3_MENU_RELOAD_MENU,
	A3_MENU_TOGGLE_CONSOLE,
	A3_MENU_PRINT_TO_CONSOLE,
	A3_MENU_PRINT_RENDERER,
	A3_MENU_CLEAR_CONSOLE,
	A3_MENU_LOAD_DEBUG,
	A3_MENU_BUILD_DEBUG,
	A3_MENU_REBUILD_DEBUG,
	A3_MENU_DEMO_LIST,
};


//-----------------------------------------------------------------------------
// internal menu creation function

// getter/setter for active console
a3boolean a3windowInternalSetConsoleToggle(a3boolean set, a3boolean setValue)
{
	static a3boolean toggleConsoleActive = 1;
	if (set)
		toggleConsoleActive = setValue;
	return toggleConsoleActive;
}

// getter/setter for printing to console
a3boolean a3windowInternalSetConsolePrint(a3boolean set, a3boolean setValue)
{
	static a3boolean printConsoleActive = 1;
	if (set)
		printConsoleActive = setValue;
	return printConsoleActive;
}

// getter for active console
inline a3boolean a3windowInternalGetConsoleToggle()
{
	return a3windowInternalSetConsoleToggle(0, 0);
}

// getter for printing to console
inline a3boolean a3windowInternalGetConsolePrint()
{
	return a3windowInternalSetConsolePrint(0, 0);
}


// create menu for window
a3ret a3windowInternalCreateMenu(a3_WindowInterface *window, const a3_DemoDescriptor *demoRecords, const a3ui32 numRecords)
{
	a3ui32 buttonID, loadMenuButtonFlags, i;
	a3ui32 greyConsoleOption = (a3windowInternalGetConsoleToggle() ? 0 : MF_GRAYED);
	HMENU menu = window->windowMenuHandle = CreateMenu();
	HMENU menu_main = window->fileMenuHandle = CreateMenu(),
		menu_utils = CreatePopupMenu(),
		menu_demos = CreatePopupMenu(),
		menu_hotbuild = CreatePopupMenu();

	// populate demos sub-menu
	if (demoRecords && numRecords)
	{
		for (i = 0, buttonID = A3_MENU_DEMO_LIST; i < numRecords; ++i, ++buttonID)
		{
			loadMenuButtonFlags = demoRecords[i].numCallbacks ? MF_STRING : MF_STRING | MF_GRAYED;
			AppendMenuA(menu_demos, loadMenuButtonFlags, buttonID, demoRecords[i].name);
		}

		// flag for new menu
		loadMenuButtonFlags = (MF_POPUP | MF_STRING);
	}
	else
		loadMenuButtonFlags = (MF_POPUP | MF_STRING | MF_GRAYED);

	// populate utilities sub-menu
	AppendMenuA(menu_utils, MF_STRING, A3_MENU_RELOAD_MENU, "Update loadable demo list");

	// add other buttons
#if A3_DEBUG
	// debugging only: add or remove console, enable window print-to-console
	AppendMenuA(menu_utils, MF_MENUBREAK, 0, 0);
	AppendMenuA(menu_utils,
		MF_STRING | (a3windowInternalGetConsoleToggle() ? MF_CHECKED : MF_UNCHECKED), 
		A3_MENU_TOGGLE_CONSOLE, "DEBUG: Toggle output-only console");
	AppendMenuA(menu_utils,
		MF_STRING | greyConsoleOption | (a3windowInternalGetConsolePrint() ? MF_CHECKED : MF_UNCHECKED),
		A3_MENU_PRINT_TO_CONSOLE, "DEBUG: Window prints to console");
	AppendMenuA(menu_utils,
		MF_STRING | greyConsoleOption, A3_MENU_PRINT_RENDERER, "DEBUG: Print renderer info");
	AppendMenuA(menu_utils,
		MF_STRING | greyConsoleOption, A3_MENU_CLEAR_CONSOLE, "DEBUG: Clear console");

	// debugging only: load active debug library
	AppendMenuA(menu_hotbuild, MF_STRING, A3_MENU_LOAD_DEBUG, "Load without building");
	AppendMenuA(menu_hotbuild, MF_STRING, A3_MENU_BUILD_DEBUG, "Quick build && load");
	AppendMenuA(menu_hotbuild, MF_STRING, A3_MENU_REBUILD_DEBUG, "Full rebuild && load");
#endif	// A3_DEBUG

	// add sub-menus to main menu
	AppendMenuA(menu_main, MF_POPUP | MF_STRING, (UINT_PTR)menu_utils, "Window utilities...");
	AppendMenuA(menu_main, loadMenuButtonFlags, (UINT_PTR)menu_demos, "Load demo...");

#if A3_DEBUG
	// debugging only: add hot build and load sub-menu to main menu
	AppendMenuA(menu_main, MF_MENUBREAK, 0, 0);
	AppendMenuA(menu_main, MF_POPUP | MF_STRING, (UINT_PTR)menu_hotbuild, "DEBUG: Demo project hot build && load...");
#endif	// A3_DEBUG

	// other options
	AppendMenuA(menu_main, MF_MENUBREAK, 0, 0);
	AppendMenuA(menu_main, MF_STRING, A3_MENU_RELOAD_DEMO, "Reload current demo");
	AppendMenuA(menu_main, MF_STRING, A3_MENU_EXIT_DEMO, "Exit current demo");
	AppendMenuA(menu_main, MF_STRING, A3_MENU_EXIT_PROG, "Exit program");

	// add main menu to file button
	AppendMenuA(menu, (MF_POPUP | MF_STRING), (UINT_PTR)menu_main, "File");

	// done, set menu
	return SetMenu(window->windowHandle, menu);
}

// reload menu
a3ret a3windowInternalReloadMenu(a3_WindowInterface *wnd, a3_DemoWindowInterface *demo)
{
	// destroy menu
	DestroyMenu(wnd->windowMenuHandle);

	// load menu items again
	demo->numRecords = a3appLoadDemoInfo(&demo->records,
		"../../../../resource/animal3D-data/animal3D-demoinfo.txt", 0);

	// create menu with loaded items
	return a3windowInternalCreateMenu(wnd, demo->records, demo->numRecords);
}


// validate window menu
void a3windowInternalToggleMenu(a3_WindowInterface *window, a3boolean enable)
{
	if (enable)
	{
	//	EnableMenuItem(window->windowMenuHandle, (UINT)window->fileMenuHandle, MF_ENABLED);
		ModifyMenuA(window->windowMenuHandle, 0, (MF_BYPOSITION | MF_POPUP | MF_STRING), (UINT_PTR)window->fileMenuHandle, "File");
		DrawMenuBar(window->windowHandle);
	}
	else
	{
	//	EnableMenuItem(window->windowMenuHandle, (UINT)window->fileMenuHandle, MF_GRAYED);
		ModifyMenuA(window->windowMenuHandle, 0, (MF_BYPOSITION | MF_GRAYED), (UINT_PTR)window->fileMenuHandle, "File");
		DrawMenuBar(window->windowHandle);
	}
}


//-----------------------------------------------------------------------------
// internal hotloading functions

// prototype for pdb remover
a3ret a3windowInternalReleasePDB(const a3byte *sdkDirStr, const a3byte *cfgDirStr, const a3byte *projName);

// internal function for calling copy utility
a3ret a3windowInternalCopyDemo(a3_WindowInterface *window, a3_PrintFunc printFunc)
{
	a3i32 status = 0;

#if A3_DEBUG
	// prepare command string
	a3byte cmd[1024] = { 0 };

	// arguments
	const a3byte *utilPath = "..\\..\\..\\..\\utility\\win\\dev\\animal3d_win_copy_build.bat";
	const a3byte *buildConfig = window->env->cfgDirStr;

	// assemble command line
	//					util     libpath              binpath                libname    binname        datapath         |   bat   |     %1     |     %2     |        %3        |        %4        |     %5     |
	sprintf(cmd, "call \"%s\" \".\\lib%s\\\" \".\\bin%s\\animal3D-demos\\\" \"%s.dll\" \"%s.dll\" \".\\bin%s\\data\\\" ", utilPath, buildConfig, buildConfig, A3DEMOPLUGINNAME, A3DEMOPLUGINNAME, buildConfig);

	// call copy utility
	printFunc("\n----------------------------------------------------------------");
	printFunc("\n A3 copying and swapping new demo DLL...");
	printFunc("\n----------------------------------------------------------------\n");
	status = system(cmd);
	printFunc("\n----------------------------------------------------------------");
	printFunc("\n A3 copy and swap complete.");
	printFunc("\n----------------------------------------------------------------\n");

	// successful if status is zero
	status = (status == 0);
#endif	// A3_DEBUG

	// done
	return status;
}

// internal attempt to release PDB
a3ret a3windowInternalAttemptReleasePDB(a3_WindowInterface *window, a3_PrintFunc printFunc)
{
	a3i32 status = 0;

#if A3_DEBUG
	a3index i;
	const a3index pdbMaxAttempts = 100;

	// arguments
	const a3byte *buildPath = window->env->sdkDirStr;
	const a3byte *configPath = window->env->cfgDirStr;

	// attempt to remove PDB until it succeeds or a counter expires ("hand slap")
	printFunc("\n----------------------------------------------------------------");
	printFunc("\n A3 attempting to unlock and remove PDB...");
	printFunc("\n----------------------------------------------------------------\n");
	for (i = 0; i < pdbMaxAttempts && !status; ++i)
		status = a3windowInternalReleasePDB(buildPath, configPath, A3DEMOPLUGINNAME);
	printFunc("\n A3 PDB removal %s (%u/%u attempts). \n",
		status ? "succeeded" : "failed", i, pdbMaxAttempts);
	if (status && a3windowInternalReleasePDB(buildPath, configPath, A3DEMOPLUGINNAME))
		printFunc("\n A3 confirmed PDB removal. \n");
	printFunc("\n----------------------------------------------------------------");
	printFunc("\n A3 PDB removal complete.");
	printFunc("\n----------------------------------------------------------------\n");
#endif	// A3_DEBUG

	// done
	return status;
}

// internal compile and copy utility for hotloading
a3ret a3windowInternalBuildDemo(a3_WindowInterface *window, a3_PrintFunc printFunc, a3boolean fullRebuild)
{
	a3i32 status = 0;

#if A3_DEBUG
	// prepare command string
	a3byte cmd[1024] = { 0 };

	// arguments
	const a3byte *utilPath = "..\\..\\..\\..\\utility\\win\\dev\\animal3d_win_build_demo.bat";
	const a3byte *launchPath = window->env->launchPathStr;
	const a3byte *buildPath = window->env->slnPathStr;
	const a3byte *configPath = window->env->cfgDirStr;
	const a3byte *configType = (A3_DEBUG) ? "Debug" : "Release";
	const a3byte *configArch = (A3_ARCH == 64) ? "x64" : "x86";
	const a3byte *buildSwitch = fullRebuild ? "/Rebuild" : "/Build";

	// pdbrm
	a3windowInternalAttemptReleasePDB(window, printFunc);

	// assemble command line
	//					util  devenv                   buildpath                    config   switch  proj   nocopy  |   bat   |    %1     |                       %2                       |          %3           |     %4     |        %5        |
	buildPath = window->env->sdkDirStr;
	sprintf(cmd, "call \"%s\" \"%s\" \"%s\\project\\VisualStudio\\%s\\%s.vcxproj\" \"%s|%s\" \"%s\" \"%s\" \"YES\" ", utilPath, launchPath, buildPath, A3DEMOPLUGINNAME, A3DEMOPLUGINNAME, configType, configArch, buildSwitch, A3DEMOPLUGINNAME);

	// assemble command line v2
	//					util  devenv  build  config   switch  proj   nocopy  |   bat   |    %1     |    %2    |          %3           |     %4     |        %5        |
//	sprintf(cmd, "call \"%s\" \"%s\" \"%s\" \"%s|%s\" \"%s\" \"%s\" \"YES\" ", utilPath, launchPath, buildPath, configType, configArch, buildSwitch, A3DEMOPLUGINNAME);

	// call build utility
	printFunc("\n----------------------------------------------------------------");
	printFunc("\n A3 hot building demo project (%s, %s)...", buildSwitch, configPath);
	printFunc("\n----------------------------------------------------------------\n");
	status = system(cmd);
	printFunc("\n----------------------------------------------------------------");
	printFunc("\n A3 hot build complete.");
	printFunc("\n----------------------------------------------------------------\n");

	// successful if status is zero
	status = (status == 0);

	// send message that says we're done
	// pass status for both reload and copy flags
	// if the build failed, no point in doing anything
	PostMessageA(window->windowHandle, A3_HOTLOAD_COMPLETE, (WPARAM)(status), (LPARAM)(status));
#endif	// A3_DEBUG

	// done
	return status;
}

// thread launcher for demo build
a3i32 _stdcall a3windowInternalBuildDemoThreaded(a3_WindowInterface *wnd)
{
	return (a3i32)a3windowInternalBuildDemo(wnd, (a3windowInternalGetConsolePrint() ? printf : a3printf_vsdebug), 0);
}

// thread launcher for rebuild
a3i32 _stdcall a3windowInternalRebuildDemoThreaded(a3_WindowInterface *wnd)
{
	return (a3i32)a3windowInternalBuildDemo(wnd, (a3windowInternalGetConsolePrint() ? printf : a3printf_vsdebug), 1);
}


// load demo given menu ID
void a3windowInternalLoadDemo(a3_WindowInterface *window, a3i32 id)
{
	// PRE-LOAD TASKS: 
	// use hotloader interface to read and link functions
	a3i32 status = a3appLoadCallbacks(window->demo, (window->demo->records + id));
	if (status > 0)
	{
		// set ID
		window->demo->id = id;

		// callbacks: load, invoke window move, invoke resize
		window->demo->data = window->demo->callbacks->callback_load(window->demo->data, 0);
		window->demo->callbacks->callback_windowMove(window->demo->data, window->demo->winPosX, window->demo->winPosY);
		window->demo->callbacks->callback_windowResize(window->demo->data, window->demo->winSzX, window->demo->winSzY);
	}
}

// unload active demo
a3ret a3windowInternalUnloadDemo(a3_WindowInterface *window)
{
	a3i32 status;

	// unload callback
	window->demo->data = window->demo->callbacks->callback_unload(window->demo->data, 0);

	// use hotloader interface unlink functions and close library
	status = a3appReleaseCallbacks(window->demo);

	// copy ID to be returned, reset variables
	status = window->demo->id;
	window->demo->id = -1;
	window->demo->data = 0;

	// return ID of demo that was unloaded
	return status;
}

// hotload
void a3windowInternalHotloadDemo(a3_WindowInterface *window, a3boolean fullRebuild)
{
	HANDLE thread;

	// invalidate window menu
	a3windowInternalToggleMenu(window, 0);

	// launch thread to invoke compile tool
	if (fullRebuild >= 0)
	{
		// create dummy thread and pass window
		thread = CreateThread(0, 0, 
			fullRebuild ? a3windowInternalRebuildDemoThreaded : a3windowInternalBuildDemoThreaded, 
			window, 0, 0);
	}

	// if not compiling, unload and skip to complete
	else
	{
		// post complete message to reload
		// pass true for reload flag and false for copy flag
		PostMessageA(window->windowHandle, A3_HOTLOAD_COMPLETE, (WPARAM)(1), (LPARAM)(0));
	}
}

// done hotloading
void a3windowInternalHotloadDemoComplete(a3_WindowInterface *window, a3boolean reload, a3boolean copy)
{
	a3i32 status;
	a3_DemoDescriptor debug[1], *debugPtr = debug;

	// unload
	if (reload)
	{
		// soft unload call
		window->demo->data = window->demo->callbacks->callback_unload(window->demo->data, 1);

		// unload current callbacks
		status = a3appReleaseCallbacks(window->demo);
	}

	// copy output
	if (copy)
	{
		// copy output
		status = a3windowInternalCopyDemo(window, (a3windowInternalGetConsolePrint() ? printf : a3printf_vsdebug));
	}

	// reload demo
	if (reload)
	{
		// load debug info
		status = a3appLoadDemoInfo(&debugPtr,
			"../../../../resource/animal3D-data/animal3D-demoinfo-debug.txt", 1);

		// same pre-load tasks: read and link functions
		status = a3appLoadCallbacks(window->demo, debugPtr);
		if (status > 0)
		{
			// debug ID
			window->demo->id = -2;

			// callback
			// different from above because it should handle persistent data
			window->demo->data = window->demo->callbacks->callback_load(window->demo->data, 1);
			window->demo->callbacks->callback_windowMove(window->demo->data, window->demo->winPosX, window->demo->winPosY);
			window->demo->callbacks->callback_windowResize(window->demo->data, window->demo->winSzX, window->demo->winSzY);
		}
	}

	// re-enable file menu
	a3windowInternalToggleMenu(window, 1);
}


//-----------------------------------------------------------------------------
// public windowing functions

// initialize platform environment
a3ret a3windowInitPlatformEnvironment(a3_PlatformEnvironment* env_out, const a3byte* launchPathStr, const a3byte* slnPathStr, const a3byte* sdkDirStr, const a3byte* cfgDirStr)
{
	if (env_out && 
		launchPathStr && *launchPathStr && 
		slnPathStr && *slnPathStr && 
		sdkDirStr && *sdkDirStr && 
		cfgDirStr && *cfgDirStr)
	{
		env_out->launchPathStr = launchPathStr;
		env_out->slnPathStr = slnPathStr;
		env_out->sdkDirStr = sdkDirStr;
		env_out->cfgDirStr = cfgDirStr;

		// done
		return 1;
	}
	return -1;
}


// create window class with rendering capabilities
a3ret a3windowCreateDefaultRenderingClass(a3_WindowClass *windowClass_out, HINSTANCE appInstance, const a3byte *windowClassName, const a3ui32 extraBytes, const a3i32 iconID, const a3i32 smallIconID)
{
	if (windowClass_out && appInstance && windowClassName)
	{
		a3_WindowClass tmp[1] = { 0 };
		LPSTR useIconID = iconID >= 0 ? MAKEINTRESOURCEA(iconID) : MAKEINTRESOURCEA(32517);//IDI_WINLOGO;
		LPSTR useSmallIconID = smallIconID >= 0 ? MAKEINTRESOURCEA(smallIconID) : MAKEINTRESOURCEA(32517);//IDI_WINLOGO;

		// fill in properties of the class
		tmp->cbSize = sizeof(a3_WindowClass);
		tmp->style = (CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS);
		tmp->lpfnWndProc = a3windowInternalWndProc;
		tmp->cbClsExtra = 0;
		tmp->cbWndExtra = extraBytes;
		tmp->hInstance = appInstance;
		tmp->hIcon = LoadIconA(appInstance, useIconID);
		tmp->hCursor = LoadCursor(NULL, IDC_ARROW);
		tmp->hbrBackground = 0;
		tmp->lpszMenuName = NULL;
		tmp->lpszClassName = windowClassName;
		tmp->hIconSm = LoadIconA(appInstance, useSmallIconID);

		// register and return result
		if (RegisterClassExA(tmp))
		{
			// copy data and return pass
			*windowClass_out = *tmp;
			return 1;
		}

		// failed
		return 0;
	}
	return -1;
}

// release window class
a3ret a3windowReleaseClass(a3_WindowClass *windowClass, HINSTANCE appInstance)
{
	if (windowClass && appInstance)
	{
		a3i32 ret = UnregisterClassA(windowClass->lpszClassName, appInstance);
		if (ret)
			memset(windowClass, 0, sizeof(a3_WindowClass));
		return ret;
	}
	return -1;
}


// create window
a3ret a3windowCreate(a3_WindowInterface *window_out, a3_WindowClass *windowClass, a3_PlatformEnvironment *env, a3_RenderingContext *renderContext_opt, const a3byte *windowName, const a3ui32 winWidth, const a3ui32 winHeight, const a3boolean alwaysRender, const a3boolean standalone)
{
	if (window_out && windowClass && windowClass->hInstance && env)
	{
		const a3i32 hasParent = 0;
		const a3i32 isWindowed = 1;
		const a3i32 showCursor = 1;
		const a3i32 showWindow = SW_SHOW;
		const a3ui32 winPosX = 0, winPosY = 0;

		HWND wndHandle;
		DWORD dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		DWORD dwExStyle = 0;
		RECT rect, wndRect;

		if (!hasParent)
		{
			if (isWindowed)
			{
				rect.left = winPosX;
				rect.top = winPosY;
				rect.right = winPosX + winWidth;
				rect.bottom = winPosY + winHeight;
				dwExStyle |= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
				dwStyle |= WS_OVERLAPPEDWINDOW;
			}
			else
			{
				GetWindowRect(GetDesktopWindow(), &rect);
				dwExStyle |= WS_EX_APPWINDOW;
				dwStyle |= WS_POPUP;
			}

			// set window to rectangle
			// essentially add borders to rectangle
			wndRect = rect;
			AdjustWindowRectEx(&wndRect, dwStyle, !standalone, dwExStyle);
		}
		else
		{
			rect.left = winPosX;
			rect.top = winPosY;
			rect.right = winPosX + winWidth;
			rect.bottom = winPosY + winHeight;
			wndRect = rect;
			dwStyle |= WS_CHILD | WS_VISIBLE;
		}

		// set environment
		window_out->env = env;

		// set render context
		window_out->renderingContext = renderContext_opt ? *renderContext_opt : 0;

		// set focus flag
		window_out->rcAlways = window_out->renderingContext ? alwaysRender : 0;

		// set standalone flag
		window_out->isStandalone = standalone;

		// set dummy functions
		a3appSetDummyCallbacks(window_out->demo->callbacks);

		// attempt to make a window
		// 'window_out->window' is set through window process callback
		wndHandle = CreateWindowExA(
			dwExStyle, windowClass->lpszClassName, windowName, dwStyle, 
			0, 0, (wndRect.right - wndRect.left), (wndRect.bottom - wndRect.top),
			0, 0, windowClass->hInstance, window_out);

		// window created
		if (wndHandle)
		{
			// present window
			ShowCursor(showCursor);
			ShowWindow(wndHandle, showWindow);
			UpdateWindow(wndHandle);

			// load demo if standalone
			if (standalone)
				a3windowInternalLoadDemo(window_out, 0);

			// done
			return 1;
		}

		// failed
		return 0;
	}
	return -1;
}

// release window
a3ret a3windowRelease(a3_WindowInterface *window)
{
	if (window)
		return DestroyWindow(window->windowHandle);
	return -1;
}

// window main loop
a3ret a3windowBeginMainLoop(a3_WindowInterface *window)
{
	// message
	MSG msg[1] = { 0 };

	// idle result
	a3i32 idle;

	// while quit message has not been posted
	while (msg->message - WM_QUIT)
	{
		// check for message
		if (PeekMessage(msg, NULL, 0, 0, PM_REMOVE))
		{
			// if there is a message, process the message
			TranslateMessage(msg);
			DispatchMessage(msg);
		}

		// if no message, idle (window required)
		else if (window)
		{
			idle = window->demo->callbacks->callback_idle(window->demo->data);

			// if the result is positive, idle is successful
			// if rendering, this should mean that a frame was rendered
			if (idle > 0)
			{
				if (a3rendererInternalContextIsCurrent(window->renderingContext))
				{
					// swap buffers
					SwapBuffers(window->deviceContext);
				}
			}

			// if the result is negative, the demo should be unloaded
			// standalone window should close the window, which also unloads
			else if (idle < 0)
			{
				if (window->isStandalone)
				{
					// standalone mode, kill window
					PostMessageA(window->windowHandle, WM_CLOSE, 0, 0);
				}
				else
				{
					// exit demo
					PostMessageA(window->windowHandle, A3_USER_EXIT_DEMO, 0, 0);
				}
			}

			// if result is zero, nothing happened
			// ...carry on
		}
	}
	return (a3i32)msg->wParam;
}

// threaded launcher for window main loop
a3i32 __stdcall a3windowInternalMainLoopStarter(a3_WindowInterface *window)
{
	return a3windowBeginMainLoop(window);
}

a3ret a3windowBeginMainLoopThreaded(a3_WindowInterface *window, a3_ThreadHandle *handle)
{
	if (window && handle)
	{
		if (!*handle)
		{
			a3i32 ret[1];
			*handle = CreateThread(0, 0, a3windowInternalMainLoopStarter, window, 0, ret);
			return *ret;
		}
		return 0;
	}
	return -1;
}

a3ret a3windowGetThreadReturnValue(a3_ThreadHandle *handle)
{
	a3ui32 status[1];
	if (handle && *handle)
	{
		GetExitCodeThread(*handle, status);
		if (*status != STILL_ACTIVE)
			return *status;
	}
	return -1;
}


//-----------------------------------------------------------------------------
// window message processing

LRESULT CALLBACK a3windowInternalWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// window counter
	// used to determine when the quit message is posted
	static a3ui32 renderWindowCount = 0;

	// get user data
	a3_WindowInterface *wnd = (a3_WindowInterface *)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
	a3_DemoWindowInterface *demo = wnd->demo;
	a3_CallbackFunctions *callbacks = demo->callbacks;

	// process message
	switch (message)
	{
		// window setup and cleanup
	case WM_NCCREATE: {
		// new window detected, create pointer to user data
		wnd = (a3_WindowInterface *)((LPCREATESTRUCT)lParam)->lpCreateParams;
		wnd->windowHandle = hWnd;
		wnd->deviceContext = GetDC(hWnd);

		// if this is a render window, initialize params
		if (wnd->renderingContext)
		{
			// allocate console with first window
			if (!renderWindowCount)
				a3stdoutConsoleCreate();

			// modify user data here: set ID and pointer to window handle
			wnd->windowID = ++renderWindowCount;

			// set up mouse tracker
			wnd->mouseTracker->cbSize = sizeof(a3_MouseTracker);
			wnd->mouseTracker->dwFlags = TME_LEAVE;
			wnd->mouseTracker->hwndTrack = hWnd;
		}

		// set modified user data
		SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR)wnd);
	}	break;
	case WM_CREATE: {
		// 'wnd' is the user data, the user data is the platform struct
		// use this to initialize rendering for this window
		typedef struct tagA3RENDERPLATFORM {
			a3_WindowHandle hwnd;
			a3_DeviceContext dc;
			a3_RenderingContext rc;
			a3i32 flag;
		} a3_RenderPlatform;
		a3_RenderPlatform *platform = (a3_RenderPlatform *)wnd;

		// window has rendering, proceed with demo business
		if (wnd->renderingContext)
		{
			// setup standalone window, no menu
			if (wnd->isStandalone)
			{
				// allocate and load single demo info
				a3appAllocDemoInfo(&demo->records, 1);
				demo->numRecords = a3appLoadDemoInfo(&demo->records,
					"./animal3D-demos/animal3D-demoinfo.txt", 1);
				if (demo->numRecords)
				{
					// moved to after window is shown to ensure context gets set
				//	a3windowInternalLoadDemo(wnd, 0);
				}
				else
				{
					MessageBoxA(hWnd, "A3 ERROR: Demo config file not found.", "ANIMAL3D ERROR", MB_ICONERROR | MB_OK);
					DestroyWindow(hWnd);
				}
			}

			// setup debug window with menu
			else
			{
				// load menu items
				demo->numRecords = a3appLoadDemoInfo(&demo->records,
					"../../../../resource/animal3D-data/animal3D-demoinfo.txt", 0);

				// create menu for render window
				a3windowInternalCreateMenu(wnd, demo->records, demo->numRecords);

				// window is not currently running demo
				demo->id = -1;
			}

			// enable drawing to this window by setting pixel format
			a3rendererInternalSetContext(platform->dc, platform->rc);
			a3rendererInternalChooseDefaultPixelFormat(platform->flag, platform->dc);
			a3rendererInternalSetContext(0, 0);

			// activate vertical sync and that's all
			a3rendererInternalSetContext(platform->dc, platform->rc);
			a3rendererInternalSetVsync(1);
			a3rendererInternalSetContext(0, 0);
		}

		// setup rendering context
		else
		{
			// set platform's flag to signal pixel format selection
			platform->flag = a3rendererInternalChooseDefaultPixelFormat(platform->flag, platform->dc);
		}
	}	break;
	case WM_CLOSE: 
		// this will also recursively take down any menus the window has
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY: {
		// if this is a render window, unload demo
		// render context should be deleted manually
		if (wnd->renderingContext)
		{
			// unload demo and release demo info records
			a3windowInternalUnloadDemo(wnd);
			a3appReleaseDemoInfo(&demo->records);

			// reset other flags
			wnd->renderingContext = 0;
			wnd->windowID = 0;

			// decrement count; close console and post 
			//	quit message if no more render windows
			if (--renderWindowCount == 0)
			{
				a3stdoutConsoleRelease();
				PostQuitMessage(0);
			}
		}

		// release device context
		if (wnd->deviceContext)
		{
			ReleaseDC(hWnd, wnd->deviceContext);
			wnd->deviceContext = 0;
		}

		// done, reset window handle
		wnd->windowHandle = 0;
	}	break;
	case WM_PAINT: 
		if (!callbacks->callback_display(demo->data))
		{
			PAINTSTRUCT paint[1];
			RECT updateRect[1];
			GetUpdateRect(hWnd, updateRect, 0);
			FillRect(BeginPaint(hWnd, paint), updateRect, (HBRUSH)(COLOR_WINDOW));
			EndPaint(hWnd, paint);
		}
		break;
	case WM_ERASEBKGND:
		break;
	case WM_ACTIVATE: 
		switch (LOWORD(wParam))
		{
		case WA_ACTIVE: 
		case WA_CLICKACTIVE: 
			// if window has rendering, the context will activate
			// activate context first in case the callback needs to draw
			a3rendererInternalSetContext(wnd->deviceContext, wnd->renderingContext);
			callbacks->callback_windowActivate(demo->data);
			break;
		case WA_INACTIVE: 
			// reverse of above, callback then disable rendering
			callbacks->callback_windowDeactivate(demo->data);
			if (!wnd->rcAlways)
				a3rendererInternalSetContext(0, 0);
			break;
		}
		break;

		// messages from window extension (menu)
	case WM_COMMAND: {
		a3i32 selection = LOWORD(wParam);
		switch (selection)
		{
		case A3_MENU_EXIT_PROG:
			// window should be closed
			PostMessageA(hWnd, WM_CLOSE, 0, 0);
			break;
		case A3_MENU_EXIT_DEMO:
			// release current demo if one is running
			a3windowInternalUnloadDemo(wnd);
			break;
		case A3_MENU_RELOAD_DEMO:
			// release and immediately load demo if running
			if (demo->id >= 0)
				a3windowInternalLoadDemo(wnd, a3windowInternalUnloadDemo(wnd));
			else if (demo->id == -2)
			{
				// unload and build
				a3windowInternalUnloadDemo(wnd);
				a3windowInternalHotloadDemo(wnd, 0);
			}
			break;

		case A3_MENU_RELOAD_MENU:
			// reload the menu
			a3windowInternalReloadMenu(wnd, demo);
			break;
		case A3_MENU_TOGGLE_CONSOLE:
			// attempt to create console, set flag to result
			if (!a3windowInternalSetConsoleToggle(1, a3stdoutConsoleCreate()))
			{
				// no longer prints to console
				a3windowInternalSetConsolePrint(1, 0);
				
				// console already exists; release
				a3stdoutConsoleRelease();
			}
			a3windowInternalReloadMenu(wnd, demo);
			break;
		case A3_MENU_PRINT_TO_CONSOLE:
			// toggle print-to-console flag; enable only if console exists
			a3windowInternalSetConsolePrint(1, 
				a3windowInternalGetConsoleToggle() && !a3windowInternalGetConsolePrint());
			a3windowInternalReloadMenu(wnd, demo);
			break;
		case A3_MENU_PRINT_RENDERER:
			a3rendererDisplayInfoPrintFunc(&wnd->renderingContext, (a3windowInternalGetConsolePrint() ? printf : a3printf_vsdebug));
			break;
		case A3_MENU_CLEAR_CONSOLE:
			a3stdoutConsoleClear();
			break;

		case A3_MENU_LOAD_DEBUG:
			// release demo if one is running, then hotload
			if (demo->id >= 0)
				a3windowInternalUnloadDemo(wnd);
			a3windowInternalHotloadDemo(wnd, -1);
			break;
		case A3_MENU_BUILD_DEBUG: 
			if (demo->id >= 0)
				a3windowInternalUnloadDemo(wnd);
			a3windowInternalHotloadDemo(wnd, 0);
			break;
		case A3_MENU_REBUILD_DEBUG:
			if (demo->id >= 0)
				a3windowInternalUnloadDemo(wnd);
			a3windowInternalHotloadDemo(wnd, 1);
			break;

		default: 
			if (selection >= A3_MENU_DEMO_LIST)
			{
				// if selecting from the demo list, 
				//	unload current then load new demo
				a3i32 id = selection - A3_MENU_DEMO_LIST;
				a3windowInternalUnloadDemo(wnd);
				a3windowInternalLoadDemo(wnd, id);
			}
			break;
		}
	}	break;

		// window callbacks
	case WM_MOVE: 
		// window moves
		demo->winPosX = (a3i32)LOWORD(lParam);
		demo->winPosY = (a3i32)HIWORD(lParam);
		callbacks->callback_windowMove(demo->data, demo->winPosX, demo->winPosY);
		break;
	case WM_SIZE:
		// window is resized
		demo->winSzX = (a3i32)LOWORD(lParam);
		demo->winSzY = (a3i32)HIWORD(lParam);
		callbacks->callback_windowResize(demo->data, demo->winSzX, demo->winSzY);
		break;

		// keyboard callbacks
	case WM_KEYDOWN: 
		// any virtual key
		(HIWORD(lParam) & KF_REPEAT ? callbacks->callback_keyHold : callbacks->callback_keyPress)(demo->data, (a3i32)LOWORD(wParam));
		break;
	case WM_CHAR: 
		// character keys, no up call
		(HIWORD(lParam) & KF_REPEAT ? callbacks->callback_keyCharHold : callbacks->callback_keyCharPress)(demo->data, (a3i32)LOWORD(wParam));
		break;
	case WM_KEYUP: 
		// release for keyPress
		callbacks->callback_keyRelease(demo->data, (a3i32)wParam);
		break;

//	case WM_SYSKEYDOWN: 
//		// alt or F10 pressed, can treat as normal keypress
//		break;
//	case WM_SYSCHAR: 
//		// alt-character combo, normally accelerated
//		break;
//	case WM_SYSKEYUP:
//		// alt released... sometimes?
//		break;

		// mouse callbacks
	case WM_LBUTTONDOWN: 
		callbacks->callback_mouseClick(demo->data, 0, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_MBUTTONDOWN:
		callbacks->callback_mouseClick(demo->data, 1, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		callbacks->callback_mouseClick(demo->data, 3, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_XBUTTONDOWN:
		callbacks->callback_mouseClick(demo->data, (a3i32)HIWORD(wParam) + 3, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_LBUTTONDBLCLK: 
		callbacks->callback_mouseDoubleClick(demo->data, 0, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_MBUTTONDBLCLK:
		callbacks->callback_mouseDoubleClick(demo->data, 1, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_RBUTTONDBLCLK:
		callbacks->callback_mouseDoubleClick(demo->data, 3, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_XBUTTONDBLCLK:
		callbacks->callback_mouseDoubleClick(demo->data, (a3i32)HIWORD(wParam) + 3, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_LBUTTONUP: 
		callbacks->callback_mouseRelease(demo->data, 0, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_MBUTTONUP:
		callbacks->callback_mouseRelease(demo->data, 1, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_RBUTTONUP:
		callbacks->callback_mouseRelease(demo->data, 3, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_XBUTTONUP:
		callbacks->callback_mouseClick(demo->data, (a3i32)HIWORD(wParam) + 3, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_MOUSEWHEEL:
		callbacks->callback_mouseWheel(demo->data, (a3i32)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		break;
	case WM_MOUSEMOVE: 
		callbacks->callback_mouseMove(demo->data, (a3i32)LOWORD(lParam), (a3i32)HIWORD(lParam));
		TrackMouseEvent(wnd->mouseTracker);
		break;
	case WM_MOUSELEAVE: 
		callbacks->callback_mouseLeave(demo->data);
		break;

		// custom messages
	case A3_HOTLOAD_COMPLETE: 
		a3windowInternalHotloadDemoComplete(wnd, (a3boolean)wParam, (a3boolean)lParam);
		break;
	case A3_USER_EXIT_DEMO:
		a3windowInternalUnloadDemo(wnd);
		InvalidateRect(wnd->windowHandle, 0, TRUE);
		UpdateWindow(wnd->windowHandle);
		break;

		// end
	default: 
		break;
	}
	return DefWindowProcA(hWnd, message, wParam, lParam);
}


//-----------------------------------------------------------------------------


#endif	// (defined _WINDOWS || defined _WIN32)