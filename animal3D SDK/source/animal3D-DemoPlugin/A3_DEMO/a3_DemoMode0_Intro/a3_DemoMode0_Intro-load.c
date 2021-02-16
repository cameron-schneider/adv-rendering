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

	a3_DemoMode0_Intro-load.c
	Demo mode implementations: animation scene.

	********************************************
	*** LOADING FOR ANIMATION SCENE MODE     ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode0_Intro.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------

void a3intro_loadValidate(a3_DemoState* demoState, a3_DemoMode0_Intro* demoMode)
{
	void a3intro_input(a3_DemoState* demoState, a3_DemoMode0_Intro* demoMode, a3f64 const dt);
	void a3intro_update(a3_DemoState* demoState, a3_DemoMode0_Intro* demoMode, a3f64 const dt);
	void a3intro_render(a3_DemoState const* demoState, a3_DemoMode0_Intro const* demoMode, a3f64 const dt);
	void a3intro_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode0_Intro* demoMode, a3i32 const asciiKey, a3i32 const state);
	void a3intro_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode0_Intro* demoMode, a3i32 const asciiKey, a3i32 const state);

	// initialize callbacks
	a3_DemoModeCallbacks* const callbacks = demoState->demoModeCallbacks + demoState_modeIntro;
	callbacks->demoMode =		demoMode;
	callbacks->handleInput =	(a3_DemoMode_EventCallback)a3intro_input;
	callbacks->handleUpdate =	(a3_DemoMode_EventCallback)a3intro_update;
	callbacks->handleRender =	(a3_DemoMode_EventCallback)a3intro_render;
	callbacks->handleKeyPress = (a3_DemoMode_InputCallback)a3intro_input_keyCharPress;
	callbacks->handleKeyHold =	(a3_DemoMode_InputCallback)a3intro_input_keyCharHold;

	// initialize scene object pointers
	a3demo_initSceneObject(demoMode->objgroup_world_root, 0,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_camera_main, 1,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_skybox, 2,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_sphere, 3,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_cylinder, 4,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_capsule, 5,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_torus, 6,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_cone, 7,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_teapot, 8,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_ground, 9,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);

	// initialize projector pointers
	a3demo_initProjector(demoMode->proj_camera_main,
		demoMode->obj_camera_main->sceneHierarchyIndex, 0,
		demoMode->projectorData, demoMode->projectorMatrixStack, demoMode->sceneObject);

	// initialize point light pointers
	// (none)

	// initialize cameras dependent on viewport
	demoMode->proj_camera_main->dataPtr->aspect = demoState->frameAspect;
	a3demo_updateProjector(demoMode->proj_camera_main);

	// initialize cameras not dependent on viewport
	// (none)
}


