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
	
	a3_ProceduralGeometry_Axes.inl
	Internal generation functions for Axes.

	**DO NOT MODIFY THIS FILE**
*/

#ifdef __ANIMAL3D_PROCEDURALGEOMETRY_C


a3ret a3proceduralCreateDescriptorAxes(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, a3f32 thickness, const a3byte color)
{
	a3_ProceduralGeometryDescriptor ret = { 0 };
	if (geom_out && !geom_out->shape)
	{
		// validate params
		if (thickness <= 0.0f || thickness > 1.0f)
			thickness = 0.02f;
		if (thickness > 0.0f && thickness <= 1.0f)
		{
			// store variables
			ret.bParams[0] = color;
			ret.fParams[0] = thickness;

			// create descriptor
			ret.shape = a3geomShape_axes;
			*geom_out = ret;
			return a3proceduralSetDescriptorFlags(geom_out, flags);
		}
	}
	return -1;
}


a3ret a3proceduralInternalGenerateAxes(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	// recover params
	const a3i32 color = geom->bParams[0];
	const a3f32 halfThickness = 0.5f * geom->fParams[0];
	const a3f32 base = halfThickness;
	const a3f32 end = 1.0f - halfThickness;
	const a3ui32 rawUniquePositionsPerAxis = 2 * 5;
	const a3_ProceduralGeometryFlag flags = a3proceduralGetDescriptorFlags(geom);
	const a3boolean isSolid = (flags != a3geomFlag_wireframe);
	const a3boolean vanilla = (flags == a3geomFlag_vanilla);
	const a3boolean repeats = isSolid && !vanilla;
	const a3_VertexPrimitiveType primType = geomData->primType = isSolid ? a3prim_triangles : a3prim_lineStrip;

	// get vertex count
	// FIXED VERTEX COUNT: 
	//	solid and not vanilla: 3*(2*6) -> 3 axes x symmetrical (2) x 6 unique vertices
	//	wire or vanilla: 3*(2*5) -> exclude repeat vertices
	// FIXED INDEX COUNT: 
	//	solid: triangles -> 3*3*(2*8) -> 3 axes x 3 verts per face x symmetrical (2) x 8 unique faces per axis
	//	wire: line strip -> 3*13 -> 3 axes x 13 indices per axis
	const a3ui32 numVertices = geomData->numVertices = 3 * 2 * (5 + (isSolid && !vanilla));
	const a3ui32 numIndices = geomData->numIndices = 3 * (isSolid ? (3 * 2 * 8) : 13);
	const a3ui32 vertexSize = a3proceduralInternalCreateFormats(geomData->vertexFormat, geomData->indexFormat, numVertices, numIndices, flags, color);
	const a3ui32 indexSize = geomData->indexFormat->indexSize;
	const a3ui32 indicesPerAxis = (numIndices / 3);


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
		*const colors = bitangents + geomData->vertexFormat->attribElements[a3attrib_bitangent] * numVertices,
		*const vertexDataEnd = colors + geomData->vertexFormat->attribElements[a3attrib_color] * numVertices;
	a3ubyte *const vertexDataEndConfirm = data + vertexBufferSize;
	a3ubyte *const indices = vertexDataEndConfirm,
		*const indexDataEnd = indices + indexSize * numIndices;
	a3ubyte *const indexDataEndConfirm = data + vertexBufferSize + indexBufferSize;
	a3f32 *attribItr = positions, *attribItr2 = positions, *attribItr3 = positions, *pPtr = positions, *tcPtr = texcoords;
	a3ubyte *indexItr = indices, *iPtr;
	a3boolean valid = 0;

	// constant set of indices
	a3ui32 i;


	// always same position set
	const a3f32 positionsLocal[] = {
		// Z axis vertices repeated because the first two sets will be transformed
		-halfThickness, -halfThickness, base,	// close to center
		+halfThickness, -halfThickness, base,
		+halfThickness, +halfThickness, base,
		-halfThickness, +halfThickness, base,
		-halfThickness, -halfThickness, end,	// close to end
		+halfThickness, -halfThickness, end,
		+halfThickness, +halfThickness, end,
		-halfThickness, +halfThickness, end,
		+halfThickness, -halfThickness, -halfThickness,	// center
		0.0f, 0.0f, 1.0f,								// extreme

		-halfThickness, -halfThickness, base,
		+halfThickness, -halfThickness, base,
		+halfThickness, +halfThickness, base,
		-halfThickness, +halfThickness, base,
		-halfThickness, -halfThickness, end,
		+halfThickness, -halfThickness, end,
		+halfThickness, +halfThickness, end,
		-halfThickness, +halfThickness, end,
		-halfThickness, +halfThickness, -halfThickness,
		0.0f, 0.0f, 1.0f,

		-halfThickness, -halfThickness, base,
		+halfThickness, -halfThickness, base,
		+halfThickness, +halfThickness, base,
		-halfThickness, +halfThickness, base,
		-halfThickness, -halfThickness, end,
		+halfThickness, -halfThickness, end,
		+halfThickness, +halfThickness, end,
		-halfThickness, +halfThickness, end,
		-halfThickness, -halfThickness, -halfThickness,
		0.0f, 0.0f, 1.0f,
	};
	memcpy(positions, positionsLocal, sizeof(positionsLocal));

	// transform
	a3proceduralInternalTransformVectors(positions + 00, rawUniquePositionsPerAxis, a3geomAxis_x);
	a3proceduralInternalTransformVectors(positions + 30, rawUniquePositionsPerAxis, a3geomAxis_y);


	//-------------------------------------------------------------------------
	// colors, if requested
	if (color)
	{
		const a3f32 colorsLocal[] = {
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,
			1.0f, 0.0f, 0.0f, 0.5f,

			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,
			0.0f, 1.0f, 0.0f, 0.5f,

			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
			0.0f, 0.0f, 1.0f, 0.5f,
		};
		memcpy(colors, colorsLocal, sizeof(colorsLocal));

		if (repeats)
		{
			const a3f32 repeatColors[] = {
				1.0f, 0.0f, 0.0f, 0.5f,
				1.0f, 0.0f, 0.0f, 0.5f,
				0.0f, 1.0f, 0.0f, 0.5f,
				0.0f, 1.0f, 0.0f, 0.5f,
				0.0f, 0.0f, 1.0f, 0.5f,
				0.0f, 0.0f, 1.0f, 0.5f,
			};
			memcpy(colors + 120, repeatColors, sizeof(repeatColors));
		}

		geomData->attribData[a3attrib_geomColor] = colors;
	}


	//-------------------------------------------------------------------------
	// solid
	if (isSolid)
	{
		// indices: triangles
		const a3ubyte rawIndices[] = {
			// copy a single axis' indices, the rest can be offset
			0, 1, 4, 5, 4, 1,
			1, 2, 5, 6, 5, 2,
			2, 3, 6, 7, 6, 3,
			3, (vanilla ? 0 : 30), 7, (vanilla ? 4 : 31), 7, (vanilla ? 0 : 30),
			4, 5, 9,
			5, 6, 9,
			6, 7, 9,
			7, (vanilla ? 4 : 31), 9,
			(vanilla ? 0 : 30), 3, 8,
			1, 0, 8,
			2, 1, 8,
			3, 2, 8,

			0, 1, 4, 5, 4, 1,
			1, 2, 5, 6, 5, 2,
			2, 3, 6, 7, 6, 3,
			3, (vanilla ? 0 : 30), 7, (vanilla ? 4 : 31), 7, (vanilla ? 0 : 30),
			4, 5, 9,
			5, 6, 9,
			6, 7, 9,
			7, (vanilla ? 4 : 31), 9,
			(vanilla ? 0 : 30), 3, 8,
			1, 0, 8,
			2, 1, 8,
			3, 2, 8,

			0, 1, 4, 5, 4, 1,
			1, 2, 5, 6, 5, 2,
			2, 3, 6, 7, 6, 3,
			3, (vanilla ? 0 : 30), 7, (vanilla ? 4 : 31), 7, (vanilla ? 0 : 30),
			4, 5, 9,
			5, 6, 9,
			6, 7, 9,
			7, (vanilla ? 4 : 31), 9,
			(vanilla ? 0 : 30), 3, 8,
			1, 0, 8,
			2, 1, 8,
			3, 2, 8,
		};
		memcpy(indices, rawIndices, sizeof(rawIndices));

		// offset indices for Y and Z axes
		// also offset repeats
		for (i = 0, iPtr = indices + indicesPerAxis; i < indicesPerAxis; ++i, ++iPtr)
			*iPtr += *iPtr < 30 ? rawUniquePositionsPerAxis : 2;
		for (i = 0; i < indicesPerAxis; ++i, ++iPtr)
			*iPtr += *iPtr < 30 ? rawUniquePositionsPerAxis + rawUniquePositionsPerAxis : 4;

		if (repeats)
		{
			const a3f32 repeatVertices[] = {
				-halfThickness, -halfThickness, base,
				-halfThickness, -halfThickness, end,
				-halfThickness, -halfThickness, base,
				-halfThickness, -halfThickness, end,
				-halfThickness, -halfThickness, base,
				-halfThickness, -halfThickness, end,
			};
			memcpy(positions + 90, repeatVertices, sizeof(repeatVertices));

			// transform repeats
			a3proceduralInternalTransformVectors(positions + 90, 2, a3geomAxis_x);
			a3proceduralInternalTransformVectors(positions + 96, 2, a3geomAxis_y);
		}


		// texture coordinates
		if (flags & a3geomFlag_texcoords)
		{
			const a3f32 texcoordsLocal[] = {
				// arrangement is 3x diamond shapes
				// repeats at end
				0.10f, 0.1f,
				0.15f, 0.1f,
				0.20f, 0.1f,
				0.25f, 0.1f,
				0.10f, 0.9f,
				0.15f, 0.9f,
				0.20f, 0.9f,
				0.25f, 0.9f,
				0.20f, 0.0f,
				0.20f, 1.0f,

				0.40f, 0.1f,
				0.45f, 0.1f,
				0.50f, 0.1f,
				0.55f, 0.1f,
				0.40f, 0.9f,
				0.45f, 0.9f,
				0.50f, 0.9f,
				0.55f, 0.9f,
				0.50f, 0.0f,
				0.50f, 1.0f,

				0.70f, 0.1f,
				0.75f, 0.1f,
				0.80f, 0.1f,
				0.85f, 0.1f,
				0.70f, 0.9f,
				0.75f, 0.9f,
				0.80f, 0.9f,
				0.85f, 0.9f,
				0.80f, 0.0f,
				0.80f, 1.0f,

				0.30f, 0.1f,
				0.30f, 0.9f,
				0.60f, 0.1f,
				0.60f, 0.9f,
				0.90f, 0.1f,
				0.90f, 0.9f,
			};
			memcpy(texcoords, texcoordsLocal, sizeof(texcoordsLocal));

			geomData->attribData[a3attrib_geomTexcoord] = texcoords;
		}

		// normals
		if (flags & a3geomFlag_normals)
		{
			// normals by vertex
			const a3f32 normalsLocal[] = {
				-0.1f, -0.1f, 0.0f,
				+0.1f, -0.1f, 0.0f,
				+0.1f, +0.1f, 0.0f,
				-0.1f, +0.1f, 0.0f,
				-0.1f, -0.1f, 0.0f,
				+0.1f, -0.1f, 0.0f,
				+0.1f, +0.1f, 0.0f,
				-0.1f, +0.1f, 0.0f,
				0.0f, 0.0f, -0.1f,
				0.0f, 0.0f, +0.1f,

				-0.1f, -0.1f, 0.0f,
				+0.1f, -0.1f, 0.0f,
				+0.1f, +0.1f, 0.0f,
				-0.1f, +0.1f, 0.0f,
				-0.1f, -0.1f, 0.0f,
				+0.1f, -0.1f, 0.0f,
				+0.1f, +0.1f, 0.0f,
				-0.1f, +0.1f, 0.0f,
				0.0f, 0.0f, -0.1f,
				0.0f, 0.0f, +0.1f,

				-0.1f, -0.1f, 0.0f,
				+0.1f, -0.1f, 0.0f,
				+0.1f, +0.1f, 0.0f,
				-0.1f, +0.1f, 0.0f,
				-0.1f, -0.1f, 0.0f,
				+0.1f, -0.1f, 0.0f,
				+0.1f, +0.1f, 0.0f,
				-0.1f, +0.1f, 0.0f,
				0.0f, 0.0f, -0.1f,
				0.0f, 0.0f, +0.1f,

				-0.1f, -0.1f, 0.0f,
				-0.1f, -0.1f, 0.0f,
				-0.1f, -0.1f, 0.0f,
				-0.1f, -0.1f, 0.0f,
				-0.1f, -0.1f, 0.0f,
				-0.1f, -0.1f, 0.0f,
			};
			memcpy(normals, normalsLocal, sizeof(normalsLocal));

			geomData->attribData[a3attrib_geomNormal] = normals;

			// transform X and Y
			a3proceduralInternalTransformVectors(normals + 00, rawUniquePositionsPerAxis, a3geomAxis_x);
			a3proceduralInternalTransformVectors(normals + 30, rawUniquePositionsPerAxis, a3geomAxis_y);
			a3proceduralInternalTransformVectors(normals + 90, 2, a3geomAxis_x);
			a3proceduralInternalTransformVectors(normals + 96, 2, a3geomAxis_y);

			// tangents and bitangents, same algorithm as above
			if (flags & a3geomFlag_tangentFlag)
			{
				// tangents: by vertex
				const a3f32 tangentsLocal[] = {
					+0.1f, -0.1f, 0.0f,
					+0.1f, +0.1f, 0.0f,
					-0.1f, +0.1f, 0.0f,
					-0.1f, -0.1f, 0.0f,
					+0.1f, -0.1f, 0.0f,
					+0.1f, +0.1f, 0.0f,
					-0.1f, +0.1f, 0.0f,
					-0.1f, -0.1f, 0.0f,
					+0.1f, 0.0f, 0.0f,
					+0.1f, 0.0f, 0.0f,

					+0.1f, -0.1f, 0.0f,
					+0.1f, +0.1f, 0.0f,
					-0.1f, +0.1f, 0.0f,
					-0.1f, -0.1f, 0.0f,
					+0.1f, -0.1f, 0.0f,
					+0.1f, +0.1f, 0.0f,
					-0.1f, +0.1f, 0.0f,
					-0.1f, -0.1f, 0.0f,
					+0.1f, 0.0f, 0.0f,
					+0.1f, 0.0f, 0.0f,

					+0.1f, -0.1f, 0.0f,
					+0.1f, +0.1f, 0.0f,
					-0.1f, +0.1f, 0.0f,
					-0.1f, -0.1f, 0.0f,
					+0.1f, -0.1f, 0.0f,
					+0.1f, +0.1f, 0.0f,
					-0.1f, +0.1f, 0.0f,
					-0.1f, -0.1f, 0.0f,
					+0.1f, 0.0f, 0.0f,
					+0.1f, 0.0f, 0.0f,

					+0.1f, -0.1f, 0.0f,
					+0.1f, -0.1f, 0.0f,
					+0.1f, -0.1f, 0.0f,
					+0.1f, -0.1f, 0.0f,
					+0.1f, -0.1f, 0.0f,
					+0.1f, -0.1f, 0.0f,
				};
				a3f32 *nPtr = normals, *tPtr = tangents, *bPtr = bitangents;
				memcpy(tangents, tangentsLocal, sizeof(tangentsLocal));

				// transform
				a3proceduralInternalTransformVectors(tangents + 00, rawUniquePositionsPerAxis, a3geomAxis_x);
				a3proceduralInternalTransformVectors(tangents + 30, rawUniquePositionsPerAxis, a3geomAxis_y);
				a3proceduralInternalTransformVectors(tangents + 90, 2, a3geomAxis_x);
				a3proceduralInternalTransformVectors(tangents + 96, 2, a3geomAxis_y);

				// bitangents: all at once
				for (i = 0; i < numVertices; ++i, nPtr += vElems, tPtr += vElems, bPtr += vElems)
					a3real3Cross(bPtr, nPtr, tPtr);

				geomData->attribData[a3attrib_geomTangent] = tangents;
			}
		}
	}


	//-------------------------------------------------------------------------
	// wireframe
	else
	{
		// indices: line strip from center to outside, back, 
		//	and again on the perpendicular axis
		const a3ubyte rawIndices[39] = {
			8, 0, 4, 9, 6, 2, 8, 1, 5, 9, 7, 3, 8,
			8, 0, 4, 9, 6, 2, 8, 1, 5, 9, 7, 3, 8,
			8, 0, 4, 9, 6, 2, 8, 1, 5, 9, 7, 3, 8,
		};
		memcpy(indices, rawIndices, sizeof(rawIndices));

		// offset
		for (i = 0, iPtr = indices + indicesPerAxis; i < indicesPerAxis; ++i, ++iPtr)
			*iPtr += rawUniquePositionsPerAxis;
		for (i = 0; i < indicesPerAxis; ++i, ++iPtr)
			*iPtr += rawUniquePositionsPerAxis + rawUniquePositionsPerAxis;
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