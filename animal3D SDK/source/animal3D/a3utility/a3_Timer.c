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
	
	a3_Timer.c
	Multi-platform high-precision timer implementation.

	**DO NOT MODIFY THIS FILE**
*/

#include "animal3D/a3utility/a3_Timer.h"


#if (defined _WINDOWS || defined _WIN32)
#include <Windows.h>
#if defined(MIDL_PASS)
#define Q	qs
#else	// !MIDL_PASS
#define Q	qu
#endif	// MIDL_PASS
#else   // !(defined _WINDOWS || defined _WIN32)
#include <time.h>
#define Q   qs
#define BILLION 1000000000
#endif  // (defined _WINDOWS || defined _WIN32)


// start
a3ret a3timerStart(a3_Timer *timer)
{
	if (timer)
	{
		// raise started flag, take current measurement
		timer->started = 1;

#ifdef _WIN32
		QueryPerformanceFrequency((LARGE_INTEGER *)timer->tf);
		QueryPerformanceCounter((LARGE_INTEGER *)timer->t0);
#else   // !_WIN32
	//	clock_getres(CLOCK_REALTIME, (struct timespec *)timer->tf);
		clock_gettime(CLOCK_REALTIME, (struct timespec *)timer->t0);

		// need to convert the entire thing to seconds
		// measurement every tf->q[1] nanoseconds
		// total number of nanoseconds = nano + 1B * t->q[0]
		timer->t0->Q = BILLION*(a3i64)(timer->t0->q[0]) + (a3i64)(timer->t0->q[1]);
		timer->tf->Q = BILLION;
#endif  // _WIN32

		// reset tick
		timer->currentTick = 0.0;
		return 1;
	}
	return -1;
}

// stop
a3ret a3timerStop(a3_Timer *timer)
{
	if (timer)
	{
		// lower started flag
		timer->started = 0;

#ifdef _WIN32
		QueryPerformanceFrequency((LARGE_INTEGER *)timer->tf);
		QueryPerformanceCounter((LARGE_INTEGER *)timer->t1);
#else   // !_WIN32
	//	clock_getres(CLOCK_REALTIME, (struct timespec *)timer->tf);
		clock_gettime(CLOCK_REALTIME, (struct timespec *)timer->t1);
		timer->t1->Q = BILLION*(a3i64)(timer->t1->q[0]) + (a3i64)(timer->t1->q[1]);
#endif  // _WIN32

		// measure tick
		timer->currentTick = (a3f64)(timer->t1->Q - timer->t0->Q) / (a3f64)(timer->tf->Q);
		return 1;
	}
	return -1;
}


// reset
a3ret a3timerReset(a3_Timer *timer)
{
	if (timer)
	{
		// set all values to zero
		timer->started = 0;
		timer->ticks = 0;
		timer->totalTime = 0.0;
		timer->currentTick = 0.0;
		timer->previousTick = 0.0;
		return 1;
	}
	return -1;
}


// set ticks per second; also resets
// NOTE: if ticks per second is ZERO, update will ALWAYS return true
//	(every update is a tick)
a3ret a3timerSet(a3_Timer *timer, a3f64 ticksPerSecond)
{
	if (timer)
	{
		// reset
		timer->started = 0;
		timer->ticks = 0;
		timer->totalTime = 0.0;
		timer->currentTick = 0.0;
		timer->previousTick = 0.0;

		// set tick rate
		if (ticksPerSecond > 0.0)
		{
			timer->ticksPerSecond = ticksPerSecond;
			timer->secondsPerTick = 1.0 / ticksPerSecond;
		}
		else
		{
			timer->ticksPerSecond = 0.0;
			timer->secondsPerTick = 0.0;
		}
		return 1;
	}
	return -1;
}


// update: determine if clock needs to tick
a3ret a3timerUpdate(a3_Timer *timer)
{
	a3i32 tick = 0;
	if (timer)
	{
		if (timer->started)
		{
			// take measurement
#ifdef _WIN32
			QueryPerformanceFrequency((LARGE_INTEGER *)timer->tf);
			QueryPerformanceCounter((LARGE_INTEGER *)timer->t1);
#else   // !_WIN32
		//	clock_getres(CLOCK_REALTIME, (struct timespec *)timer->tf);
			clock_gettime(CLOCK_REALTIME, (struct timespec *)timer->t1);
			timer->t1->Q = BILLION*(a3i64)(timer->t1->q[0]) + (a3i64)(timer->t1->q[1]);
#endif  // _WIN32

			// accumulate time
			timer->currentTick = (a3f64)(timer->t1->Q - timer->t0->Q) / (a3f64)(timer->tf->Q);

			// if not continuously updating
			if (timer->secondsPerTick > 0.0)
			{
				// if the current measure exceeds the rate
				while (timer->currentTick >= timer->secondsPerTick)
				{
					// copy current time measurement to previous
					*(timer->t0) = *(timer->t1);

					// update tick: transfer expected delta from tick to total 
					//	until the tick does not exceed rate
					timer->totalTime += timer->secondsPerTick;
					timer->previousTick = timer->currentTick;
					timer->currentTick -= timer->secondsPerTick;
					++timer->ticks;
					++tick;
				}
			}

			// "continuous" update, always tick and update time
			else
			{
				*(timer->t0) = *(timer->t1);
				timer->totalTime += timer->currentTick;
				timer->previousTick = timer->currentTick;
				timer->currentTick = 0.0;
				++timer->ticks;
				++tick;
			}
		}
		return tick;
	}
	return -1;
}