void a3intro_load(a3_DemoState const* demoState, a3_DemoMode0_Intro* demoMode)
{
	a3_SceneObjectComponent const* sceneObject = 0, *sceneObject_parent = 0;
	a3_SceneObjectData* sceneObjectData = 0;

	a3_ProjectorComponent const* projector = 0;
	a3_ProjectorData* projectorData = 0;

	a3_PointLightComponent const* pointLight = 0;
	a3_PointLightData* pointLightData = 0;

	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	const a3real sceneCameraAxisPos = 10.0f;
	const a3vec3 sceneCameraStartPos = {
		+sceneCameraAxisPos,
		-sceneCameraAxisPos,
		+sceneCameraAxisPos + 5.0f,
	};
	const a3vec3 sceneCameraStartEuler = {
		+60.0f,
		+0.0f,
		+45.0f,
	};
	const a3f32 sceneObjectDistance = 8.0f;
	const a3f32 sceneObjectHeight = 2.0f;


	// set up scene hierarchy and scene objects
	a3hierarchyCreate(demoMode->hierarchy_scene, introMaxCount_sceneObject, 0);

	sceneObject = sceneObject_parent = demoMode->objgroup_world_root;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, -1, "objgroup_world_root");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);

	sceneObject = demoMode->obj_camera_main;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_camera_main");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.xyz = sceneCameraStartPos;
	sceneObjectData->euler.xyz = sceneCameraStartEuler;

	sceneObject = demoMode->obj_skybox;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_skybox");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->scale.x = a3real_oneeighty;
	sceneObjectData->scaleMode = a3scale_uniform;

	sceneObject = demoMode->obj_sphere;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_sphere");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = +a3real_six;
	sceneObjectData->position.y = a3real_zero;
	sceneObjectData->position.z = +a3real_four;
	sceneObjectData->scale.x = a3real_two;
	sceneObjectData->scaleMode = a3scale_uniform;

	sceneObject = demoMode->obj_cylinder;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_cylinder");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = +a3real_three;
	sceneObjectData->position.y = +a3real_five;
	sceneObjectData->position.z = +a3real_four;
	sceneObjectData->scale.x = a3real_four;
	sceneObjectData->scale.y = a3real_onehalf;
	sceneObjectData->scale.z = a3real_onehalf;
	sceneObjectData->scaleMode = a3scale_nonuniform;

	sceneObject = demoMode->obj_capsule;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_capsule");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = -a3real_three;
	sceneObjectData->position.y = +a3real_five;
	sceneObjectData->position.z = +a3real_four;
	sceneObjectData->scale.x = a3real_two;
	sceneObjectData->scaleMode = a3scale_uniform;

	sceneObject = demoMode->obj_torus;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_torus");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = -a3real_six;
	sceneObjectData->position.y = a3real_zero;
	sceneObjectData->position.z = +a3real_four;
	sceneObjectData->scale.x = a3real_two;
	sceneObjectData->scaleMode = a3scale_uniform;

	sceneObject = demoMode->obj_cone;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_cone");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = -a3real_three;
	sceneObjectData->position.y = -a3real_five;
	sceneObjectData->position.z = +a3real_four;
	sceneObjectData->scale.x = a3real_three;
	sceneObjectData->scale.y = a3real_two;
	sceneObjectData->scale.z = a3real_two;
	sceneObjectData->scaleMode = a3scale_nonuniform;

	sceneObject = demoMode->obj_teapot;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_teapot");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.x = +a3real_three;
	sceneObjectData->position.y = -a3real_five;
	sceneObjectData->position.z = +a3real_four;
	sceneObjectData->scale.x = a3real_one;
	sceneObjectData->scaleMode = a3scale_uniform;

	sceneObject = demoMode->obj_ground;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_ground");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.z = -a3real_two;
	sceneObjectData->scale.x = a3real_fortyfive;
	sceneObjectData->scale.y = a3real_fortyfive;
	sceneObjectData->scale.z = a3real_epsilon;
	sceneObjectData->scaleMode = a3scale_nonuniform;


	// set up projectors
	projector = demoMode->proj_camera_main;
	projectorData = projector->dataPtr;
	projectorData->fovy = a3real_fortyfive;
	projectorData->znear = a3real_one;
	projectorData->zfar = a3real_threesixty;
	projectorData->ctrlMoveSpeed = a3real_one;
	projectorData->ctrlRotateSpeed = a3real_two;
	projectorData->ctrlZoomSpeed = a3real_fortyfive;
	projectorData->perspective = a3true;
	a3demo_updateProjector(projector);


	// set up lights
	pointLightData = demoMode->pointLightData + 0;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos = demoMode->proj_camera_main->sceneObjectPtr->dataPtr->position;
	a3demo_setPointLightRadius(pointLightData, a3real_onehundred);

	pointLightData = demoMode->pointLightData + 1;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos.x = +a3real_four;
	pointLightData->worldPos.y = +a3real_four;
	pointLightData->worldPos.z = +a3real_four;
	pointLightData->color.g = a3real_half;
	pointLightData->color.b = a3real_half;
	a3demo_setPointLightRadius(pointLightData, a3real_fortyfive);

	pointLightData = demoMode->pointLightData + 2;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos.x = -a3real_four;
	pointLightData->worldPos.y = -a3real_four;
	pointLightData->worldPos.z = +a3real_four;
	pointLightData->color.b = a3real_half;
	pointLightData->color.r = a3real_half;
	a3demo_setPointLightRadius(pointLightData, a3real_fortyfive);

	pointLightData = demoMode->pointLightData + 3;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos.z = +a3real_six;
	pointLightData->color.r = a3real_half;
	pointLightData->color.g = a3real_half;
	a3demo_setPointLightRadius(pointLightData, a3real_sixty);
}


//-----------------------------------------------------------------------------
