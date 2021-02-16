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
	
	a3_DemoSceneObject.c
	Example of demo utility source file.
*/

#include "../a3_DemoSceneObject.h"

#include "../a3_DemoRenderUtils.h"


//-----------------------------------------------------------------------------
// GENERAL UTILITIES

inline void a3demo_applyRotation_internal(a3_SceneObjectComponent const* sceneObject, a3boolean const useZYX)
{
	if (useZYX)
		a3real4x4SetRotateZYX(sceneObject->modelMatrixStackPtr->modelMat.m,
			sceneObject->dataPtr->euler.x, sceneObject->dataPtr->euler.y, sceneObject->dataPtr->euler.z);
	else
		a3real4x4SetRotateXYZ(sceneObject->modelMatrixStackPtr->modelMat.m,
			sceneObject->dataPtr->euler.x, sceneObject->dataPtr->euler.y, sceneObject->dataPtr->euler.z);
}

inline void a3demo_applyPosition_internal(a3_SceneObjectComponent const* sceneObject)
{
	sceneObject->modelMatrixStackPtr->modelMat.v3.xyz = sceneObject->dataPtr->position.xyz;
	sceneObject->modelMatrixStackPtr->modelMat.m33 = a3real_one;
}

inline void a3demo_applyScaleAndInvert_internal(a3_SceneObjectComponent const* sceneObject)
{
	switch (sceneObject->dataPtr->scaleMode)
	{
	case a3scale_disable:
		// do not scale, quick invert
		a3real4x4TransformInverseIgnoreScale(
			sceneObject->modelMatrixStackPtr->modelMatInverse.m,
			sceneObject->modelMatrixStackPtr->modelMat.m);
		break;
	case a3scale_uniform:
		// scale entire model matrix, uniform invert
		a3real3MulS(sceneObject->modelMatrixStackPtr->modelMat.m[0],
			sceneObject->dataPtr->scale.x);
		a3real3MulS(sceneObject->modelMatrixStackPtr->modelMat.m[1],
			sceneObject->dataPtr->scale.x);
		a3real3MulS(sceneObject->modelMatrixStackPtr->modelMat.m[2],
			sceneObject->dataPtr->scale.x);
		a3real4x4TransformInverseUniformScale(
			sceneObject->modelMatrixStackPtr->modelMatInverse.m,
			sceneObject->modelMatrixStackPtr->modelMat.m);
		break;
	case a3scale_nonuniform:
		// scale individual columns, full invert
		a3real3MulS(sceneObject->modelMatrixStackPtr->modelMat.m[0],
			sceneObject->dataPtr->scale.x);
		a3real3MulS(sceneObject->modelMatrixStackPtr->modelMat.m[1],
			sceneObject->dataPtr->scale.y);
		a3real3MulS(sceneObject->modelMatrixStackPtr->modelMat.m[2],
			sceneObject->dataPtr->scale.z);
		a3real4x4TransformInverse(
			sceneObject->modelMatrixStackPtr->modelMatInverse.m,
			sceneObject->modelMatrixStackPtr->modelMat.m);
		break;
	}
}


//-----------------------------------------------------------------------------

extern inline void a3demo_resetModelMatrixStack(a3_ModelMatrixStack* modelMatrixStack)
{
	modelMatrixStack->modelMat = modelMatrixStack->modelMatInverse = a3mat4_identity;
	modelMatrixStack->modelMatInverseTranspose = a3mat4_identity;
	modelMatrixStack->modelViewMat = modelMatrixStack->modelViewMatInverse = a3mat4_identity;
	modelMatrixStack->modelViewMatInverseTranspose = a3mat4_identity;
	modelMatrixStack->modelViewProjectionMat = a3mat4_identity;
	modelMatrixStack->atlasMat = a3mat4_identity;
}

extern inline void a3demo_resetProjectorMatrixStack(a3_ProjectorMatrixStack* projectorMatrixStack)
{
	projectorMatrixStack->projectionMat = projectorMatrixStack->projectionMatInverse = a3mat4_identity;
	projectorMatrixStack->projectionBiasMat = projectorMatrixStack->projectionBiasMatInverse = a3mat4_identity;
	projectorMatrixStack->viewProjectionMat = projectorMatrixStack->viewProjectionMatInverse = a3mat4_identity;
	projectorMatrixStack->viewProjectionBiasMat = projectorMatrixStack->viewProjectionBiasMatInverse = a3mat4_identity;
}

extern inline void a3demo_resetSceneObjectData(a3_SceneObjectData* sceneObjectData)
{
	sceneObjectData->position = a3vec4_w;
	sceneObjectData->euler = a3vec4_zero;
	sceneObjectData->scale = a3vec3_one;
	sceneObjectData->scaleMode = a3scale_disable;
}

