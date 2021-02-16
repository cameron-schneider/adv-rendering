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
	
	a3_ProceduralGeometry_Plane.inl
	Internal generation functions for Plane.

	**DO NOT MODIFY THIS FILE**
*/

#ifdef __ANIMAL3D_PROCEDURALGEOMETRY_C


//-----------------------------------------------------------------------------
// utility to generate plane (this will be useful when creating planar texture 
//	coordinate layouts, i.e. cylinder, sphere body, torus)

inline a3f32 *a3proceduralInternalCreatePlane(a3f32 *position, const a3f32 right, const a3f32 left, const a3f32 top, const a3f32 bottom, const a3f32 depth, const a3ui32 subdivsW, const a3ui32 subdivsH, const a3ui32 attribElems, const a3boolean byRow)
{
	a3ui32 sl, st;
	a3f32 x, y, dx = (right - left) / (a3f32)subdivsW, dy = (top - bottom) / (a3f32)subdivsH;

	// go row-by-row
	if (byRow)
	{
		for (st = 0, y = bottom; st <= subdivsH; y = dy*(a3f32)(++st) + bottom)
			for (sl = 0, x = left; sl <= subdivsW; x = dx*(a3f32)(++sl) + left, position += attribElems)
			{
				position[0] = x;
				position[1] = y;
				if (attribElems > 2) position[2] = depth;
			}
	}

	// column-by-column
	else
	{
		for (sl = 0, x = left; sl <= subdivsW; x = dx*(a3f32)(++sl) + left)
			for (st = 0, y = bottom; st <= subdivsH; y = dy*(a3f32)(++st) + bottom, position += attribElems)
			{
				position[0] = x;
				position[1] = y;
				if (attribElems > 2) position[2] = depth;
			}
	}

	// done
	return position;
}


//-----------------------------------------------------------------------------
// utility to generate indices for a plane arrangement; also useful for round 
//	things because it's basically a strip indexer

// do 6 index stores in triangle strip format (backwards Z)
inline a3ubyte *a3proceduralInternalStorePlaneIndicesSolidByte(a3ubyte *index, const a3ui32 hOffset, const a3ui32 vOffset, const a3ui32 i)
{
	index[0] = (a3ubyte)i;
	index[1] = index[5] = (a3ubyte)(i + hOffset);
	index[2] = index[4] = (a3ubyte)(i + vOffset);
	index[3] = (a3ubyte)(i + hOffset + vOffset);
	return (index + 6);
}

inline a3ui16 *a3proceduralInternalStorePlaneIndicesSolidShort(a3ui16 *index, const a3ui32 hOffset, const a3ui32 vOffset, const a3ui32 i)
{
	index[0] = (a3ui16)i;
	index[1] = index[5] = (a3ui16)(i + hOffset);
	index[2] = index[4] = (a3ui16)(i + vOffset);
	index[3] = (a3ui16)(i + hOffset + vOffset);
	return (index + 6);
}

inline a3ui32 *a3proceduralInternalStorePlaneIndicesSolidInt(a3ui32 *index, const a3ui32 hOffset, const a3ui32 vOffset, const a3ui32 i)
{
	index[0] = i;
	index[1] = (index[5] = i + hOffset);
	index[2] = (index[4] = i + vOffset);
	index[3] = (i + hOffset + vOffset);
	return (index + 6);
}

// square pattern
inline a3ubyte *a3proceduralInternalStorePlaneIndicesWireByte(a3ubyte *index, const a3ui32 hOffset, const a3ui32 vOffset, const a3ui32 i)
{
	index[0] = index[7] = (a3ubyte)i;
	index[2] = index[1] = (a3ubyte)(i + hOffset);
	index[4] = index[3] = (a3ubyte)(i + hOffset + vOffset);
	index[6] = index[5] = (a3ubyte)(i + vOffset);
	return (index + 8);
}

inline a3ui16 *a3proceduralInternalStorePlaneIndicesWireShort(a3ui16 *index, const a3ui32 hOffset, const a3ui32 vOffset, const a3ui32 i)
{
	index[0] = index[7] = (a3ui16)i;
	index[2] = index[1] = (a3ui16)(i + hOffset);
	index[4] = index[3] = (a3ui16)(i + hOffset + vOffset);
	index[6] = index[5] = (a3ui16)(i + vOffset);
	return (index + 8);
}

inline a3ui32 *a3proceduralInternalStorePlaneIndicesWireInt(a3ui32 *index, const a3ui32 hOffset, const a3ui32 vOffset, const a3ui32 i)
{
	index[0] = index[7] = i;
	index[2] = index[1] = (i + hOffset);
	index[4] = index[3] = (i + hOffset + vOffset);
	index[6] = index[5] = (i + vOffset);
	return (index + 8);
}

