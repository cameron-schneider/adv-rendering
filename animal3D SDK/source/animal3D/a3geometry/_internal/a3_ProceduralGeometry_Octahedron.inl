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
	
	a3_ProceduralGeometry_Octahedron.inl
	Internal generation functions for Octahedron.

	**DO NOT MODIFY THIS FILE**
*/

#ifdef __ANIMAL3D_PROCEDURALGEOMETRY_C


a3ret a3proceduralCreateDescriptorOctahedron(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, const a3_ProceduralGeometryAxis axis, a3f32 width, a3f32 length)
{
	a3_ProceduralGeometryDescriptor ret = { 0 };
	if (geom_out && !geom_out->shape)
	{
		// validate params
		if (width <= 0.0f)
			width = 1.0f;
		if (length <= 0.0f)
			length = 1.0f;
		if (width > 0.0f && length > 0.0f)
		{
			// store variables
			ret.fParams[fWidth] = width;
			ret.fParams[fLength] = length;

			// create descriptor
			ret.shape = a3geomShape_octahedron;
			*geom_out = ret;
			return a3proceduralSetDescriptorFlagsAxis(geom_out, flags, axis);
		}
	}
	return -1;
}


a3ret a3proceduralInternalGenerateOctahedron(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	// recover params
	const a3f32 halfWidth = 0.5f * geom->fParams[fWidth];
	const a3f32 halfLength = 0.5f * geom->fParams[fLength];
	const a3_ProceduralGeometryAxis axis = a3proceduralGetDescriptorAxis(geom);
	const a3_ProceduralGeometryFlag flags = a3proceduralGetDescriptorFlags(geom);
	const a3boolean isSolid = (flags != a3geomFlag_wireframe);
	const a3_VertexPrimitiveType primType = geomData->primType = isSolid ? a3prim_triangles : a3prim_lineLoop;

	// get vertex count
	// FIXED VERTEX COUNT: if not wireframe or vanilla, 24; else, 6
	// FIXED INDEX COUNT: if wireframe, 12 (line loop); else if vanilla, 24 (triangles); else, 0 (unique triangles)
	const a3ui32 numVertices = geomData->numVertices = (flags != a3geomFlag_wireframe && flags != a3geomFlag_vanilla ? 24 : 6);
	const a3ui32 numIndices = geomData->numIndices = (flags != a3geomFlag_wireframe ? flags != a3geomFlag_vanilla ? 0 : 24 : 12);
	const a3ui32 vertexSize = a3proceduralInternalCreateFormats(geomData->vertexFormat, geomData->indexFormat, numVertices, numIndices, flags, 0);
	const a3ui32 indexSize = geomData->indexFormat->indexSize;
	const a3ui32 numFaces = 8;

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
	a3f32 *attribItr = positions, *attribItr2 = positions, *attribItr3 = positions, *pPtr = positions, *tcPtr = texcoords;
	a3ubyte *indexItr = indices;
	a3boolean valid = 0;

	a3ui32 i, j;


	// extra info
	const a3ui32 vertsPerFace[8] = { 3, 3, 3, 3, 3, 3, 3, 3 };

	// tangent basis for faces
	a3f32 t[24], b[24], n[24], *tPtr = t, *bPtr = b, *nPtr = n;
	a3f32 tLen = 1.0f, bLen = 1.0f, nLen = 1.0f;



	//-------------------------------------------------------------------------
	// solid
	if (isSolid)
	{
		// has other attributes
		if (flags != a3geomFlag_vanilla)
		{
			const a3f32 positionsLocal[] = {
				// front sides, CCW, starting with center
				-halfWidth, -halfWidth, 0.0f,
				+halfWidth, -halfWidth, 0.0f,
				0.0f, 0.0f, +halfLength,
				+halfWidth, -halfWidth, 0.0f,
				+halfWidth, +halfWidth, 0.0f,
				0.0f, 0.0f, +halfLength,
				+halfWidth, +halfWidth, 0.0f,
				-halfWidth, +halfWidth, 0.0f,
				0.0f, 0.0f, +halfLength,
				-halfWidth, +halfWidth, 0.0f,
				-halfWidth, -halfWidth, 0.0f,
				0.0f, 0.0f, +halfLength,
				// back sides, exactly the same but in reverse order
				//	and the point is negative length
				-halfWidth, -halfWidth, 0.0f,
				-halfWidth, +halfWidth, 0.0f,
				0.0f, 0.0f, -halfLength,
				-halfWidth, +halfWidth, 0.0f,
				+halfWidth, +halfWidth, 0.0f,
				0.0f, 0.0f, -halfLength,
				+halfWidth, +halfWidth, 0.0f,
				+halfWidth, -halfWidth, 0.0f,
				0.0f, 0.0f, -halfLength,
				+halfWidth, -halfWidth, 0.0f,
				-halfWidth, -halfWidth, 0.0f,
				0.0f, 0.0f, -halfLength,
			}, *pPtr = positions;

			memcpy(positions, positionsLocal, sizeof(positionsLocal));

			a3proceduralInternalTransformVectors(positions, numVertices, axis);

			// (NOT INDEXED)

			// texture coordinates
			if (flags & a3geomFlag_texcoords)
			{
				const a3f32 texcoordsLocal[] = {
					// arrangement is diamond shape
					// front half outside
					0.25f, 0.75f,
					0.75f, 0.75f,
					0.50f, 1.00f,
					0.75f, 0.75f,
					0.75f, 0.25f,
					1.00f, 0.50f,
					0.75f, 0.25f,
					0.25f, 0.25f,
					0.50f, 0.00f,
					0.25f, 0.25f,
					0.25f, 0.75f,
					0.00f, 0.50f,
					// bottom half inside
					0.25f, 0.75f,
					0.25f, 0.25f,
					0.50f, 0.50f,
					0.25f, 0.25f,
					0.75f, 0.25f,
					0.50f, 0.50f,
					0.75f, 0.25f,
					0.75f, 0.75f,
					0.50f, 0.50f,
					0.75f, 0.75f,
					0.25f, 0.75f,
					0.50f, 0.50f,
				}, *tcPtr = texcoords;

				memcpy(texcoords, texcoordsLocal, sizeof(texcoordsLocal));

				geomData->attribData[a3attrib_geomTexcoord] = texcoords;

				// calculate tangent basis per face
				for (i = 0, j = vertsPerFace[i]; i < numFaces; ++i, pPtr += j * vElems, tcPtr += j * tElems, tPtr += vElems, bPtr += vElems, nPtr += vElems, j = vertsPerFace[i])
					a3proceduralInternalCalculateTangentBasis(tPtr, bPtr, nPtr,
						pPtr, pPtr + vElems, pPtr + 2 * vElems,
						tcPtr, tcPtr + tElems, tcPtr + 2 * tElems
					);
			}
			else
			{
				// if normals were not calculated via tangent basis, do it here
				for (i = 0, j = vertsPerFace[i]; i < numFaces; ++i, pPtr += j * vElems, nPtr += vElems, j = vertsPerFace[i])
					a3proceduralInternalCalculateNormal(nPtr, pPtr, pPtr + vElems, pPtr + 2 * vElems);
			}

			// normals
			if (flags & a3geomFlag_normals)
			{
				a3f32 *normalsPtr = normals;
				for (i = 0, nPtr = n; i < numFaces; ++i, nPtr += vElems)
				{
					// copy face normal to 3 vertices
				//	nLen = 1.0f / a3real3Length(nPtr);
					for (j = 0; j < 3; ++j, normalsPtr += vElems)
					{
						normalsPtr[0] = nPtr[0] * nLen;
						normalsPtr[1] = nPtr[1] * nLen;
						normalsPtr[2] = nPtr[2] * nLen;
					}
				}

				geomData->attribData[a3attrib_geomNormal] = normals;

				// tangents and bitangents, same algorithm as above
				if (flags & a3geomFlag_tangentFlag)
				{
					a3f32 *tangentsPtr = tangents, *bitangentsPtr = bitangents;
					for (i = 0, tPtr = t, bPtr = b; i < numFaces; ++i, tPtr += vElems, bPtr += vElems)
					{
					//	tLen = 1.0f / a3real3Length(tPtr);
					//	bLen = 1.0f / a3real3Length(bPtr);
						for (j = 0; j < 3; ++j, tangentsPtr += vElems, bitangentsPtr += vElems)
						{
							tangentsPtr[0] = tPtr[0] * tLen;
							tangentsPtr[1] = tPtr[1] * tLen;
							tangentsPtr[2] = tPtr[2] * tLen;
							bitangentsPtr[0] = bPtr[0] * bLen;
							bitangentsPtr[1] = bPtr[1] * bLen;
							bitangentsPtr[2] = bPtr[2] * bLen;
						}
					}

					geomData->attribData[a3attrib_geomTangent] = tangents;
				}
			}
		}
		else
		{
			// just need the raw positions because there's nothing else 
			//	to complicate indexing
			const a3f32 positionsLocal[] = {
				// start with bottom left, end with point
				-halfWidth, -halfWidth, 0.0f,
				+halfWidth, -halfWidth, 0.0f,
				+halfWidth, +halfWidth, 0.0f,
				-halfWidth, +halfWidth, 0.0f,
				0.0f, 0.0f, +halfLength,
				0.0f, 0.0f, -halfLength,
			};
			// indices: triangles
			const a3ubyte rawIndices[] = {
				0,	1,	4,
				1,	2,	4,
				2,	3,	4,
				3,	0,	4,
				0,	3,	5,
				3,	2,	5,
				2,	1,	5,
				1,	0,	5,
			};

			memcpy(positions, positionsLocal, sizeof(positionsLocal));
			memcpy(indices, rawIndices, sizeof(rawIndices));

			// set index pointer and transform
			a3proceduralInternalTransformVectors(positions, numVertices, axis);
		}
	}


	//-------------------------------------------------------------------------
	// wireframe
	else
	{
		const a3f32 positionsLocal[] = {
			// just need the raw positions
			-halfWidth, -halfWidth, 0.0f,
			+halfWidth, -halfWidth, 0.0f,
			+halfWidth, +halfWidth, 0.0f,
			-halfWidth, +halfWidth, 0.0f,
			0.0f, 0.0f, +halfLength,
			0.0f, 0.0f, -halfLength,
		};

		// indices: line loop
		const a3ubyte rawIndices[] = {
			0, 1, 2, 3, 0, 4, 2, 5, 1, 4, 3, 5,
		};

		memcpy(positions, positionsLocal, sizeof(positionsLocal));
		memcpy(indices, rawIndices, sizeof(rawIndices));

		// set index pointer and transform
		a3proceduralInternalTransformVectors(positions, numVertices, axis);
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