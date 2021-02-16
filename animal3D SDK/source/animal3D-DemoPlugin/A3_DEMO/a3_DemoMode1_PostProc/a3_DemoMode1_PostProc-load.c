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

	a3_DemoMode1_PostProc-load.c
	Demo mode implementations: animation scene.

	********************************************
	*** LOADING FOR ANIMATION SCENE MODE     ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_PostProc.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"


//-----------------------------------------------------------------------------

void a3postproc_loadValidate(a3_DemoState* demoState, a3_DemoMode1_PostProc* demoMode)
{
	void a3postproc_input(a3_DemoState* demoState, a3_DemoMode1_PostProc* demoMode, a3f64 const dt);
	void a3postproc_update(a3_DemoState* demoState, a3_DemoMode1_PostProc* demoMode, a3f64 const dt);
	void a3postproc_render(a3_DemoState const* demoState, a3_DemoMode1_PostProc const* demoMode, a3f64 const dt);
	void a3postproc_input_keyCharPress(a3_DemoState const* demoState, a3_DemoMode1_PostProc* demoMode, a3i32 const asciiKey, a3i32 const state);
	void a3postproc_input_keyCharHold(a3_DemoState const* demoState, a3_DemoMode1_PostProc* demoMode, a3i32 const asciiKey, a3i32 const state);

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

	// initialize callbacks
	a3_DemoModeCallbacks* const callbacks = demoState->demoModeCallbacks + demoState_modePostProc;
	callbacks->demoMode =		demoMode;
	callbacks->handleInput =	(a3_DemoMode_EventCallback)a3postproc_input;
	callbacks->handleUpdate =	(a3_DemoMode_EventCallback)a3postproc_update;
	callbacks->handleRender =	(a3_DemoMode_EventCallback)a3postproc_render;
	callbacks->handleKeyPress = (a3_DemoMode_InputCallback)a3postproc_input_keyCharPress;
	callbacks->handleKeyHold =	(a3_DemoMode_InputCallback)a3postproc_input_keyCharHold;

	// initialize scene object pointers
	a3demo_initSceneObject(demoMode->objgroup_world_root, 0,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_light_main, 1,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_camera_main, 2,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_skybox, 3,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_sphere, 4,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_cylinder, 5,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_capsule, 6,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_torus, 7,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_cone, 8,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_teapot, 9,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);
	a3demo_initSceneObject(demoMode->obj_ground, 10,
		demoMode->sceneObjectData, demoMode->modelMatrixStack);

	// initialize projector pointers
	a3demo_initProjector(demoMode->proj_camera_main,
		demoMode->obj_camera_main->sceneHierarchyIndex, 0,
		demoMode->projectorData, demoMode->projectorMatrixStack, demoMode->sceneObject);
	a3demo_initProjector(demoMode->proj_light_main,
		demoMode->obj_light_main->sceneHierarchyIndex, 1,
		demoMode->projectorData, demoMode->projectorMatrixStack, demoMode->sceneObject);

	// initialize point light pointers
	a3demo_initPointLight(demoMode->light_point_main,
		demoMode->obj_light_main->sceneHierarchyIndex, 0,
		demoMode->pointLightData, demoMode->sceneObject);

	// initialize cameras dependent on viewport
	demoMode->proj_camera_main->dataPtr->aspect = demoState->frameAspect;
	a3demo_updateProjector(demoMode->proj_camera_main);
	a3demo_updateProjectorViewMats(demoMode->proj_camera_main);
	a3demo_updateProjectorBiasMats(demoMode->proj_camera_main, bias, biasInv);

	// initialize cameras not dependent on viewport
	a3demo_updateProjector(demoMode->proj_light_main);
	a3demo_updateProjectorViewMats(demoMode->proj_light_main);
	a3demo_updateProjectorBiasMats(demoMode->proj_light_main, bias, biasInv);
}


