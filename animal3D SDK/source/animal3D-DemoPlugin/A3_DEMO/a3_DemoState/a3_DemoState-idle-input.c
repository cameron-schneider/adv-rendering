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
	
	a3_DemoState_idle-input.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     HANDLING INPUT in this file.             ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// INPUT SUB-ROUTINES

void a3demo_input_controlProjector(a3_ProjectorComponent const* projector,
	a3_DemoState const* demoState, a3f64 const dt)
{
	a3real azimuth = a3real_zero;
	a3real elevation = a3real_zero;
	a3real zoom = a3real_zero;

	a3f64 const moveRate = dt * projector->dataPtr->ctrlMoveSpeed;
	a3f64 const rotateRate = dt * projector->dataPtr->ctrlRotateSpeed, ctrlRotateRate = rotateRate * 15.0;
	a3f64 const zoomRate = dt * projector->dataPtr->ctrlZoomSpeed, zoomLimit = (180.0 - (a3f64)zoomRate);

	// controller input
	if (a3XboxControlIsConnected(demoState->xcontrol))
	{
		// move and rotate camera using joysticks
		a3f64 lJoystick[2], rJoystick[2], lTrigger, rTrigger;
		a3XboxControlGetJoysticks(demoState->xcontrol, lJoystick, rJoystick);
		a3XboxControlGetTriggers(demoState->xcontrol, &lTrigger, &rTrigger);

		a3demo_moveProjectorSceneObject(projector,
			(a3real)(moveRate * (+lJoystick[0])),
			(a3real)(moveRate * (rTrigger - lTrigger)),
			(a3real)(moveRate * (-lJoystick[1]))
		);
		// rotate
		{
			azimuth = (a3real)(-rJoystick[0]);
			elevation = (a3real)(+rJoystick[1]);

			// this really defines which way is "up"
			// mouse's Y motion controls pitch, but X can control yaw or roll
			// controlling yaw makes Y axis seem "up", roll makes Z seem "up"
			a3demo_rotateProjectorSceneObject(projector,
				// pitch: vertical tilt
				(a3real)ctrlRotateRate * elevation,
				// yaw/roll depends on "vertical" axis: if y, yaw; if z, roll
				a3real_zero,
				(a3real)ctrlRotateRate * azimuth);
		}

		// zoom
		projector->dataPtr->fovy -= (a3real)zoomRate
			* (a3real)((a3XboxControlGetState(demoState->xcontrol, a3xbox_leftBumper) / a3xbox_leftBumper
				- a3XboxControlGetState(demoState->xcontrol, a3xbox_rightBumper) / a3xbox_rightBumper));
	}

	// keyboard and mouse input
	else
	{
		// move using WASDEQ
		a3demo_moveProjectorSceneObject(projector,
			(a3real)(moveRate * (a3f64)a3keyboardGetDifference(demoState->keyboard, a3key_D, a3key_A)),
			(a3real)(moveRate * (a3f64)a3keyboardGetDifference(demoState->keyboard, a3key_E, a3key_Q)),
			(a3real)(moveRate * (a3f64)a3keyboardGetDifference(demoState->keyboard, a3key_S, a3key_W))
		);
		if (a3mouseIsHeld(demoState->mouse, a3mouse_left))
		{
			azimuth = -(a3real)a3mouseGetDeltaX(demoState->mouse);
			elevation = -(a3real)a3mouseGetDeltaY(demoState->mouse);

			// this really defines which way is "up"
			// mouse's Y motion controls pitch, but X can control yaw or roll
			// controlling yaw makes Y axis seem "up", roll makes Z seem "up"
			a3demo_rotateProjectorSceneObject(projector,
				// pitch: vertical tilt
				(a3real)rotateRate * elevation,
				// yaw/roll depends on "vertical" axis: if y, yaw; if z, roll
				a3real_zero,
				(a3real)rotateRate * azimuth);
		}

		// zoom
		projector->dataPtr->fovy -= (a3real)zoomRate
			* (a3real)(a3mouseIsChanged(demoState->mouse, a3mouse_wheel));
	}

	// clamp zoom
	projector->dataPtr->fovy = a3clamp((a3real)zoomRate, (a3real)zoomLimit, projector->dataPtr->fovy);
	a3demo_updateProjector(projector);
}


//-----------------------------------------------------------------------------
// CALLBACKS

// ascii key callback
void a3demo_input_keyCharPress(a3_DemoState* demoState, a3i32 const asciiKey)
{
	switch (asciiKey)
	{
		// change pipeline mode
		a3demoCtrlCasesLoop(demoState->demoMode, demoState_mode_max, '>', '<');
		a3demoCtrlCasesLoop(demoState->demoMode, demoState_mode_max, '.', ',');

		// toggle grid
		a3demoCtrlCaseToggle(demoState->displayGrid, 'g');

		// toggle skybox
		a3demoCtrlCaseToggle(demoState->displaySkybox, 'b');

		// toggle hidden volumes
		a3demoCtrlCaseToggle(demoState->displayHiddenVolumes, 'h');

		// toggle world axes
		a3demoCtrlCaseToggle(demoState->displayWorldAxes, 'x');

		// toggle object axes
		a3demoCtrlCaseToggle(demoState->displayObjectAxes, 'z');

		// toggle tangent bases on vertices or other
		a3demoCtrlCaseToggle(demoState->displayTangentBases, 'B');

		// toggle wireframe overlay
		a3demoCtrlCaseToggle(demoState->displayWireframe, 'F');

		// update animation
		a3demoCtrlCaseToggle(demoState->updateAnimation, 'm');

		// toggle stencil test
		a3demoCtrlCaseToggle(demoState->stencilTest, 'i');

		// toggle stencil test
		a3demoCtrlCaseToggle(demoState->skipIntermediatePasses, 'I');
	}


	// callbacks for current mode
	demoState->demoModeCallbacksPtr = demoState->demoModeCallbacks + demoState->demoMode;
	if (demoState->demoModeCallbacksPtr)
	{
		if (demoState->demoModeCallbacksPtr->handleKeyPress)
			demoState->demoModeCallbacksPtr->handleKeyPress(demoState,
				demoState->demoModeCallbacksPtr->demoMode, asciiKey, 0);
		if (demoState->demoModeCallbacksPtr->handleKeyHold)
			demoState->demoModeCallbacksPtr->handleKeyHold(demoState,
				demoState->demoModeCallbacksPtr->demoMode, asciiKey, 0);
	}
}

// ascii key hold callback
void a3demo_input_keyCharHold(a3_DemoState* demoState, a3i32 asciiKey)
{
//	switch (asciiKey)
//	{
//
//	}


	// callbacks for current mode
	demoState->demoModeCallbacksPtr = demoState->demoModeCallbacks + demoState->demoMode;
	if (demoState->demoModeCallbacksPtr)
	{
		if (demoState->demoModeCallbacksPtr->handleKeyHold)
			demoState->demoModeCallbacksPtr->handleKeyHold(demoState,
				demoState->demoModeCallbacksPtr->demoMode, asciiKey, 0);
	}
}


//-----------------------------------------------------------------------------
// INPUT

void a3demo_input(a3_DemoState* demoState, a3f64 const dt)
{
	// callback for current mode
	demoState->demoModeCallbacksPtr = demoState->demoModeCallbacks + demoState->demoMode;
	if (demoState->demoModeCallbacksPtr)
	{
		if (demoState->demoModeCallbacksPtr->handleInput)
			demoState->demoModeCallbacksPtr->handleInput(demoState,
				demoState->demoModeCallbacksPtr->demoMode, dt);
	}
}


//-----------------------------------------------------------------------------