extern inline void a3demo_resetProjectorData(a3_ProjectorData* projectorData)
{
	projectorData->fovy = a3real_ninety;
	projectorData->aspect = a3real_one;
	projectorData->znear = -a3real_one;
	projectorData->zfar = +a3real_one;
	projectorData->ctrlMoveSpeed = projectorData->ctrlRotateSpeed = projectorData->ctrlZoomSpeed = a3real_zero;
	projectorData->perspective = a3false;
}

extern inline void a3demo_resetPointLightData(a3_PointLightData* pointLightData)
{
	pointLightData->position = pointLightData->worldPos = a3vec4_w;
	pointLightData->color = a3vec4_one;
	pointLightData->radius = pointLightData->radiusSq = pointLightData->radiusInv = pointLightData->radiusInvSq = a3real_one;
}

extern inline void a3demo_setPointLightRadius(a3_PointLightData* pointLightData, a3real const radius)
{
	if (radius > a3real_zero)
	{
		pointLightData->radius = radius;
		pointLightData->radiusSq = radius * radius;
		pointLightData->radiusInv = a3recip(pointLightData->radius);
		pointLightData->radiusInvSq = a3recip(pointLightData->radiusSq);
	}
	else
	{
		pointLightData->radius = pointLightData->radiusSq = pointLightData->radiusInv = pointLightData->radiusInvSq = a3real_zero;
	}
}


extern inline void a3demo_initSceneObject(a3_SceneObjectComponent* sceneObject, a3ui32 const sceneHierarchyIndex, a3_SceneObjectData* sceneObjectDataArray, a3_ModelMatrixStack* modelMatrixStackArray)
{
	sceneObject->sceneHierarchyIndex = sceneHierarchyIndex;
	sceneObject->dataPtr = sceneObjectDataArray + sceneHierarchyIndex;
	sceneObject->modelMatrixStackPtr = modelMatrixStackArray + sceneHierarchyIndex;
}

extern inline void a3demo_initProjector(a3_ProjectorComponent* projector, a3ui32 const sceneHierarchyIndex, a3ui32 const projectorIndex, a3_ProjectorData* projectorDataArray, a3_ProjectorMatrixStack* projectorMatrixStackArray, a3_SceneObjectComponent const* sceneObjectArray)
{
	projector->sceneHierarchyIndex = sceneHierarchyIndex;
	projector->projectorIndex = projectorIndex;
	projector->dataPtr = projectorDataArray + projectorIndex;
	projector->projectorMatrixStackPtr = projectorMatrixStackArray + projectorIndex;
	projector->sceneObjectPtr = sceneObjectArray + sceneHierarchyIndex;
}

extern inline void a3demo_initPointLight(a3_PointLightComponent* pointLight, a3ui32 const sceneHierarchyIndex, a3ui32 const pointLightIndex, a3_PointLightData* pointLightDataArray, a3_SceneObjectComponent const* sceneObjectArray)
{
	pointLight->sceneHierarchyIndex = sceneHierarchyIndex;
	pointLight->pointLightIndex = pointLightIndex;
	pointLight->dataPtr = pointLightDataArray + pointLightIndex;
	pointLight->sceneObjectPtr = sceneObjectArray + pointLightIndex;
}


extern inline void a3demo_rotateProjectorSceneObject(a3_ProjectorComponent const* projector, const a3real deltaX, const a3real deltaY, const a3real deltaZ)
{
	if (deltaX || deltaY || deltaZ)
	{
		// validate angles so they don't get zero'd out (trig functions have a limit)
		projector->sceneObjectPtr->dataPtr->euler.x = a3trigValid_sind(
			projector->sceneObjectPtr->dataPtr->euler.x + projector->dataPtr->ctrlRotateSpeed * deltaX);
		projector->sceneObjectPtr->dataPtr->euler.y = a3trigValid_sind(
			projector->sceneObjectPtr->dataPtr->euler.y + projector->dataPtr->ctrlRotateSpeed * deltaY);
		projector->sceneObjectPtr->dataPtr->euler.z = a3trigValid_sind(
			projector->sceneObjectPtr->dataPtr->euler.z + projector->dataPtr->ctrlRotateSpeed * deltaZ);
	}
}

extern inline void a3demo_moveProjectorSceneObject(a3_ProjectorComponent const* projector, const a3real deltaX, const a3real deltaY, const a3real deltaZ)
{
	a3real3 delta[3];
	if (deltaX || deltaY || deltaZ)
	{
		// account for orientation of object
		a3real3ProductS(delta[0], projector->sceneObjectPtr->modelMatrixStackPtr->modelMat.m[0], deltaX);
		a3real3ProductS(delta[1], projector->sceneObjectPtr->modelMatrixStackPtr->modelMat.m[1], deltaY);
		a3real3ProductS(delta[2], projector->sceneObjectPtr->modelMatrixStackPtr->modelMat.m[2], deltaZ);

		// add the 3 deltas together
		a3real3Add(delta[0], a3real3Add(delta[1], delta[2]));

		// normalize and scale by speed
		a3real3MulS(delta[0], projector->dataPtr->ctrlMoveSpeed * a3real3LengthInverse(delta[0]));

		// add delta to current
		a3real3Add(projector->sceneObjectPtr->dataPtr->position.v, delta[0]);
	}
}

