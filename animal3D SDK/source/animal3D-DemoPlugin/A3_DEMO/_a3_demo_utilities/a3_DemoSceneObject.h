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
	
	a3_DemoSceneObject.h
	Example of demo utility header file.
*/

#ifndef __ANIMAL3D_DEMOSCENEOBJECT_H
#define __ANIMAL3D_DEMOSCENEOBJECT_H


// math library
#include "animal3D-A3DM/animal3D-A3DM.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ModelMatrixStack						a3_ModelMatrixStack;
typedef struct a3_ProjectorMatrixStack					a3_ProjectorMatrixStack;

typedef enum a3_ScaleMode								a3_ScaleMode;
typedef struct a3_SceneObjectData						a3_SceneObjectData;
typedef struct a3_PointLightData						a3_PointLightData;
typedef struct a3_ProjectorData							a3_ProjectorData;
typedef struct a3_SceneObjectComponent					a3_SceneObjectComponent;
typedef struct a3_PointLightComponent					a3_PointLightComponent;
typedef struct a3_ProjectorComponent					a3_ProjectorComponent;
#endif	// __cplusplus

	
//-----------------------------------------------------------------------------

// matrix stack for a single scene object/model
struct a3_ModelMatrixStack
{
	a3mat4 modelMat;						// model matrix (object -> world)
	a3mat4 modelMatInverse;					// model inverse matrix (world -> object)
	a3mat4 modelMatInverseTranspose;		// model inverse-transpose matrix (object -> world skewed)
	a3mat4 modelViewMat;					// model-view matrix (object -> viewer)
	a3mat4 modelViewMatInverse;				// model-view inverse matrix (viewer -> object)
	a3mat4 modelViewMatInverseTranspose;	// model-view inverse transpose matrix (object -> viewer skewed)
	a3mat4 modelViewProjectionMat;			// model-view-projection matrix (object -> clip)
	a3mat4 atlasMat;						// atlas matrix (texture -> cell)
};

// matrix stack for a viewer object
struct a3_ProjectorMatrixStack
{
	a3mat4 projectionMat;					// projection matrix (viewer -> clip)
	a3mat4 projectionMatInverse;			// projection inverse matrix (clip -> viewer)
	a3mat4 projectionBiasMat;				// projection-bias matrix (viewer -> biased clip)
	a3mat4 projectionBiasMatInverse;		// projection-bias inverse matrix (biased clip -> viewer)
	a3mat4 viewProjectionMat;				// view-projection matrix (world -> clip)
	a3mat4 viewProjectionMatInverse;		// view-projection inverse matrix (clip -> world)
	a3mat4 viewProjectionBiasMat;			// view projection-bias matrix (world -> biased clip)
	a3mat4 viewProjectionBiasMatInverse;	// view-projection-bias inverse matrix (biased clip -> world)
};

// named scale modes
enum a3_ScaleMode
{
	a3scale_disable,
	a3scale_uniform,
	a3scale_nonuniform,
};

// scene object descriptor
struct a3_SceneObjectData
{
	a3vec4 position;		// scene position for direct control
	a3vec4 euler;			// euler angles for direct rotation control
	a3vec3 scale;			// scale (not accounted for in update, use separate utilities)
	a3_ScaleMode scaleMode;	// 0 = off; 1 = uniform; other = non-uniform (nightmare)
};
struct a3_SceneObjectComponent
{
	a3ui32 sceneHierarchyIndex;
	a3_SceneObjectData* dataPtr;
	a3_ModelMatrixStack* modelMatrixStackPtr;
};

// projector/camera/viewer
struct a3_ProjectorData
{
	a3real fovy;						// persp: vert field of view/ortho: vert size
	a3real aspect;						// aspect ratio
	a3real znear, zfar;					// near and far clipping planes
	a3real ctrlMoveSpeed;				// how fast controlled camera moves
	a3real ctrlRotateSpeed;				// control rotate speed (degrees)
	a3real ctrlZoomSpeed;				// control zoom speed (degrees)
	a3boolean perspective;				// perspective or orthographic
};
struct a3_ProjectorComponent
{
	a3ui32 sceneHierarchyIndex;
	a3ui32 projectorIndex;
	a3_ProjectorData* dataPtr;
	a3_ProjectorMatrixStack* projectorMatrixStackPtr;
	a3_SceneObjectComponent const* sceneObjectPtr;
};