// generate indices for a solid grid
inline a3ubyte *a3proceduralInternalCreatePlaneIndicesSolid(a3ubyte *index, const a3ui32 subdivsW, const a3ui32 subdivsH, const a3ui32 indexSize, a3ui32 *start_end, const a3boolean byRow, const a3boolean repeatFirst)
{
	a3ui32 sl, st;
	a3ui32 cols = subdivsW + 1, rows = subdivsH + 1;
	a3ui32 i = *start_end, iStart;
	a3i32 hOffset = byRow ? 1 : rows;
	a3i32 vOffset = byRow ? cols : 1;

	// go by quad and create a 6-indexed triangle strip figure
	if (indexSize < 2)
		for (st = 0; st < subdivsH; ++st, ++i)
		{
			for (sl = 0, iStart = i; sl < subdivsW; ++sl, ++i)
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesSolidByte((a3ubyte *)index, hOffset, vOffset, i);
			if (repeatFirst)
			{
				hOffset = byRow ? -(a3i32)subdivsW : rows;
				vOffset = byRow ? cols : -(a3i32)subdivsH;
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesSolidByte((a3ubyte *)index, hOffset, vOffset, i);
			}
		}
	else if (indexSize < 4)
		for (st = 0; st < subdivsH; ++st, ++i)
		{
			for (sl = 0, iStart = i; sl < subdivsW; ++sl, ++i)
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesSolidShort((a3ui16 *)index, hOffset, vOffset, i);
			if (repeatFirst)
			{
				hOffset = byRow ? -(a3i32)subdivsW : rows;
				vOffset = byRow ? cols : -(a3i32)subdivsH;
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesSolidShort((a3ui16 *)index, hOffset, vOffset, i);
			}
		}
	else
		for (st = 0; st < subdivsH; ++st, ++i)
		{
			for (sl = 0, iStart = i; sl < subdivsW; ++sl, ++i)
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesSolidInt((a3ui32 *)index, hOffset, vOffset, i);
			if (repeatFirst)
			{
				hOffset = byRow ? -(a3i32)subdivsW : rows;
				vOffset = byRow ? cols : -(a3i32)subdivsH;
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesSolidInt((a3ui32 *)index, hOffset, vOffset, i);
			}
		}

	// done
	// apply index offset to the end of the last row
	// subtract 1 because the secondary offset adds 1
	*start_end = i + hOffset + vOffset - 1;
	return index;
}

// indices for a wire grid
inline a3ubyte *a3proceduralInternalCreatePlaneIndicesWire(a3ubyte *index, const a3ui32 subdivsW, const a3ui32 subdivsH, const a3ui32 indexSize, a3ui32 *start_end, const a3boolean byRow, const a3boolean repeatFirst)
{
	a3ui32 sl, st;
	a3ui32 cols = subdivsW + 1, rows = subdivsH + 1;
	a3ui32 i = *start_end, iStart;
	a3i32 hOffset = byRow ? 1 : rows;
	a3i32 vOffset = byRow ? cols : 1;

	// go by quad and create a 8-indexed triangle strip figure
	if (indexSize < 2)
		for (st = 0; st < subdivsH; ++st, ++i)
		{
			for (sl = 0, iStart = i; sl < subdivsW; ++sl, ++i)
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesWireByte((a3ubyte *)index, hOffset, vOffset, i);
			if (repeatFirst)
			{
				hOffset = byRow ? -(a3i32)subdivsW : rows;
				vOffset = byRow ? cols : -(a3i32)subdivsH;
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesWireByte((a3ubyte *)index, hOffset, vOffset, i);
			}
		}
	else if (indexSize < 4)
		for (st = 0; st < subdivsH; ++st, ++i)
		{
			for (sl = 0, iStart = i; sl < subdivsW; ++sl, ++i)
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesWireShort((a3ui16 *)index, hOffset, vOffset, i);
			if (repeatFirst)
			{
				hOffset = byRow ? -(a3i32)subdivsW : rows;
				vOffset = byRow ? cols : -(a3i32)subdivsH;
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesWireShort((a3ui16 *)index, hOffset, vOffset, i);
			}
		}
	else
		for (st = 0; st < subdivsH; ++st, ++i)
		{
			for (sl = 0, iStart = i; sl < subdivsW; ++sl, ++i)
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesWireInt((a3ui32 *)index, hOffset, vOffset, i);
			if (repeatFirst)
			{
				hOffset = byRow ? -(a3i32)subdivsW : rows;
				vOffset = byRow ? cols : -(a3i32)subdivsH;
				index = (a3ubyte *)a3proceduralInternalStorePlaneIndicesWireInt((a3ui32 *)index, hOffset, vOffset, i);
			}
		}

	// done
	*start_end = i + hOffset + vOffset - 1;
	return index;
}