extern inline void a3demo_updateSceneObject(a3_SceneObjectComponent const* sceneObject, const a3boolean useZYX)
{
	a3demo_applyRotation_internal(sceneObject, useZYX);
	a3demo_applyPosition_internal(sceneObject);
	a3demo_applyScaleAndInvert_internal(sceneObject);
}

extern inline void a3demo_updateProjector(a3_ProjectorComponent const* projector)
{
	if (projector->dataPtr->perspective)
		a3real4x4MakePerspectiveProjection(
			projector->projectorMatrixStackPtr->projectionMat.m,
			projector->projectorMatrixStackPtr->projectionMatInverse.m,
			projector->dataPtr->fovy, projector->dataPtr->aspect, projector->dataPtr->znear, projector->dataPtr->zfar);
	else
		a3real4x4MakeOrthographicProjection(
			projector->projectorMatrixStackPtr->projectionMat.m,
			projector->projectorMatrixStackPtr->projectionMatInverse.m,
			projector->dataPtr->fovy * projector->dataPtr->aspect, projector->dataPtr->fovy, projector->dataPtr->znear, projector->dataPtr->zfar);
}

extern inline void a3demo_updateProjectorViewMats(a3_ProjectorComponent const* projector)
{
	// view-projection = P * V = P * M^1
	a3real4x4Product(
		projector->projectorMatrixStackPtr->viewProjectionMat.m,
		projector->projectorMatrixStackPtr->projectionMat.m,
		projector->sceneObjectPtr->modelMatrixStackPtr->modelMatInverse.m);

	// view-projection inverse = V^-1 * P^-1 = M * P^-1
	a3real4x4Product(
		projector->projectorMatrixStackPtr->viewProjectionMatInverse.m,
		projector->sceneObjectPtr->modelMatrixStackPtr->modelMat.m,
		projector->projectorMatrixStackPtr->projectionMatInverse.m);
}

extern inline void a3demo_updateSceneObjectStack(a3_SceneObjectComponent const* sceneObject, a3_ProjectorComponent const* projector_active)
{
	// model inverse-transpose
	a3demo_quickTransposedZeroBottomRow(
		sceneObject->modelMatrixStackPtr->modelMatInverseTranspose.m,
		sceneObject->modelMatrixStackPtr->modelMatInverse.m);

	// model-view = V_proj * M = M_proj^-1 * M
	a3real4x4ProductTransform(
		sceneObject->modelMatrixStackPtr->modelViewMat.m,
		projector_active->sceneObjectPtr->modelMatrixStackPtr->modelMatInverse.m,
		sceneObject->modelMatrixStackPtr->modelMat.m);

	// model-view inverse = M^-1 * V_proj^-1 = M^-1 * M_proj
	a3real4x4ProductTransform(
		sceneObject->modelMatrixStackPtr->modelViewMatInverse.m,
		sceneObject->modelMatrixStackPtr->modelMatInverse.m,
		projector_active->sceneObjectPtr->modelMatrixStackPtr->modelMat.m);
	
	// model-view inverse-transpose
	a3demo_quickTransposedZeroBottomRow(
		sceneObject->modelMatrixStackPtr->modelViewMatInverseTranspose.m,
		sceneObject->modelMatrixStackPtr->modelViewMatInverse.m);

	// model-view-projection = P_proj * (MV)
	a3real4x4Product(
		sceneObject->modelMatrixStackPtr->modelViewProjectionMat.m,
		projector_active->projectorMatrixStackPtr->projectionMat.m,
		sceneObject->modelMatrixStackPtr->modelViewMat.m);
}

extern inline void a3demo_updateProjectorBiasMats(a3_ProjectorComponent const* projector, a3mat4 const bias, a3mat4 const biasInv)
{
	// projection-bias
	a3real4x4Product(
		projector->projectorMatrixStackPtr->projectionBiasMat.m,
		bias.m, projector->projectorMatrixStackPtr->projectionMat.m);

	// projection-bias inverse
	a3real4x4Product(
		projector->projectorMatrixStackPtr->projectionBiasMatInverse.m,
		projector->projectorMatrixStackPtr->projectionMatInverse.m, biasInv.m);

	// view-projection-bias
	a3real4x4Product(
		projector->projectorMatrixStackPtr->viewProjectionBiasMat.m,
		bias.m, projector->projectorMatrixStackPtr->viewProjectionMat.m);

	// view-projection-bias inverse
	a3real4x4Product(
		projector->projectorMatrixStackPtr->viewProjectionBiasMatInverse.m,
		projector->projectorMatrixStackPtr->viewProjectionMatInverse.m, biasInv.m);
}


//-----------------------------------------------------------------------------
