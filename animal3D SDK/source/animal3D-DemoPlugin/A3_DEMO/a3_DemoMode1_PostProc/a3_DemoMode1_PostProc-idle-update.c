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
	
	a3_DemoMode1_PostProc-idle-update.c
	Demo mode implementations: animation scene.

	********************************************
	*** UPDATE FOR ANIMATION SCENE MODE      ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_PostProc.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// UPDATE

void a3postproc_update_graphics(a3_DemoState* demoState, a3_DemoMode1_PostProc* demoMode)
{
	// ****TO-DO:
	//	-> uncomment uniform data upload
	//	-> upload model and light data to respective uniform buffers
	//		(hint: projectors and models are together)
/*	// upload
	a3bufferRefillOffset(demoState->ubo_transform, 0, 0, sizeof(demoMode->projectorMatrixStack), demoMode->projectorMatrixStack);
	//...*/
}

void a3postproc_update_scene(a3_DemoState* demoState, a3_DemoMode1_PostProc* demoMode, a3f64 const dt)
{
	void a3demo_update_defaultAnimation(a3f64 const dt, a3_SceneObjectComponent const* sceneObjectArray,
		a3ui32 const count, a3ui32 const axis, a3boolean const updateAnimation);
	void a3demo_update_bindSkybox(a3_SceneObjectComponent const* sceneObject_skybox,
		a3_ProjectorComponent const* projector_active);

	const a3mat4 bias = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	}, biasInv = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 1.0f
	};

	a3_ProjectorComponent* projector = demoMode->proj_camera_main;

	a3_PointLightData* pointLightData;
	a3ui32 i;

	// update camera
	a3demo_updateSceneObject(demoMode->obj_camera_main, 1);
	a3demo_updateSceneObjectStack(demoMode->obj_camera_main, projector);
	a3demo_updateProjector(projector);
	a3demo_updateProjectorViewMats(projector);
	a3demo_updateProjectorBiasMats(projector, bias, biasInv);

	// update light
	a3demo_updateSceneObject(demoMode->obj_light_main, 1);
	a3demo_updateSceneObjectStack(demoMode->obj_light_main, projector);
	a3demo_updateProjector(demoMode->proj_light_main);
	a3demo_updateProjectorViewMats(demoMode->proj_light_main);
	a3demo_updateProjectorBiasMats(demoMode->proj_light_main, bias, biasInv);

	// update skybox
	a3demo_updateSceneObject(demoMode->obj_skybox, 0);
	a3demo_update_bindSkybox(demoMode->obj_skybox, projector);
	a3demo_updateSceneObjectStack(demoMode->obj_skybox, projector);

	// update scene objects
	a3demo_update_defaultAnimation((dt * 15.0), demoMode->obj_sphere,
		(a3ui32)(demoMode->obj_ground - demoMode->obj_sphere), 2, demoState->updateAnimation);

	a3demo_updateSceneObject(demoMode->obj_sphere, 0);
	a3demo_updateSceneObjectStack(demoMode->obj_sphere, projector);

	a3demo_updateSceneObject(demoMode->obj_cylinder, 0);
	a3demo_updateSceneObjectStack(demoMode->obj_cylinder, projector);

	a3demo_updateSceneObject(demoMode->obj_capsule, 0);
	a3demo_updateSceneObjectStack(demoMode->obj_capsule, projector);

	a3demo_updateSceneObject(demoMode->obj_torus, 0);
	a3demo_updateSceneObjectStack(demoMode->obj_torus, projector);

	a3demo_updateSceneObject(demoMode->obj_cone, 0);
	a3demo_updateSceneObjectStack(demoMode->obj_cone, projector);

	a3demo_updateSceneObject(demoMode->obj_teapot, 0);
	a3demo_updateSceneObjectStack(demoMode->obj_teapot, projector);

	a3demo_updateSceneObject(demoMode->obj_ground, 0);
	a3demo_updateSceneObjectStack(demoMode->obj_ground, projector);

	// update light positions
	for (i = 0, pointLightData = demoMode->pointLightData;
		i < postprocMaxCount_pointLight;
		++i, ++pointLightData)
	{
		a3real4Real4x4Product(pointLightData->position.v,
			projector->sceneObjectPtr->modelMatrixStackPtr->modelMatInverse.m,
			pointLightData->worldPos.v);
	}
}

void a3postproc_update(a3_DemoState* demoState, a3_DemoMode1_PostProc* demoMode, a3f64 const dt)
{
	// update scene objects and related data
	a3postproc_update_scene(demoState, demoMode, dt);

	// prepare and upload graphics data
	a3postproc_update_graphics(demoState, demoMode);
}


//-----------------------------------------------------------------------------
