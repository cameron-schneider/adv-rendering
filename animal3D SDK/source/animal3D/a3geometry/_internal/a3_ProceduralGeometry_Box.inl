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
	
	a3_ProceduralGeometry_Box.inl
	Internal generation functions for Box.

	**DO NOT MODIFY THIS FILE**
*/

#ifdef __ANIMAL3D_PROCEDURALGEOMETRY_C


a3ret a3proceduralCreateDescriptorBox(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, a3f32 width, a3f32 height, a3f32 length, a3ubyte subdivsW, a3ubyte subdivsH, a3ubyte subdivsL)
{
	a3_ProceduralGeometryDescriptor ret = { 0 };
	if (geom_out && !geom_out->shape)
	{
		// validate params
		if (width <= 0.0f)
			width = 2.0f;
		if (height <= 0.0f)
			height = 2.0f;
		if (length <= 0.0f)
			length = 2.0f;
		if (subdivsW < 1)
			subdivsW = 1;
		if (subdivsH < 1)
			subdivsH = 1;
		if (subdivsL < 1)
			subdivsL = 1;
		if (width > 0.0f && height > 0.0f && length > 0.0f && subdivsW && subdivsH && subdivsL)
		{
			// store variables
			ret.fParams[fWidth] = width;
			ret.fParams[fHeight] = height;
			ret.fParams[fLength] = length;
			ret.bParams[bSubdivsW] = subdivsW;
			ret.bParams[bSubdivsH] = subdivsH;
			ret.bParams[bSubdivsL] = subdivsL;

			// create descriptor
			ret.shape = a3geomShape_box;
			*geom_out = ret;
			return a3proceduralSetDescriptorFlags(geom_out, flags);
		}
	}
	return -1;
}