void a3postproc_load(a3_DemoState const* demoState, a3_DemoMode1_PostProc* demoMode)
{
	a3_SceneObjectComponent const* sceneObject = 0, *sceneObject_parent = 0;
	a3_SceneObjectData* sceneObjectData = 0;

	a3_ProjectorComponent const* projector = 0;
	a3_ProjectorData* projectorData = 0;

	a3_PointLightComponent const* pointLight = 0;
	a3_PointLightData* pointLightData = 0;

	// camera's starting orientation depends on "vertical" axis
	// we want the exact same view in either case
	const a3real sceneCameraAxisPos = 15.0f;
	const a3vec3 sceneCameraStartPos = {
		+sceneCameraAxisPos,
		-sceneCameraAxisPos,
		+sceneCameraAxisPos + 5.0f,
	};
	const a3vec3 sceneCameraStartEuler = {
		+55.0f,
		+0.0f,
		+45.0f,
	};
	const a3f32 sceneObjectDistance = 8.0f;
	const a3f32 sceneObjectHeight = 2.0f;


	// set up scene hierarchy and scene objects
	a3hierarchyCreate(demoMode->hierarchy_scene, postprocMaxCount_sceneObject, 0);

	sceneObject = sceneObject_parent = demoMode->objgroup_world_root;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, -1, "objgroup_world_root");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);

	sceneObject = demoMode->obj_light_main;
	sceneObjectData = sceneObject->dataPtr;
	a3hierarchySetNode(demoMode->hierarchy_scene,
		sceneObject->sceneHierarchyIndex, sceneObject_parent->sceneHierarchyIndex, "obj_light_main");
	a3demo_resetSceneObjectData(sceneObjectData);
	a3demo_resetModelMatrixStack(sceneObject->modelMatrixStackPtr);
	sceneObjectData->position.xyz = sceneCameraStartPos;
	sceneObjectData->euler.xyz = sceneCameraStartEuler;

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
	sceneObjectData->position.z = +a3real_two + a3real_half;
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

	projector = demoMode->proj_light_main;
	projectorData = projector->dataPtr;
	projectorData->fovy = a3real_ninety;
	projectorData->aspect = a3real_one;
	projectorData->znear = a3real_five;
	projectorData->zfar = a3real_onehundred;
	projectorData->perspective = a3true;
	a3demo_updateProjector(projector);


	// set up lights
	pointLightData = demoMode->pointLightData + 0;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos = demoMode->proj_light_main->sceneObjectPtr->dataPtr->position;
	a3demo_setPointLightRadius(pointLightData, a3real_onehundred);

	pointLightData = demoMode->pointLightData + 1;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos.x = +a3real_six;
	pointLightData->worldPos.y = +a3real_six;
	pointLightData->worldPos.z = +a3real_two;
	pointLightData->color.g = a3real_half;
	pointLightData->color.b = a3real_half;
	a3demo_setPointLightRadius(pointLightData, a3real_six);

	pointLightData = demoMode->pointLightData + 2;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos.x = -a3real_six * a3real_two;
	pointLightData->worldPos.y = -a3real_six * a3real_two;
	pointLightData->worldPos.z = +a3real_two;
	pointLightData->color.b = a3real_half;
	pointLightData->color.r = a3real_half;
	a3demo_setPointLightRadius(pointLightData, a3real_fortyfive);

	pointLightData = demoMode->pointLightData + 3;
	a3demo_resetPointLightData(pointLightData);
	pointLightData->worldPos.z = +a3real_six;
	pointLightData->color.r = a3real_half;
	pointLightData->color.g = a3real_half;
	a3demo_setPointLightRadius(pointLightData, a3real_sixty);


	// other options
	demoMode->renderMode = postproc_renderModePhongSM;
	demoMode->renderPass = postproc_renderPassScene;
	demoMode->renderTarget[postproc_renderPassShadow] = postproc_renderTargetShadowDepth;
	demoMode->renderTargetCount[postproc_renderPassShadow] = postproc_renderTargetShadow_max;
	demoMode->renderTarget[postproc_renderPassScene] = postproc_renderTargetSceneColor;
	demoMode->renderTargetCount[postproc_renderPassScene] = postproc_renderTargetScene_max;
	demoMode->renderTarget[postproc_renderPassDisplay] =
		demoMode->renderTarget[postproc_renderPassBright2] =
		demoMode->renderTarget[postproc_renderPassBlurH2] =
		demoMode->renderTarget[postproc_renderPassBlurV2] =
		demoMode->renderTarget[postproc_renderPassBright4] =
		demoMode->renderTarget[postproc_renderPassBlurH4] =
		demoMode->renderTarget[postproc_renderPassBlurV4] =
		demoMode->renderTarget[postproc_renderPassBright8] =
		demoMode->renderTarget[postproc_renderPassBlurH8] =
		demoMode->renderTarget[postproc_renderPassBlurV8] = postproc_renderTargetPostColor;
	demoMode->renderTargetCount[postproc_renderPassDisplay] =
		demoMode->renderTargetCount[postproc_renderPassBright2] =
		demoMode->renderTargetCount[postproc_renderPassBlurH2] =
		demoMode->renderTargetCount[postproc_renderPassBlurV2] =
		demoMode->renderTargetCount[postproc_renderPassBright4] =
		demoMode->renderTargetCount[postproc_renderPassBlurH4] =
		demoMode->renderTargetCount[postproc_renderPassBlurV4] =
		demoMode->renderTargetCount[postproc_renderPassBright8] =
		demoMode->renderTargetCount[postproc_renderPassBlurH8] =
		demoMode->renderTargetCount[postproc_renderPassBlurV8] = postproc_renderTargetPost_max;
}


//-----------------------------------------------------------------------------
