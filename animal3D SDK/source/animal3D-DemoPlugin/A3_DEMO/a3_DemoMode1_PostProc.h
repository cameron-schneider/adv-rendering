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

	a3_DemoMode1_PostProc.h
	Demo mode interface: animation editor scene.

	********************************************
	*** THIS IS ONE DEMO MODE'S HEADER FILE  ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOMODE1_POSTPROC_H
#define __ANIMAL3D_DEMOMODE1_POSTPROC_H


//-----------------------------------------------------------------------------

#include "_a3_demo_utilities/a3_DemoSceneObject.h"

#include "_animation/a3_Hierarchy.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_DemoMode1_PostProc						a3_DemoMode1_PostProc;
typedef enum a3_DemoMode1_PostProc_RenderMode				a3_DemoMode1_PostProc_RenderMode;
typedef enum a3_DemoMode1_PostProc_RenderPass				a3_DemoMode1_PostProc_RenderPass;
typedef enum a3_DemoMode1_PostProc_RenderTarget				a3_DemoMode1_PostProc_RenderTarget;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// render modes
enum a3_DemoMode1_PostProc_RenderMode
{
	postproc_renderModePhong,			// Phong shading model
	postproc_renderModePhongSM,			// Phong shading model with shadow mapping

	postproc_renderMode_max
};


// render passes
enum a3_DemoMode1_PostProc_RenderPass
{
	postproc_renderPassShadow,
	postproc_renderPassScene,
	postproc_renderPassBright2,
	postproc_renderPassBlurH2,
	postproc_renderPassBlurV2,
	postproc_renderPassBright4,
	postproc_renderPassBlurH4,
	postproc_renderPassBlurV4,
	postproc_renderPassBright8,
	postproc_renderPassBlurH8,
	postproc_renderPassBlurV8,
	postproc_renderPassDisplay,

	postproc_renderPass_max
};


// render targets
enum a3_DemoMode1_PostProc_RenderTarget
{
	// scene targets
	postproc_renderTargetSceneColor = 0,
	postproc_renderTargetSceneDepth,
	postproc_renderTargetScene_max,

	// post-processing targets
	postproc_renderTargetPostColor = 0,
	postproc_renderTargetPost_max,

	// shadow pass targets
	postproc_renderTargetShadowDepth = 0,
	postproc_renderTargetShadow_max,
};


// maximum unique objects
enum a3_DemoMode1_PostProc_ObjectMaxCount
{
	postprocMaxCount_sceneObject = 16,
	postprocMaxCount_projector = 2,		// how many of the above behave as projectors
	postprocMaxCount_pointLight = 4,	// how many of the above behave as lights
};


//-----------------------------------------------------------------------------

// demo mode for basic shading
struct a3_DemoMode1_PostProc
{
	// render mode
	a3_DemoMode1_PostProc_RenderMode renderMode;

	// render pass
	a3_DemoMode1_PostProc_RenderPass renderPass;

	// render targets
	a3_DemoMode1_PostProc_RenderTarget renderTarget[postproc_renderPass_max], renderTargetCount[postproc_renderPass_max];

	// scene hierarchy and data
	a3_Hierarchy hierarchy_scene[1];

	// scene object components and related data
	union {
		a3_SceneObjectComponent sceneObject[postprocMaxCount_sceneObject];
		struct {
			a3_SceneObjectComponent
				objgroup_world_root[1],
				obj_light_main[1],
				obj_camera_main[1];
			a3_SceneObjectComponent
				obj_skybox[1],
				obj_sphere[1],
				obj_cylinder[1],
				obj_capsule[1],
				obj_torus[1],
				obj_cone[1],
				obj_teapot[1],
				obj_ground[1];
		};
	};
	a3_SceneObjectData sceneObjectData[postprocMaxCount_sceneObject];
	a3_ModelMatrixStack modelMatrixStack[postprocMaxCount_sceneObject];

	// projector components and related data
	union {
		a3_ProjectorComponent projector[postprocMaxCount_projector];
		struct {
			a3_ProjectorComponent
				proj_camera_main[1];
			a3_ProjectorComponent
				proj_light_main[1];
		};
	};
	a3_ProjectorData projectorData[postprocMaxCount_projector];
	a3_ProjectorMatrixStack projectorMatrixStack[postprocMaxCount_projector];

	// light components and related data
	union {
		a3_PointLightComponent pointLight[postprocMaxCount_pointLight];
		struct {
			a3_PointLightComponent
				light_point_main[1];
		};
	};
	a3_PointLightData pointLightData[postprocMaxCount_pointLight];
};


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOMODE1_POSTPROC_H