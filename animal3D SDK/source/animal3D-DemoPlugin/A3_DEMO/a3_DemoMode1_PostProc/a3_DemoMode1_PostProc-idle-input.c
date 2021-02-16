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

	a3_DemoMode1_PostProc-idle-input.c
	Demo mode implementations: animation scene.

	********************************************
	*** INPUT FOR ANIMATION SCENE MODE       ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_PostProc.h"

typedef struct a3_DemoState a3_DemoState;
//#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// CALLBACKS

// main demo mode callback
void a3postproc_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode1_PostProc* demoMode, a3i32 const asciiKey, a3i32 const state)
{
	switch (asciiKey)
	{
		// toggle render program
		a3demoCtrlCasesLoop(demoMode->renderMode, postproc_renderMode_max, 'k', 'j');
		// toggle render pass
		a3demoCtrlCasesLoop(demoMode->renderPass, postproc_renderPass_max, 'K', 'J');
		// toggle render target
		a3demoCtrlCasesLoop(demoMode->renderTarget[demoMode->renderPass],
			demoMode->renderTargetCount[demoMode->renderPass], 'M', 'N');
	}
}

void a3postproc_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode1_PostProc* demoMode, a3i32 const asciiKey, a3i32 const state)
{
//	switch (asciiKey)
//	{
//
//	}
}


//-----------------------------------------------------------------------------

void a3postproc_input(a3_DemoState* demoState, a3_DemoMode1_PostProc* demoMode, a3f64 const dt)
{
	void a3demo_input_controlProjector(a3_ProjectorComponent const* projector,
		a3_DemoState const* demoState, a3f64 const dt);

	a3demo_input_controlProjector(demoMode->proj_camera_main, demoState, dt);
}


//-----------------------------------------------------------------------------
