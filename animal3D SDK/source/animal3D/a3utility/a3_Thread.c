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
	
	a3_Thread.c
	Basic thread starter implementation.

	**DO NOT MODIFY THIS FILE**
*/

#include "animal3D/a3utility/a3_Thread.h"


// the new C++ standard comes with all of this stuff "built-in"
// ...either use that or be an engineer and fashion your own!
#if (defined _WINDOWS || defined _WIN32)
#include <Windows.h>
#else	// !(defined _WINDOWS || defined _WIN32)
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#endif	// (defined _WINDOWS || defined _WIN32)
#include <string.h>


//-----------------------------------------------------------------------------

// internal name set function
// from "How to: Set a Thread Name in Native Code"
// msdn.microsoft.com
a3ret a3threadInternalSetName(a3ui32 id, const a3byte *name)
{
	// name must exist and id must be -1 or positive
	if (name && id)
	{
#if (defined _WINDOWS || defined _WIN32)
		// name change structure: 
		// declare a data structure that can be used internally
#pragma pack(push, 8)	// 8-bit alignment
		typedef struct tagTHREADNAME_INFO
		{
			DWORD type;		// must be 0x1000 (reserved)
			LPCSTR name;	// name string
			DWORD threadID;	// thread ID (-1 for calling thread)
			DWORD flags;	// must be zero (reserved)
		} THREADNAME_INFO;
		THREADNAME_INFO info = { 0x1000, name, id, 0 };
#pragma pack(pop)
#pragma warning(push)
#pragma warning(disable: 6320 6322)	// suppress warnings
		__try
		{
			// attempt name change
			RaiseException(0x406D1388, 0,
				sizeof(info) / sizeof(DWORD), (ULONG_PTR *)(&info));
		}
		__except (EXCEPTION_CONTINUE_EXECUTION)
		{
			// unhandled exception
		}
#pragma warning(pop)
		return 1;
#else	// !(defined _WINDOWS || defined _WIN32)
		// do rename
		return 0;
#endif	// (defined _WINDOWS || defined _WIN32)
	}
	return -1;
}


// internal thread launcher
#if (defined _WINDOWS || defined _WIN32)
a3ui32 __stdcall a3threadInternalLaunch(a3_Thread *thread)
{
#else	// !(defined _WINDOWS || defined _WIN32)
void *a3threadInternalLaunch(a3_Thread *thread)
{
	// push "not complete" flag
	// set "not complete" parameter to not-complete
#endif	// (defined _WINDOWS || defined _WIN32)

	// change the name of the thread before execution 
	//	so we can identify it
	a3threadInternalSetName((a3ui32)(-1), thread->threadName ? thread->threadName : "animal3D thread");

	// call your thread function (assuming no params)
	// be sure to catch return value
	thread->inThreadFunc = 1;
	thread->result = thread->threadFunc(thread->threadArgs);
	thread->inThreadFunc = 0;

	// post-call: lower active flag and invalidate handle(s) to indicate 
	//	thread is done
	thread->threadID = 0;
	thread->handle[0] = 0;
	thread->handle[1] = 0;

	// done, exit
#if (defined _WINDOWS || defined _WIN32)
	return (a3ui32)(thread->result);
}
#else	// !(defined _WINDOWS || defined _WIN32)
	// pop "non-complete flag"
	return p;
}
#endif	// (defined _WINDOWS || defined _WIN32)


//-----------------------------------------------------------------------------

// start thread
a3ret a3threadLaunch(a3_Thread *thread_out, a3_threadfunc func, void *args_opt, a3byte *name_opt)
{
	if (thread_out && func)
	{
		if (!*thread_out->handle)
		{
			// copy internal arguments
			thread_out->result = 0;
			thread_out->inThreadFunc = 0;
			thread_out->threadFunc = func;
			thread_out->threadArgs = args_opt;
			thread_out->threadName = name_opt;

			// launch
#if (defined _WINDOWS || defined _WIN32)
			*thread_out->handle = CreateThread(0, 0, a3threadInternalLaunch, thread_out, 0, &thread_out->threadID);
#else	// !(defined _WINDOWS || defined _WIN32)
			thread_out->threadID = 1;
			pthread_create((pthread_t *)thread_out->handle, 0, a3threadInternalLaunch, thread_out);
#endif	// (defined _WINDOWS || defined _WIN32)

			if (*thread_out->handle)
				return 1;
		}
		return 0;
	}
	return -1;
}


// kill thread
a3ret a3threadTerminate(a3_Thread *thread)
{
	if (thread && *thread->handle)
	{
		a3i32 success;
#if (defined _WINDOWS || defined _WIN32)
		success = TerminateThread(*thread->handle, (a3ui32)(-1));
		success = CloseHandle(*thread->handle);
#else	// !(defined _WINDOWS || defined _WIN32)
		success = (pthread_kill(*(pthread_t *)thread->handle, SIGKILL) == 0);
#endif	// (defined _WINDOWS || defined _WIN32)

		// reset handle values
		if (success)
		{
			thread->threadID = 0;
			thread->handle[0] = 0;
			thread->handle[1] = 0;
			return 1;
		}
		return 0;
	}
	return -1;
}


// wait for thread
a3ret a3threadWait(a3_Thread *thread)
{
	if (thread && *thread->handle)
	{
#if (defined _WINDOWS || defined _WIN32)
		return (WaitForSingleObject(*thread->handle, INFINITE) == 0);
#else	// !(defined _WINDOWS || defined _WIN32)
		return (pthread_join(*(pthread_t *)thread->handle, 0) == 0);
#endif	// (defined _WINDOWS || defined _WIN32)
	}
	return -1;
}


// is running thread
a3ret a3threadIsRunning(const a3_Thread *thread)
{
	if (thread && *thread->handle)
	{
#if (defined _WINDOWS || defined _WIN32)
		a3ui32 status[1];
		GetExitCodeThread(*thread->handle, status);
		if (*status == STILL_ACTIVE)
			return 1;
#else	// !(defined _WINDOWS || defined _WIN32)
		// check if "not complete" flag is set non-complete
#endif	// (defined _WINDOWS || defined _WIN32)

		return 0;
	}
	return -1;
}


// get caller's thread ID
a3ret a3threadInternalGetID()
{
#if (defined _WINDOWS || defined _WIN32)
	return GetCurrentThreadId();
#else	// !(defined _WINDOWS || defined _WIN32)
	return gettid();
#endif	// (defined _WINDOWS || defined _WIN32)
}
