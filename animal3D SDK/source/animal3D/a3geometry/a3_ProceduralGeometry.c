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
	
	a3_ProceduralGeometry.c
	Definitions for procedural geometry generation.

	**DO NOT MODIFY THIS FILE**
*/

#include "animal3D/a3geometry/a3_ProceduralGeometry.h"

#include "animal3D-A3DM/a3math/a3vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#include <assert.h>
#else	// NDEBUG
#define assert( expr )	( (void)0 )
#endif	// !NDEBUG


//-----------------------------------------------------------------------------
// internal function prototypes

typedef a3i32(*a3proceduralGenerateFunc)(a3_GeometryData *, const a3_ProceduralGeometryDescriptor *);


// internal flags
enum a3_ProceduralGeometryFlagInternal
{
	a3geomFlag_tangentFlag = 4,
};

// other useful constants
enum a3_ProceduralGeometryConstInternal
{
	tElems = 2,
	vElems = 3,
};


// generate geometry
a3ret a3proceduralInternalGenerateTriangle(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateCircle(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGeneratePlane(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGeneratePyramid(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateOctahedron(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateBox(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateSemisphere(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateSphere(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateCone(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateDiamond(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateCylinder(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateCapsule(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateTorus(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);
a3ret a3proceduralInternalGenerateAxes(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);

// dummy for the ones that are not finished
a3ret a3proceduralInternalGenerateNothing(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	return -1;
}


//-----------------------------------------------------------------------------
// internal function selector

inline a3ret a3proceduralInternalGenerateData(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	const a3proceduralGenerateFunc generateFuncList[] = {
		a3proceduralInternalGenerateNothing,	// filler
		a3proceduralInternalGenerateTriangle,
		a3proceduralInternalGenerateCircle,
		a3proceduralInternalGeneratePlane,
		a3proceduralInternalGeneratePyramid,
		a3proceduralInternalGenerateOctahedron,
		a3proceduralInternalGenerateBox,
		a3proceduralInternalGenerateSemisphere,
		a3proceduralInternalGenerateSphere,
		a3proceduralInternalGenerateCone,
		a3proceduralInternalGenerateDiamond,
		a3proceduralInternalGenerateCylinder,
		a3proceduralInternalGenerateCapsule,
		a3proceduralInternalGenerateTorus,
		a3proceduralInternalGenerateAxes,
	}, generateFunc = generateFuncList[geom->shape];
	const a3_GeometryData reset = { 0 };
	*geomData = reset;
	return generateFunc(geomData, geom);
}


//-----------------------------------------------------------------------------
// utility to freeze transforms on vertices and normals (if applicable)

inline a3ret a3proceduralInternalFreezeTransform(a3_GeometryData *geomData, const a3f32 *transform)
{
	a3ui32 i;
	a3f32 *dataPtr, *dataPtr2;
	a3f32 tmpVec[3] = { 0 };

	// apply transform to all positions
	for (i = 0, dataPtr = (a3f32 *)geomData->attribData[a3attrib_geomPosition]; 
		i < geomData->numVertices; ++i, dataPtr += 3)
	{
		tmpVec[0] = transform[0] * dataPtr[0] + transform[4] * dataPtr[1] + transform[8] * dataPtr[2] + transform[12];
		tmpVec[1] = transform[1] * dataPtr[0] + transform[5] * dataPtr[1] + transform[9] * dataPtr[2] + transform[13];
		tmpVec[2] = transform[2] * dataPtr[0] + transform[6] * dataPtr[1] + transform[10] * dataPtr[2] + transform[14];
		memcpy(dataPtr, tmpVec, sizeof(tmpVec));
	}

	// apply to normals and tangents if applicable
	if (geomData->attribData[a3attrib_geomNormal])
	{
		const a3f32 invScale0 = a3real3LengthSquaredInverse(transform + 0);
		const a3f32 invScale1 = a3real3LengthSquaredInverse(transform + 4);
		const a3f32 invScale2 = a3real3LengthSquaredInverse(transform + 8);
		a3f32 transform_nrm[12] = { 0 };

		a3real3ProductS(transform_nrm + 0, transform + 0, invScale0);
		a3real3ProductS(transform_nrm + 4, transform + 4, invScale1);
		a3real3ProductS(transform_nrm + 8, transform + 8, invScale1);

		// normals
		for (i = 0, dataPtr = (a3f32 *)geomData->attribData[a3attrib_geomNormal]; 
			i < geomData->numVertices; ++i, dataPtr += 3)
		{
			tmpVec[0] = transform_nrm[0] * dataPtr[0] + transform_nrm[4] * dataPtr[1] + transform_nrm[8] * dataPtr[2];
			tmpVec[1] = transform_nrm[1] * dataPtr[0] + transform_nrm[5] * dataPtr[1] + transform_nrm[9] * dataPtr[2];
			tmpVec[2] = transform_nrm[2] * dataPtr[0] + transform_nrm[6] * dataPtr[1] + transform_nrm[10] * dataPtr[2];
			memcpy(dataPtr, tmpVec, sizeof(tmpVec));
		}

		// tangents and bitangents
		if (geomData->attribData[a3attrib_geomTangent])
		{
			a3geometryGetAddressBitangent(&dataPtr2, geomData);
			for (i = 0, dataPtr = (a3f32 *)geomData->attribData[a3attrib_geomTangent]; 
				i < geomData->numVertices; ++i, dataPtr += 3, dataPtr2 += 3)
			{
				tmpVec[0] = transform_nrm[0] * dataPtr[0] + transform_nrm[4] * dataPtr[1] + transform_nrm[8] * dataPtr[2];
				tmpVec[1] = transform_nrm[1] * dataPtr[0] + transform_nrm[5] * dataPtr[1] + transform_nrm[9] * dataPtr[2];
				tmpVec[2] = transform_nrm[2] * dataPtr[0] + transform_nrm[6] * dataPtr[1] + transform_nrm[10] * dataPtr[2];
				memcpy(dataPtr, tmpVec, sizeof(tmpVec));

				tmpVec[0] = transform_nrm[0] * dataPtr2[0] + transform_nrm[4] * dataPtr2[1] + transform_nrm[8] * dataPtr2[2];
				tmpVec[1] = transform_nrm[1] * dataPtr2[0] + transform_nrm[5] * dataPtr2[1] + transform_nrm[9] * dataPtr2[2];
				tmpVec[2] = transform_nrm[2] * dataPtr2[0] + transform_nrm[6] * dataPtr2[1] + transform_nrm[10] * dataPtr2[2];
				memcpy(dataPtr2, tmpVec, sizeof(tmpVec));
			}
		}
	}

	return (geomData->numVertices);
}


//-----------------------------------------------------------------------------
// utility to prepare vertex and index format from flags

inline a3ui32 a3proceduralInternalCreateFormats(a3_VertexFormatDescriptor *vertexFormat, a3_IndexFormatDescriptor *indexFormat, const a3ui32 numVertices, const a3ui32 numIndices, const a3ubyte flags, const a3ubyte color)
{
	a3_GeometryVertexAttributeName attribs[a3attrib_geomNameMax + 2];
	a3ui32 numAttribs = 0;

	// enable everything indicated
	if (flags & a3geomFlag_texcoords)
		attribs[numAttribs++] = a3attrib_geomTexcoord;
	if (flags & a3geomFlag_normals)
		attribs[numAttribs++] = a3attrib_geomNormal;
	if (flags & a3geomFlag_tangentFlag)
		attribs[numAttribs++] = a3attrib_geomTangent;
	if (color)
		attribs[numAttribs++] = a3attrib_geomColor;

	// create vertex format with attributes and index format descriptor 
	//	with best sized index if needed
	a3geometryCreateVertexFormat(vertexFormat, attribs, numAttribs);
	a3geometryCreateIndexFormat(indexFormat, (numIndices ? numVertices : 0));

	// done
	return (vertexFormat->vertexSize);
}


//-----------------------------------------------------------------------------
// setters

a3ret a3proceduralSetDescriptorFlags(a3_ProceduralGeometryDescriptor *geom, const a3_ProceduralGeometryFlag flags)
{
	if (geom && geom->shape)
	{
		// store new flags and retain other flags
		geom->bParams[3] = (geom->bParams[3] & 0xf0) | (flags);
		return 1;
	}
	return -1;
}

a3ret a3proceduralSetDescriptorAxis(a3_ProceduralGeometryDescriptor *geom, const a3_ProceduralGeometryAxis axis)
{
	if (geom && geom->shape)
	{
		// confirm valid shape
		a3byte newAxis = axis;
		a3i32 ret = 1;
		switch (geom->shape)
		{
			// these shapes don't use this feature
		case a3geomShape_box:
		case a3geomShape_axes:
			newAxis = a3geomAxis_default;
			ret = 0;
		}
		geom->bParams[3] = (newAxis << 4) | (geom->bParams[3] & 0x0f);
		return ret;
	}
	return -1;
}

a3ret a3proceduralSetDescriptorFlagsAxis(a3_ProceduralGeometryDescriptor *geom, const a3_ProceduralGeometryFlag flags, const a3_ProceduralGeometryAxis axis)
{
	if (geom && geom->shape)
	{
		a3byte newAxis = axis;
		a3i32 ret = 1;
		switch (geom->shape)
		{
		case a3geomShape_box:
		case a3geomShape_axes:
			newAxis = a3geomAxis_default;
			ret = 0;
		}
		geom->bParams[3] = (newAxis << 4) | (flags);
		return ret;
	}
	return -1;
}


//-----------------------------------------------------------------------------
// generate

a3ret a3proceduralGenerateGeometryData(a3_GeometryData *geomData_out, const a3_ProceduralGeometryDescriptor *geom, const a3f32 *transform_opt)
{
	if (geomData_out && geom)
		if (!geomData_out->data && geom->shape)
		{
			const a3i32 result = a3proceduralInternalGenerateData(geomData_out, geom);
			if (result && transform_opt)
				a3proceduralInternalFreezeTransform(geomData_out, transform_opt);
			return result;
		}
	return -1;
}


//-----------------------------------------------------------------------------
// internal function prototypes

// conform vector list to axis
inline void a3proceduralInternalTransformVectors(a3f32 *vectors, const a3ui32 numVectors, const a3_ProceduralGeometryAxis axis)
{
	a3f32 swapValue = 0.0f;
	a3ui32 i = 0;

	switch (axis)
	{
	case a3geomAxis_default: 
//	case a3geomAxis_z:
		// do nothing
		break;
	case a3geomAxis_z_negative: 
		// 180 degrees about Y: Z <- -Z; X <- -X
		for (i = 0; i < numVectors; ++i, vectors += 3)
		{
			vectors[2] = -vectors[2];
			vectors[0] = -vectors[0];
		}
		break;
	case a3geomAxis_x: 
		// +90 degrees about Y: +Z -> +X; +X -> -Z
		for (i = 0; i < numVectors; ++i, vectors += 3)
		{
			swapValue = vectors[2];
			vectors[2] = -vectors[0];
			vectors[0] = swapValue;
		}
		break;
	case a3geomAxis_x_negative: 
		// -90 degrees about Y: +Z -> -X; +X -> +Z
		for (i = 0; i < numVectors; ++i, vectors += 3)
		{
			swapValue = vectors[2];
			vectors[2] = vectors[0];
			vectors[0] = -swapValue;
		}
		break;
	case a3geomAxis_y:
		// -90 degrees about X: +Z -> +Y; +Y -> -Z
		for (i = 0; i < numVectors; ++i, vectors += 3)
		{
			swapValue = vectors[2];
			vectors[2] = -vectors[1];
			vectors[1] = swapValue;
		}
		break;
	case a3geomAxis_y_negative: 
		// +90 degrees about X: +Z -> -Y; +Y -> +Z
		for (i = 0; i < numVectors; ++i, vectors += 3)
		{
			swapValue = vectors[2];
			vectors[2] = vectors[1];
			vectors[1] = -swapValue;
		}
		break;
	}
}


//-----------------------------------------------------------------------------

// general utility to store indices in buffer
inline a3ubyte *a3proceduralInternalStoreIndexStrip(a3ubyte *index, const a3ui32 indexSize, const a3ui32 count, a3ui32 *i_start_end, const a3i32 direction)
{
	a3ui32 i = *i_start_end;
	a3ui32 j;

	if (indexSize < 2)
		for (j = 0; j < count; ++j, i += direction)
			*(index++) = (a3ubyte)i;
	else if (indexSize < 4)
		for (j = 0; j < count; ++j, i += direction)
			*(((a3ui16 *)index)++) = (a3ui16)i;
	else
		for (j = 0; j < count; ++j, i += direction)
			*(((a3ui32 *)index)++) = i;

	// store output
	*i_start_end = i;
	return index;
}

// helpers to store indices
inline a3ubyte *a3proceduralInternalStoreTriangleIndicesByte(a3ubyte *index, const a3ui32 i0, const a3ui32 i1, const a3ui32 i2)
{
	index[0] = (a3ubyte)i0;
	index[1] = (a3ubyte)i1;
	index[2] = (a3ubyte)i2;
	return (index + 3);
}

inline a3ui16 *a3proceduralInternalStoreTriangleIndicesShort(a3ui16 *index, const a3ui32 i0, const a3ui32 i1, const a3ui32 i2)
{
	index[0] = (a3ui16)i0;
	index[1] = (a3ui16)i1;
	index[2] = (a3ui16)i2;
	return (index + 3);
}

inline a3ui32 *a3proceduralInternalStoreTriangleIndicesInt(a3ui32 *index, const a3ui32 i0, const a3ui32 i1, const a3ui32 i2)
{
	index[0] = i0;
	index[1] = i1;
	index[2] = i2;
	return (index + 3);
}

inline a3ubyte *a3proceduralInternalStoreTriangleIndices(a3ubyte *index, const a3ui32 indexSize, const a3ui32 i0, const a3ui32 i1, const a3ui32 i2)
{
	if (indexSize < 2)
		index = a3proceduralInternalStoreTriangleIndicesByte(index, i0, i1, i2);
	else if (indexSize < 4)
		index = (a3ubyte *)a3proceduralInternalStoreTriangleIndicesShort((a3ui16 *)index, i0, i1, i2);
	else
		index = (a3ubyte *)a3proceduralInternalStoreTriangleIndicesInt((a3ui32 *)index, i0, i1, i2);
	return index;
}


//-----------------------------------------------------------------------------
// additional utilities for implementations

// indices for shape data
enum a3_ProceduralGeometryDataIndex
{
	// a3f32 params
	fWidth = 0,
	fHeight,
	fLength,
	fLength2,
	fRadius = 0,
	fRadius2,
	fBase = fWidth,

	// byte params
	bSubdivsW = 0,
	bSubdivsH,
	bSubdivsL,
	bFlags,
	bSlices = 0,
	bSubdivsB,
	bStacks,
	bSubdivsR = bSubdivsB,
};


//-----------------------------------------------------------------------------
// external math and storage utilities

inline void a3proceduralInternalCalculateNormal(a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2);
inline void a3proceduralInternalCalculateTangentBasis(a3f32 *tangent_out, a3f32 *bitangent_out, a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2, const a3f32 *t0, const a3f32 *t1, const a3f32 *t2);
inline void a3proceduralInternalCalculateTangentBasisOrtho(a3f32 *tangent_out, a3f32 *bitangent_out, a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2, const a3f32 *t0, const a3f32 *t1, const a3f32 *t2);

inline a3ubyte *a3proceduralInternalStoreIndex(a3ubyte *index, const a3ui32 indexSize, const a3ui32 i);


//-----------------------------------------------------------------------------

#include "animal3D-A3DM/a3math/a3trig.h"
#include "animal3D-A3DM/a3math/a3sqrt.h"


// include function definitions organized into files
#define __ANIMAL3D_PROCEDURALGEOMETRY_C
#include "_internal/a3_ProceduralGeometry_Triangle.inl"
#include "_internal/a3_ProceduralGeometry_Circle.inl"
#include "_internal/a3_ProceduralGeometry_Plane.inl"
#include "_internal/a3_ProceduralGeometry_Pyramid.inl"
#include "_internal/a3_ProceduralGeometry_Octahedron.inl"
#include "_internal/a3_ProceduralGeometry_Box.inl"
#include "_internal/a3_ProceduralGeometry_Semisphere.inl"
#include "_internal/a3_ProceduralGeometry_Sphere.inl"
#include "_internal/a3_ProceduralGeometry_Cone.inl"
#include "_internal/a3_ProceduralGeometry_Diamond.inl"
#include "_internal/a3_ProceduralGeometry_Cylinder.inl"
#include "_internal/a3_ProceduralGeometry_Capsule.inl"
#include "_internal/a3_ProceduralGeometry_Torus.inl"
#include "_internal/a3_ProceduralGeometry_Axes.inl"


//-----------------------------------------------------------------------------
// removed

/*
	// A3: Create arrow descriptor.
	//	param geom_out: non-null pointer to descriptor
	//	param flags: configuration flags
	//	param axis: axis direction for shape
	//	param radiusBase: positive radius of base shaft
	//	param radiusHead: positive radius of arrowhead
	//	param lengthBase: positive length of base shaft of arrow
	//	param lengthHead: positive length of head from end of shaft to point
	//		(total length of arrow is base length + head length)
	//	param slices: number of divisions around axis; 
	//		must be 3 or greater, less than 256
	//	return: 1 if success
	//	return: 0 if failed
	//	return: -1 if invalid params
	a3ret a3proceduralCreateDescriptorArrow(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, const a3_ProceduralGeometryAxis axis, const a3f32 radiusBase, const a3f32 radiusHead, const a3f32 lengthBase, const a3f32 lengthHead, const a3ubyte slices);
*/
/*
	a3ret a3proceduralInternalSetFlagsArrow(a3_ProceduralGeometryDescriptor *geom, const a3_ProceduralGeometryFlag flags);
	a3ret a3proceduralInternalGenerateArrow(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom);

	//...
		a3proceduralInternalSetFlagsArrow,
		a3proceduralInternalGenerateArrow,
	//...

#include "_internal/a3_ProceduralGeometry_Arrow.inl"
*/

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
	
	a3_ProceduralGeometry_Arrow.inl
	Internal generation functions for Arrow.

	**DO NOT MODIFY THIS FILE**
*/
/*
#ifdef __ANIMAL3D_PROCEDURALGEOMETRY_C


a3ret a3proceduralCreateDescriptorArrow(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, const a3_ProceduralGeometryAxis axis, const a3f32 radiusBase, const a3f32 radiusHead, const a3f32 lengthBase, const a3f32 lengthHead, const a3ubyte slices)
{
	if (geom_out && !geom_out->shape)
	{

		return 1;
	}
	return -1;
}


a3ret a3proceduralInternalGenerateArrow(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{

	return 1;
}


#endif	// __ANIMAL3D_PROCEDURALGEOMETRY_C
*/


//-----------------------------------------------------------------------------
