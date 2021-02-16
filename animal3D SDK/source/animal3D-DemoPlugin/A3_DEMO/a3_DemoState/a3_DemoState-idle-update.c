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
	
	a3_DemoState_idle-update.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     UPDATING THE STATE in this file.         ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------
// UPDATE SUB-ROUTINES

void a3demo_update_objects(a3f64 const dt, a3_SceneObjectComponent const* sceneObjectArray,
	a3ui32 const count, a3boolean const useZYX)
{
	a3ui32 i;

	// update transforms
	for (i = 0; i < count; ++i, ++sceneObjectArray)
	{
		a3demo_updateSceneObject(sceneObjectArray, useZYX);
	}
}

void a3demo_update_defaultAnimation(a3f64 const dt, a3_SceneObjectComponent const* sceneObjectArray,
	a3ui32 const count, a3ui32 const axis, a3boolean const updateAnimation)
{
	a3real const dr = (a3real)(dt * (a3f64)updateAnimation);
	a3ui32 i;

	// do simple animation
	for (i = 0; i < count; ++i, ++sceneObjectArray)
	{
		sceneObjectArray->dataPtr->euler.v[axis] =
			a3trigValid_sind(sceneObjectArray->dataPtr->euler.v[axis] + dr);
	}
}

void a3demo_update_bindSkybox(a3_SceneObjectComponent const* sceneObject_skybox,
	a3_ProjectorComponent const* projector_active)
{
	// model transformations (if needed)
	const a3mat4 convertY2Z = {
		+1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, +1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, +1.0f,
	};

	// bind skybox to camera
	sceneObject_skybox->modelMatrixStackPtr->modelMat.v3 = projector_active->sceneObjectPtr->modelMatrixStackPtr->modelMat.v3;

	// correct rotations as needed
	// need to rotate skybox if Z-up
	a3real4x4ConcatL(sceneObject_skybox->modelMatrixStackPtr->modelMat.m, convertY2Z.m);
}

void a3demo_update_pointLight(a3_PointLightComponent const* pointLightArray,
	a3_ProjectorComponent const* projector_active, a3ui32 const count)
{
	a3ui32 i;

	// update lights view positions for current camera
	for (i = 0; i < count; ++i, ++pointLightArray)
	{
		// convert to view space and retrieve view position
		a3real4Real4x4Product(pointLightArray->dataPtr->position.v,
			projector_active->sceneObjectPtr->modelMatrixStackPtr->modelMatInverse.m,
			pointLightArray->dataPtr->worldPos.v);
	}
}


//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update(a3_DemoState *demoState, a3f64 const dt)
{
	// callback for current mode
	demoState->demoModeCallbacksPtr = demoState->demoModeCallbacks + demoState->demoMode;
	if (demoState->demoModeCallbacksPtr)
	{
		demoState->demoModeCallbacksPtr->handleUpdate(demoState,
			demoState->demoModeCallbacksPtr->demoMode, dt);
	}
}


//-----------------------------------------------------------------------------
