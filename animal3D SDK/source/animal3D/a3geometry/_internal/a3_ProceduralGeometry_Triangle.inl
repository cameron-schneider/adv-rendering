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
	
	a3_ProceduralGeometry_Triangle.inl
	Internal generation functions for Triangle.

	**DO NOT MODIFY THIS FILE**
*/

#ifdef __ANIMAL3D_PROCEDURALGEOMETRY_C


a3ret a3proceduralCreateDescriptorTriangle(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, const a3_ProceduralGeometryAxis axis, a3f32 base, a3f32 height)
{
	a3_ProceduralGeometryDescriptor ret = { 0 };
	if (geom_out && !geom_out->shape)
	{
		// validate params
		if (base <= 0.0f)
			base = 1.0f;
		if (height <= 0.0f)
			height = 1.0f;
		if (base > 0.0f && height > 0.0f)
		{
			// store variables
			ret.fParams[fBase] = base;
			ret.fParams[fHeight] = height;
			
			// create descriptor
			ret.shape = a3geomShape_triangle;
			*geom_out = ret;
			return a3proceduralSetDescriptorFlagsAxis(geom_out, flags, axis);
		}
	}
	return -1;
}


a3ret a3proceduralInternalGenerateTriangle(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	// recover params
	const a3f32 halfBase = 0.5f * geom->fParams[fBase];
	const a3f32 halfHeight = 0.5f * geom->fParams[fHeight];
	const a3_ProceduralGeometryAxis axis = a3proceduralGetDescriptorAxis(geom);
	const a3_ProceduralGeometryFlag flags = a3proceduralGetDescriptorFlags(geom);
	const a3boolean isSolid = (flags != a3geomFlag_wireframe);
	const a3_VertexPrimitiveType primType = geomData->primType = isSolid ? a3prim_triangles : a3prim_lineLoop;

	// FIXED VERTEX COUNT: 3
	// FIXED INDEX COUNT: 0
	const a3ui32 numVertices = geomData->numVertices = 3;
	const a3ui32 numIndices = geomData->numIndices = 0;
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
	a3f32 *attribItr = positions, *attribItr2 = positions, *attribItr3 = positions, *pPtr = positions, *tcPtr = texcoords;
	a3ubyte *indexItr = indices;
	a3boolean valid = 0;
	a3ui32 i = 0, j = 0, k = 0;


	// test tangent basis
	a3f32 t[3] = { 1.0f, 0.0f, 0.0f }, b[3] = { 0.0f, 1.0f, 0.0f }, n[3] = { 0.0f, 0.0f, 1.0f };


	//-------------------------------------------------------------------------

	// create vertex list
	const a3f32 positionsLocal[] = {
		-halfBase, -halfHeight, 0.0f,
		+halfBase, -halfHeight, 0.0f,
		0.0f, +halfHeight, 0.0f,
	};
	memcpy(positions, positionsLocal, sizeof(positionsLocal));

	a3proceduralInternalTransformVectors(positions, numVertices, axis);


	// (NOT INDEXED)


	//-------------------------------------------------------------------------
	// solid
	if (isSolid)
	{
		// create texture coordinates (DO NOT TRANSFORM)
		if (flags & a3geomFlag_texcoords)
		{
			const a3f32 texcoordsLocal[] = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				0.5f, 1.0f,
			};
			memcpy(texcoords, texcoordsLocal, sizeof(texcoordsLocal));
			geomData->attribData[a3attrib_geomTexcoord] = texcoords;

			// calculate tangent basis
			a3proceduralInternalCalculateTangentBasis(t, b, n, 
				positions, positions + vElems, positions + vElems * 2, 
				texcoords, texcoords + tElems, texcoords + tElems * 2
			);
		}
		else
		{
			// calculate normals
			a3proceduralInternalCalculateNormal(n, positions, positions + vElems, positions + vElems * 2);
		}

		// create normals
		if (flags & a3geomFlag_normals)
		{
			const a3f32 normalsLocal[] = {
				n[0], n[1], n[2],
				n[0], n[1], n[2],
				n[0], n[1], n[2],
			};
			memcpy(normals, normalsLocal, sizeof(normalsLocal));
			geomData->attribData[a3attrib_geomNormal] = normals;

			// if not calculating precise tangent basis, transform here
		//	a3proceduralInternalTransformVectors(normals, numVerts, axis);

			// create tangents and bitangents
			if (flags & a3geomFlag_tangentFlag)
			{
				const a3f32 tangentsLocal[] = {
					t[0], t[1], t[2],
					t[0], t[1], t[2],
					t[0], t[1], t[2],
				};
				const a3f32 bitangentsLocal[] = {
					b[0], b[1], b[2],
					b[0], b[1], b[2],
					b[0], b[1], b[2],
				};
				memcpy(tangents, tangentsLocal, sizeof(tangentsLocal));
				memcpy(bitangents, bitangentsLocal, sizeof(bitangentsLocal));
				geomData->attribData[a3attrib_geomTangent] = tangents;

				// if not calculating precise tangent basis, transform here
			//	a3proceduralInternalTransformVectors(tangents, numVerts, axis);
			//	a3proceduralInternalTransformVectors(bitangents, numVerts, axis);
			}
		}
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