//-----------------------------------------------------------------------------


a3ret a3proceduralCreateDescriptorPlane(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, const a3_ProceduralGeometryAxis axis, a3f32 width, a3f32 height, a3ubyte subdivsW, a3ubyte subdivsH)
{
	a3_ProceduralGeometryDescriptor ret = { 0 };
	if (geom_out && !geom_out->shape)
	{
		// validate params
		if (width <= 0.0f)
			width = 2.0f;
		if (height <= 0.0f)
			height = 2.0f;
		if (subdivsW < 1)
			subdivsW = 1;
		if (subdivsH < 1)
			subdivsH = 1;
		if (width > 0.0f && height > 0.0f && subdivsW && subdivsH)
		{
			// store variables
			ret.fParams[fWidth] = width;
			ret.fParams[fHeight] = height;
			ret.bParams[bSubdivsW] = subdivsW;
			ret.bParams[bSubdivsH] = subdivsH;

			// create descriptor
			ret.shape = a3geomShape_plane;
			*geom_out = ret;
			return a3proceduralSetDescriptorFlagsAxis(geom_out, flags, axis);
		}
	}
	return -1;
}


a3ret a3proceduralInternalGeneratePlane(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	// recover params and flags
	const a3f32 halfWidth = 0.5f * geom->fParams[fWidth];
	const a3f32 halfHeight = 0.5f * geom->fParams[fHeight];
	const a3ui32 subdivsW = geom->bParams[bSubdivsW];
	const a3ui32 subdivsH = geom->bParams[bSubdivsH];
	const a3_ProceduralGeometryAxis axis = a3proceduralGetDescriptorAxis(geom);
	const a3_ProceduralGeometryFlag flags = a3proceduralGetDescriptorFlags(geom);
	const a3boolean isSolid = (flags != a3geomFlag_wireframe);
	const a3boolean isGrid = (subdivsW > 1 || subdivsH > 1);
	const a3_VertexPrimitiveType primType = geomData->primType = isSolid ? isGrid ? a3prim_triangles : a3prim_triangleStrip : isGrid ? a3prim_lines : a3prim_lineLoop;

	// create descriptor
	// vertex count: divisions (x+1)(y+1) -> based on area
	// index count: if using divisions on either dimension, 6xy for solid, 8xy wire
	//	(two triangles per cell = 6 indices per cell)
	// else, fixed count of 0 because it's a triangle strip or line loop
	// get vertex count
	const a3ui32 numVertices = geomData->numVertices = ((subdivsW + 1)*(subdivsH + 1));
	const a3ui32 numIndices = geomData->numIndices = ((subdivsW > 1 || subdivsH > 1) ? subdivsW*subdivsH * ((flags != a3geomFlag_wireframe) ? 6 : 8) : 0);
	const a3ui32 vertexSize = a3proceduralInternalCreateFormats(geomData->vertexFormat, geomData->indexFormat, numVertices, numIndices, flags, 0);
	const a3ui32 indexSize = geomData->indexFormat->indexSize;

	// allocate space for whole buffer required
	const a3ui32 vertexBufferSize = a3vertexFormatGetStorageSpaceRequired(geomData->vertexFormat, numVertices);
	const a3ui32 indexBufferSize = a3indexFormatGetStorageSpaceRequired(geomData->indexFormat, numIndices);
	a3ubyte *const data = (a3ubyte *)malloc(vertexBufferSize + indexBufferSize);

	// temporary pointers to non-interleaved attributes
	a3f32 *const positions = (a3f32 *)(data),
		*const texcoords = positions + geomData->vertexFormat->attribElements[a3attrib_position] * numVertices,
		*const normals = texcoords + geomData->vertexFormat->attribElements[a3attrib_texcoord] * numVertices,
		*const tangents = normals + geomData->vertexFormat->attribElements[a3attrib_normal] * numVertices,
		*const bitangents = tangents + geomData->vertexFormat->attribElements[a3attrib_tangent] * numVertices,
		*const vertexDataEnd = bitangents + geomData->vertexFormat->attribElements[a3attrib_bitangent] * numVertices;
	a3ubyte *const vertexDataEndConfirm = data + vertexBufferSize;
	a3ubyte *const indices = vertexDataEndConfirm,
		*const indexDataEnd = indices + indexSize * numIndices;
	a3ubyte *const indexDataEndConfirm = data + vertexBufferSize + indexBufferSize;
	a3f32 *attribItr = positions;
	a3ubyte *indexItr = indices;
	a3ui32 i = 0;
	a3boolean valid = 0;


	// test tangent basis
	a3f32 t[3] = { 1.0f, 0.0f, 0.0f }, b[3] = { 0.0f, 1.0f, 0.0f }, n[3] = { 0.0f, 0.0f, 1.0f };


	//-------------------------------------------------------------------------
	// solid
	if (isSolid)
	{
		// generate common vertices
		attribItr = a3proceduralInternalCreatePlane(attribItr, +halfWidth, -halfWidth, +halfHeight, -halfHeight, 0.0f, subdivsW, subdivsH, vElems, 1);
		a3proceduralInternalTransformVectors(positions, numVertices, axis);
		assert(valid = attribItr == texcoords);

		// if we're indexed, generate indices for solid grid
		// otherwise, we're done
		if (numIndices)
		{
			indexItr = a3proceduralInternalCreatePlaneIndicesSolid(indexItr, subdivsW, subdivsH, indexSize, &i, 1, 0);
			assert(valid = indexItr == indexDataEnd);
		}

		// create texture coordinates (DO NOT TRANSFORM)
		if (flags & a3geomFlag_texcoords)
		{
			// use grid generator to do it
			attribItr = texcoords;
			attribItr = a3proceduralInternalCreatePlane(attribItr, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, subdivsW, subdivsH, tElems, 1);
			assert(valid = attribItr == normals);

			geomData->attribData[a3attrib_geomTexcoord] = texcoords;
			
			// calculate tangent basis with correct positions and texcoords
			a3proceduralInternalCalculateTangentBasis(t, b, n, 
				positions, positions + vElems, positions + vElems * (subdivsW + 1),
				texcoords, texcoords + tElems, texcoords + tElems * (subdivsW + 1)
			);
		}
		else
		{
			// calculate normal
			a3proceduralInternalCalculateNormal(n, positions, positions + vElems, positions + vElems * (subdivsW + 1));
		}

		// create and transform normals
		if (flags & a3geomFlag_normals)
		{
			// the whole thing is a copy of the Z axis
			attribItr = normals;
			for (i = 0; i < numVertices; ++i, attribItr += vElems)
			{
				attribItr[0] = n[0];
				attribItr[1] = n[1];
				attribItr[2] = n[2];
			}

			assert(valid = attribItr == tangents);

			geomData->attribData[a3attrib_geomNormal] = normals;

			// if not calculating precise tangent basis, transform here
		//	a3proceduralInternalTransformVectors(normals, numVertices, axis);

			// create and transform tangents and bitangents
			if (flags & a3geomFlag_tangentFlag)
			{
				// copy tangent and bitangent
				a3f32 *attribItr2 = bitangents;
				attribItr = tangents;
				for (i = 0; i < numVertices; ++i, attribItr += vElems, attribItr2 += vElems)
				{
					attribItr[0] = t[0];
					attribItr[1] = t[1];
					attribItr[2] = t[2];
					attribItr2[0] = b[0];
					attribItr2[1] = b[1];
					attribItr2[2] = b[2];
				}

				// done
				assert(valid = attribItr == bitangents);
				assert(valid = attribItr2 == vertexDataEnd);

				geomData->attribData[a3attrib_geomTangent] = tangents;

				// if not calculating precise tangent basis, transform here
			//	a3proceduralInternalTransformVectors(tangents, numVertices, axis);
			//	a3proceduralInternalTransformVectors(bitangents, numVertices, axis);
			}
		}
	}


	//-------------------------------------------------------------------------
	// wireframe
	else
	{
		// generate common vertices
		attribItr = a3proceduralInternalCreatePlane(attribItr, +halfWidth, -halfWidth, +halfHeight, -halfHeight, 0.0f, subdivsW, subdivsH, vElems, 1);
		a3proceduralInternalTransformVectors(positions, numVertices, axis);
		assert(valid = attribItr == texcoords);

		// indices
		if (numIndices)
		{
			indexItr = a3proceduralInternalCreatePlaneIndicesWire(indexItr, subdivsW, subdivsH, indexSize, &i, 1, 0);
			assert(valid = indexItr == indexDataEnd);
		}
		else
		{
			// flip last two verts because it's a line loop
			a3f32 *third = positions + 2 * vElems, *fourth = third + vElems;
			const a3f32 tmp[3] = { third[0], third[1], third[2] };
			third[0] = fourth[0];
			third[1] = fourth[1];
			third[2] = fourth[2];
			fourth[0] = tmp[0];
			fourth[1] = tmp[1];
			fourth[2] = tmp[2];
		}
	}


	//-------------------------------------------------------------------------

	// set data pointer
	geomData->attribData[a3attrib_geomPosition] = positions;
	geomData->indexData = numIndices ? indices : 0;
	geomData->data = data;


	//-------------------------------------------------------------------------
	// done
	return 1;
}


#endif	// __ANIMAL3D_PROCEDURALGEOMETRY_C