// simple point light
struct a3_PointLightData
{
	a3vec4 position;					// position in rendering target space
	a3vec4 worldPos;					// original position in world space
	a3vec4 color;						// RGB color with padding
	a3real radius;						// radius (distance of effect from center)
	a3real radiusSq;					// radius squared (if needed)
	a3real radiusInv;					// radius inverse (attenuation factor)
	a3real radiusInvSq;					// radius inverse squared (attenuation factor)
};
struct a3_PointLightComponent
{
	a3ui32 sceneHierarchyIndex;
	a3ui32 pointLightIndex;
	a3_PointLightData* dataPtr;
	a3_SceneObjectComponent const* sceneObjectPtr;
};


//-----------------------------------------------------------------------------

inline void a3demo_resetModelMatrixStack(a3_ModelMatrixStack* modelMatrixStack);
inline void a3demo_resetProjectorMatrixStack(a3_ProjectorMatrixStack* projectionMatrixStack);
inline void a3demo_resetSceneObjectData(a3_SceneObjectData* sceneObjectData);
inline void a3demo_resetProjectorData(a3_ProjectorData* projectorData);
inline void a3demo_resetPointLightData(a3_PointLightData* pointLightData);

inline void a3demo_setPointLightRadius(a3_PointLightData* pointLightData, a3real const radius);

inline void a3demo_initSceneObject(a3_SceneObjectComponent* sceneObject, a3ui32 const sceneHierarchyIndex, a3_SceneObjectData* sceneObjectDataArray, a3_ModelMatrixStack* modelMatrixStackArray);
inline void a3demo_initProjector(a3_ProjectorComponent* projector, a3ui32 const sceneHierarchyIndex, a3ui32 const projectorIndex, a3_ProjectorData* projectorDataArray, a3_ProjectorMatrixStack* projectorMatrixStackArray, a3_SceneObjectComponent const* sceneObjectArray);
inline void a3demo_initPointLight(a3_PointLightComponent* pointLight, a3ui32 const sceneHierarchyIndex, a3ui32 const pointLightIndex, a3_PointLightData* pointLightDataArray, a3_SceneObjectComponent const* sceneObjectArray);

// update rotation of data only
inline void a3demo_rotateProjectorSceneObject(a3_ProjectorComponent const* projector, const a3real deltaX, const a3real deltaY, const a3real deltaZ);

// update position of data only
//	(uses stack to align movement with current orientation)
inline void a3demo_moveProjectorSceneObject(a3_ProjectorComponent const* projector, const a3real deltaX, const a3real deltaY, const a3real deltaZ);

// update model matrix and inverse only using object's transformation data
inline void a3demo_updateSceneObject(a3_SceneObjectComponent const* sceneObject, const a3boolean useZYX);

// update projection and inverse matrices only
inline void a3demo_updateProjector(a3_ProjectorComponent const* projector);

// update view-projection and inverse matrices only given projector with updated projection matrices
//	(uses its camera object with updated model matrices)
inline void a3demo_updateProjectorViewMats(a3_ProjectorComponent const* projector);

// update scene object's full stack given scene object with updated model matrices and reference projector with updated projection matrix
inline void a3demo_updateSceneObjectStack(a3_SceneObjectComponent const* sceneObject, a3_ProjectorComponent const* projector_active);

// update projection bias matrices given projector with updated projection and view-projection matrices
inline void a3demo_updateProjectorBiasMats(a3_ProjectorComponent const* projector, a3mat4 const bias, a3mat4 const biasInv);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__ANIMAL3D_DEMOSCENEOBJECT_H