a3ret a3proceduralInternalGenerateBox(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	// recover params and flags
	const a3f32 halfWidth = 0.5f * geom->fParams[fWidth];
	const a3f32 halfHeight = 0.5f * geom->fParams[fHeight];
	const a3f32 halfLength = 0.5f * geom->fParams[fLength];
	const a3ubyte subdivsW = geom->bParams[bSubdivsW];
	const a3ubyte subdivsH = geom->bParams[bSubdivsH];
	const a3ubyte subdivsL = geom->bParams[bSubdivsL];
	const a3_ProceduralGeometryFlag flags = a3proceduralGetDescriptorFlags(geom);
	const a3boolean isSolid = (flags != a3geomFlag_wireframe);
	const a3_VertexPrimitiveType primType = geomData->primType = isSolid ? a3prim_triangles : a3prim_lines;

	// vertex count: 
	// divisions 2((x+1)(y+1) + (x+1)(z+1) + (y+1)(z+1)) -> surface area
	// index count: 
	// solid: 12xy + 12xz + 12yz; wire: 16...
	const a3ui32 indexMultiplier = (isSolid ? 6 : 8);
	const a3ui32 vertsZ = ((a3i32)subdivsW + 1)*((a3i32)subdivsH + 1);
	const a3ui32 vertsY = ((a3i32)subdivsW + 1)*((a3i32)subdivsL + 1);
	const a3ui32 vertsX = ((a3i32)subdivsH + 1)*((a3i32)subdivsL + 1);
	const a3ui32 indsZ = (a3i32)subdivsW*(a3i32)subdivsH*indexMultiplier;
	const a3ui32 indsY = (a3i32)subdivsW*(a3i32)subdivsL*indexMultiplier;
	const a3ui32 indsX = (a3i32)subdivsH*(a3i32)subdivsL*indexMultiplier;
	const a3ui32 numVertices = geomData->numVertices = 2 * (vertsZ + vertsY + vertsX);
	const a3ui32 numIndices = geomData->numIndices = (indsZ + indsY + indsX) * 2;
	const a3ui32 vertexSize = a3proceduralInternalCreateFormats(geomData->vertexFormat, geomData->indexFormat, numVertices, numIndices, flags, 0);
	const a3ui32 indexSize = geomData->indexFormat->indexSize;
	const a3ui32 numFaces = 6;


	// tangent basis per face
	a3f32 t[18] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 
		0.0f, 0.0f, +1.0f,
	}, b[18] = {
		0.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, +1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	}, n[18] = {
		0.0f, 0.0f, +1.0f,
		0.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, -1.0f, 0.0f,
		+1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
	}, *tPtr = t, *bPtr = b, *nPtr = n;
	const a3ui32 faceRowOffset[6] = { subdivsW + 1, subdivsW + 1, subdivsW + 1, subdivsW + 1, subdivsL + 1, subdivsL + 1 };

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
	a3f32 *attribItr = positions, *attribItr2 = positions, *pPtr = positions, *tcPtr = texcoords;
	a3ubyte *indexItr = indices;
	a3ui32 i = 0, j = 0, k = 0;
	a3boolean valid = 0;

	// extra info
	const a3ui32 vertsPerFace[6] = { vertsZ, vertsY, vertsZ, vertsY, vertsX, vertsX };


	//-------------------------------------------------------------------------
	// positions: same for solid or wire

	// generate common vertices: Z axis
	attribItr = a3proceduralInternalCreatePlane(attribItr, +halfWidth, -halfWidth, +halfHeight, -halfHeight, halfLength, subdivsW, subdivsH, vElems, 1);

	// Y axis: generate and transform to top
	attribItr2 = a3proceduralInternalCreatePlane(attribItr, +halfWidth, -halfWidth, +halfLength, -halfLength, halfHeight, subdivsW, subdivsL, vElems, 1);
	a3proceduralInternalTransformVectors(attribItr, vertsY, a3geomAxis_y);
	attribItr = attribItr2;

	// -Z axis: do same as first, rotate about X twice to turn them upside down
	attribItr2 = a3proceduralInternalCreatePlane(attribItr, +halfWidth, -halfWidth, +halfHeight, -halfHeight, halfLength, subdivsW, subdivsH, vElems, 1);
	a3proceduralInternalTransformVectors(attribItr, vertsZ, a3geomAxis_y);
	a3proceduralInternalTransformVectors(attribItr, vertsZ, a3geomAxis_y);
	attribItr = attribItr2;

	// -Y axis: generate and transform to bottom
	attribItr2 = a3proceduralInternalCreatePlane(attribItr, +halfWidth, -halfWidth, +halfLength, -halfLength, halfHeight, subdivsW, subdivsL, vElems, 1);
	a3proceduralInternalTransformVectors(attribItr, vertsY, a3geomAxis_y_negative);
	attribItr = attribItr2;

	// X axis: generate and transform to right
	attribItr2 = a3proceduralInternalCreatePlane(attribItr, +halfLength, -halfLength, +halfHeight, -halfHeight, halfWidth, subdivsL, subdivsH, vElems, 1);
	a3proceduralInternalTransformVectors(attribItr, vertsX, a3geomAxis_x);
	attribItr = attribItr2;

	// X axis: generate and transform to left
	attribItr2 = a3proceduralInternalCreatePlane(attribItr, +halfLength, -halfLength, +halfHeight, -halfHeight, halfWidth, subdivsL, subdivsH, vElems, 1);
	a3proceduralInternalTransformVectors(attribItr, vertsX, a3geomAxis_x_negative);
	attribItr = attribItr2;

	// done
	assert(valid = attribItr == texcoords);


	//-------------------------------------------------------------------------
	// solid
	if (isSolid)
	{
		// indices: same process as above, really just generating 6 planes
		// order is: +Z, +Y, -Z, -Y, +X, -X
		indexItr = a3proceduralInternalCreatePlaneIndicesSolid(indexItr, subdivsW, subdivsH, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesSolid(indexItr, subdivsW, subdivsL, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesSolid(indexItr, subdivsW, subdivsH, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesSolid(indexItr, subdivsW, subdivsL, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesSolid(indexItr, subdivsL, subdivsH, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesSolid(indexItr, subdivsL, subdivsH, indexSize, &i, 1, 0);
		assert(valid = indexItr == indexDataEnd);

		// create texture coordinates (DO NOT TRANSFORM)
		if (flags & a3geomFlag_texcoords)
		{
			// upside down T shape
			attribItr = texcoords;
			attribItr = a3proceduralInternalCreatePlane(attribItr, 0.625f, 0.375f, 0.250f, 0.000f, 0.0f, subdivsW, subdivsH, tElems, 1);
			attribItr = a3proceduralInternalCreatePlane(attribItr, 0.625f, 0.375f, 0.500f, 0.250f, 0.0f, subdivsW, subdivsL, tElems, 1);
			attribItr = a3proceduralInternalCreatePlane(attribItr, 0.625f, 0.375f, 0.750f, 0.500f, 0.0f, subdivsW, subdivsH, tElems, 1);
			attribItr = a3proceduralInternalCreatePlane(attribItr, 0.625f, 0.375f, 1.000f, 0.750f, 0.0f, subdivsW, subdivsL, tElems, 1);
			attribItr = a3proceduralInternalCreatePlane(attribItr, 0.875f, 0.625f, 0.250f, 0.000f, 0.0f, subdivsL, subdivsH, tElems, 1);
			attribItr = a3proceduralInternalCreatePlane(attribItr, 0.375f, 0.125f, 0.250f, 0.000f, 0.0f, subdivsL, subdivsH, tElems, 1);
			assert(valid = attribItr == normals);

			geomData->attribData[a3attrib_geomTexcoord] = texcoords;

			// calculate tangent basis per face
			for (i = 0, j = vertsPerFace[i]; i < numFaces; ++i, pPtr += j * vElems, tcPtr += j * tElems, tPtr += vElems, bPtr += vElems, nPtr += vElems, j = vertsPerFace[i])
				a3proceduralInternalCalculateTangentBasis(tPtr, bPtr, nPtr,
					pPtr, pPtr + vElems, pPtr + vElems * faceRowOffset[i],
					tcPtr, tcPtr + tElems, tcPtr + tElems * faceRowOffset[i]
				);
		}
		else
		{
			// if normals were not calculated via tangent basis, do it here
			for (i = 0, j = vertsPerFace[i]; i < numFaces; ++i, pPtr += j * vElems, nPtr += vElems, j = vertsPerFace[i])
				a3proceduralInternalCalculateNormal(nPtr, pPtr, pPtr + vElems, pPtr + vElems * faceRowOffset[i]);
		}

		// create normals
		if (flags & a3geomFlag_normals)
		{
			// the whole thing is a copy of the Z axis
			attribItr = normals;
			for (i = 0, nPtr = n; i < numFaces; ++i, nPtr += vElems)
				// copy face normal to the number of vertices per face
				for (j = 0, k = vertsPerFace[i]; j < k; ++j, attribItr += vElems)
				{
					attribItr[0] = nPtr[0];
					attribItr[1] = nPtr[1];
					attribItr[2] = nPtr[2];
				}
			assert(valid = attribItr == tangents);

			geomData->attribData[a3attrib_geomNormal] = normals;

			// create tangents and bitangents
			if (flags & a3geomFlag_tangentFlag)
			{
				// copy tangent and bitangent
				attribItr = tangents;
				attribItr2 = bitangents;
				for (i = 0, tPtr = t, bPtr = b; i < numFaces; ++i, tPtr += vElems, bPtr += vElems)
					for (j = 0, k = vertsPerFace[i]; j < k; ++j, attribItr += vElems, attribItr2 += vElems)
					{
						attribItr[0] = tPtr[0];
						attribItr[1] = tPtr[1];
						attribItr[2] = tPtr[2];
						attribItr2[0] = bPtr[0];
						attribItr2[1] = bPtr[1];
						attribItr2[2] = bPtr[2];
					}

				assert(valid = attribItr == bitangents);
				assert(valid = attribItr2 == vertexDataEnd);

				geomData->attribData[a3attrib_geomTangent] = tangents;
			}
		}
	}


	//-------------------------------------------------------------------------
	// wireframe
	else
	{
		// indices
		indexItr = a3proceduralInternalCreatePlaneIndicesWire(indexItr, subdivsW, subdivsH, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesWire(indexItr, subdivsW, subdivsL, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesWire(indexItr, subdivsW, subdivsH, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesWire(indexItr, subdivsW, subdivsL, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesWire(indexItr, subdivsL, subdivsH, indexSize, &i, 1, 0);
		indexItr = a3proceduralInternalCreatePlaneIndicesWire(indexItr, subdivsL, subdivsH, indexSize, &i, 1, 0);
		assert(valid = indexItr == indexDataEnd);
	}


	//-------------------------------------------------------------------------

	// copy remaining stuff
	geomData->attribData[a3attrib_geomPosition] = positions;
	geomData->indexData = numIndices ? indices : 0;
	geomData->data = data;


	//-------------------------------------------------------------------------
	// done
	return 1;
}


#endif	// __ANIMAL3D_PROCEDURALGEOMETRY_C