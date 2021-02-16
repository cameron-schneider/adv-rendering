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

	a3_DemoMode0_Intro.h
	Demo mode interface: animation editor scene.

	********************************************
	*** THIS IS ONE DEMO MODE'S HEADER FILE  ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOMODE0_INTRO_H
#define __ANIMAL3D_DEMOMODE0_INTRO_H


//-----------------------------------------------------------------------------

#include "_a3_demo_utilities/a3_DemoSceneObject.h"

#include "_animation/a3_Hierarchy.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_DemoMode0_Intro						a3_DemoMode0_Intro;
typedef enum a3_DemoMode0_Intro_RenderMode				a3_DemoMode0_Intro_RenderMode;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// render modes
enum a3_DemoMode0_Intro_RenderMode
{
	intro_renderModeSolid,		// solid color
	intro_renderModeTexture,	// flat texture
	intro_renderModeLambert,	// Lambertian shading model
	intro_renderModePhong,		// Phong shading model

	intro_renderMode_max
};


// maximum unique objects
enum a3_DemoMode0_Intro_ObjectMaxCount
{
	introMaxCount_sceneObject = 16,
	introMaxCount_projector = 1,	// how many of the above behave as projectors
	introMaxCount_pointLight = 4,	// how many of the above behave as lights
};


//-----------------------------------------------------------------------------

// demo mode for basic shading
struct a3_DemoMode0_Intro
{
	// render mode
	a3_DemoMode0_Intro_RenderMode renderMode;

	// scene hierarchy and data
	a3_Hierarchy hierarchy_scene[1];

	// scene object components and related data
	union {
		a3_SceneObjectComponent sceneObject[introMaxCount_sceneObject];
		struct {
			a3_SceneObjectComponent
				objgroup_world_root[1],
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
	a3_SceneObjectData sceneObjectData[introMaxCount_sceneObject];
	a3_ModelMatrixStack modelMatrixStack[introMaxCount_sceneObject];

	// projector components and related data
	union {
		a3_ProjectorComponent projector[introMaxCount_projector];
		struct {
			a3_ProjectorComponent
				proj_camera_main[1];
		};
	};
	a3_ProjectorData projectorData[introMaxCount_projector];
	a3_ProjectorMatrixStack projectorMatrixStack[introMaxCount_projector];

	// light components and related data
	//union {
	//	// (no components)
	//};
	a3_PointLightData pointLightData[introMaxCount_pointLight];
};


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOMODE0_INTRO_H