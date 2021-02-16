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
	
	a3_ProceduralGeometry_Torus.inl
	Internal generation functions for Torus.

	**DO NOT MODIFY THIS FILE**
*/

#ifdef __ANIMAL3D_PROCEDURALGEOMETRY_C


a3ret a3proceduralCreateDescriptorTorus(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, const a3_ProceduralGeometryAxis axis, a3f32 radiusMajor, a3f32 radiusMinor, a3ubyte slices, a3ubyte subdivsR)
{
	a3_ProceduralGeometryDescriptor ret = { 0 };
	if (geom_out && !geom_out->shape)
	{
		// validate params
		if (radiusMajor <= 0.0f)
			radiusMajor = 1.0f;
		if (radiusMinor <= 0.0f)
			radiusMinor = 0.25f;
		if (slices < 3)
			slices = 8;
		if (subdivsR < 3)
			subdivsR = 8;
		if (radiusMajor > 0.0f && radiusMinor > 0.0f && slices >= 3 && subdivsR >= 3)
		{
			// store variables
			ret.fParams[fRadius] = radiusMajor;
			ret.fParams[fRadius2] = radiusMinor;
			ret.bParams[bSlices] = slices;
			ret.bParams[bSubdivsR] = subdivsR;

			// create descriptor
			ret.shape = a3geomShape_torus;
			*geom_out = ret;
			return a3proceduralSetDescriptorFlagsAxis(geom_out, flags, axis);
		}
	}
	return -1;
}


a3ret a3proceduralInternalGenerateTorus(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	// recover params and flags
	const a3f32 radiusMajor = geom->fParams[fRadius];
	const a3f32 radiusMinor = geom->fParams[fRadius2];
	const a3ui32 slices = geom->bParams[bSlices];
	const a3ui32 subdivsR = geom->bParams[bSubdivsR];
	const a3_ProceduralGeometryAxis axis = a3proceduralGetDescriptorAxis(geom);
	const a3_ProceduralGeometryFlag flags = a3proceduralGetDescriptorFlags(geom);
	const a3boolean isSolid = (flags != a3geomFlag_wireframe);
	const a3_VertexPrimitiveType primType = geomData->primType = isSolid ? a3prim_triangles : a3prim_lineStrip;

	// get vertex count
	// vertex count: 
	//	solid: (slices + 1)(subdivs + 1) -> grid formation with repeats
	//	wire: (slices)(subdivs) -> grid without repeats, rings
	// index count: 
	//	solid: 6(slices)(subdivs) -> grid
	//	wire: (slices + 1)(subdivs) + (slices)(subdivs + 1) -> rings
	const a3ui32 numVertices = geomData->numVertices = isSolid ? ((slices + 1)*(subdivsR + 1)) : (slices * subdivsR);
	const a3ui32 numIndices = geomData->numIndices = isSolid ? (slices * subdivsR * 6) : ((slices + 1)*(subdivsR)+(slices - 1)*(subdivsR + 1) + 1);
	const a3ui32 vertexSize = a3proceduralInternalCreateFormats(geomData->vertexFormat, geomData->indexFormat, numVertices, numIndices, flags, 0);
	const a3ui32 indexSize = geomData->indexFormat->indexSize;


	// first tangent basis, to be transformed per vertex
	a3f32 t[3] = { 0.0f, 1.0f, 0.0f }, b[3] = { 0.0f, 0.0f, 1.0f }, n[3] = { 1.0f, 0.0f, 0.0f };
	a3f32 tBase[3] = { 1.0f, 0.0f, 0.0f }, bBase[3] = { 0.0f, -1.0f, 0.0f }, nBase[3] = { 0.0f, 0.0f, -1.0f };

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
	a3ui32 i = 0, j = 0, k = 0;
	a3boolean valid = 0;


	//-------------------------------------------------------------------------
	// solid
	if (isSolid)
	{
		// body: a bunch of rings, circle can be generated using 
		//	procedural circle algorithm
		//	radius of each ring is sin(elev) and 
		//	depths are measured as cos(elev), where elev = [180, 0]
		const a3f32 dU = 1.0f / (a3f32)slices;
		const a3f32 dV = 1.0f / (a3f32)subdivsR;
		const a3f32 deltaElev = 360.0f / (a3f32)subdivsR;
		a3f32 elev = 0.0f, ringRadius[1], ringDepth[1];
		
		// inner-most circle
		attribItr = a3proceduralInternalCreateCircle(attribItr, radiusMajor - radiusMinor, 0.0f, 0.0f, 0.0f, 0.0f, slices, vElems, 1, 0);
		for (i = 1, elev = deltaElev; i < subdivsR; elev = deltaElev*(a3f32)(++i))
		{
			a3trigTaylor_sind_cosd(elev, ringDepth, ringRadius);
			attribItr = a3proceduralInternalCreateCircle(attribItr, radiusMajor - radiusMinor * *ringRadius, 0.0f, 0.0f, -radiusMinor * *ringDepth, 0.0f, slices, vElems, 1, 0);
		}
		// inner-most again
		attribItr = a3proceduralInternalCreateCircle(attribItr, radiusMajor - radiusMinor, 0.0f, 0.0f, 0.0f, 0.0f, slices, vElems, 1, 0);

		// done, transform AT THE END because current state used for normals
		assert(valid = attribItr == texcoords);


		// indices: literally a grid
		i = 0;
		indexItr = a3proceduralInternalCreatePlaneIndicesSolid(indexItr, slices, subdivsR, indexSize, &i, 1, 0);
		assert(valid = indexItr == indexDataEnd);


		// create texture coordinates (DO NOT TRANSFORM)
		if (flags & a3geomFlag_texcoords)
		{
			// body: straight up grid
			attribItr = texcoords;
			attribItr = a3proceduralInternalCreatePlane(attribItr, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, slices, subdivsR, tElems, 1);

			assert(valid = attribItr == normals);

			geomData->attribData[a3attrib_geomTexcoord] = texcoords;
		}
		
		// create normals
		if (flags & a3geomFlag_normals)
		{
			// normals are easy: normalize the position
			// ...but we need the length; hard-code the first 
			//	triangle to calculate correct vector length
			const a3ui32 bodyVerts = (slices + 1)*(subdivsR + 1);
			const a3ui32 capVerts = slices * 2;
			const a3f32 deltaAzim = 360.0f / (a3f32)slices;
			const a3f32 p0[3] = {
				radiusMajor - radiusMinor, 0.0f, 0.0f
			}, p1[3] = {
				(radiusMajor - radiusMinor)*a3cosdTaylor(deltaAzim), (radiusMajor - radiusMinor)*a3sindTaylor(deltaAzim), 0.0f
			}, p2[3] = {
				radiusMajor - radiusMinor*a3cosdTaylor(deltaElev), 0.0f, -radiusMinor*a3sindTaylor(deltaElev)
			}, tc0[2] = {
				0.0f, 0.0f
			}, tc1[2] = {
				dU, 0.0f
			}, tc2[2] = {
				0.0f, dV
			};
			a3f32 corePos[3] = { radiusMajor, 0.0f, 0.0f };
			a3f32 deltaPos[3] = { 0.0f, 0.0, 0.0f };
			a3f32 tLen = 0.0f, bLen = 0.0f, nLen = 0.0f;
			a3f32 lenConvert;
			a3proceduralInternalCalculateTangentBasis(t, b, n, p0, p1, p2, tc0, tc1, tc2);
			tLen = a3real3Length(t);
			bLen = a3real3Length(b);
			nLen = a3real3Length(n);
		//	lenConvert = 1.0f / radiusMinor;
			lenConvert = nLen / radiusMinor;

			// delta from core
			attribItr = attribItr2 = normals, pPtr = positions;
			for (i = (slices+1)*vElems, j = 0; j <= slices; ++j, attribItr = normals + j*vElems, pPtr = positions + j*vElems)
			{
				// calculate core
				a3trigTaylor_sind_cosd(deltaAzim*(a3f32)j, corePos + 1, corePos);
				corePos[0] *= radiusMajor;
				corePos[1] *= radiusMajor;
				for (k = 0; k <= subdivsR; ++k, attribItr += i, pPtr += i, attribItr2 += vElems)
				{
					deltaPos[0] = pPtr[0] - corePos[0];
					deltaPos[1] = pPtr[1] - corePos[1];
					deltaPos[2] = pPtr[2] - corePos[2];
					attribItr[0] = deltaPos[0] * lenConvert;
					attribItr[1] = deltaPos[1] * lenConvert;
					attribItr[2] = deltaPos[2] * lenConvert;
				}
			}

			// all iterations occurred
			assert(valid = attribItr2 == tangents);

			geomData->attribData[a3attrib_geomNormal] = normals;
			
			// create tangents and bitangents
			if (flags & a3geomFlag_tangentFlag)
			{
				attribItr = tangents;
				// create body tangents once and copy to all stacks
				// just do a circle with a 90 degree offset
				lenConvert = tLen;
			//	lenConvert = 1.0f;
				attribItr2 = a3proceduralInternalCreateCircle(attribItr, lenConvert, 0.0f, 0.0f, 0.0f, 90.0f, slices, vElems, 1, 0);
				for (j = 1; j <= subdivsR; ++j)
					for (k = 0, attribItr3 = attribItr; k <= slices; ++k, attribItr2 += vElems, attribItr3 += vElems)
					{
						attribItr2[0] = attribItr3[0];
						attribItr2[1] = attribItr3[1];
						attribItr2[2] = attribItr3[2];
					}
				assert(valid = attribItr2 == bitangents);

				// bitangents: all at once
				attribItr = tangents;
				attribItr2 = bitangents;
				attribItr3 = normals;
				for (i = 0; i < numVertices; ++i, attribItr += vElems, attribItr2 += vElems, attribItr3 += vElems)
					a3real3Cross(attribItr2, attribItr3, attribItr);
				assert(valid = attribItr == bitangents);
				assert(valid = attribItr2 == vertexDataEnd);

				// done
				geomData->attribData[a3attrib_geomTangent] = tangents;

				// transform all
				a3proceduralInternalTransformVectors(tangents, numVertices, axis);
				a3proceduralInternalTransformVectors(bitangents, numVertices, axis);
			}

			// transform normals
			a3proceduralInternalTransformVectors(normals, numVertices, axis);
		}

		// transform positions
		a3proceduralInternalTransformVectors(positions, numVertices, axis);
	}


	//-------------------------------------------------------------------------
	// wireframe
	else
	{
		// body: a bunch of rings
		const a3f32 deltaElev = 360.0f / (a3f32)subdivsR;
		a3f32 elev = 0.0f, ringRadius[1], ringDepth[1];

		// inner-most circle
		attribItr = a3proceduralInternalCreateCircle(attribItr, radiusMajor - radiusMinor, 0.0f, 0.0f, 0.0f, 0.0f, slices, vElems, 0, 0);
		for (i = 1, elev = deltaElev; i < subdivsR; elev = deltaElev*(a3f32)(++i))
		{
			a3trigTaylor_sind_cosd(elev, ringDepth, ringRadius);
			attribItr = a3proceduralInternalCreateCircle(attribItr, radiusMajor - radiusMinor * *ringRadius, 0.0f, 0.0f, -radiusMinor * *ringDepth, 0.0f, slices, vElems, 0, 0);
		}

		// done, transform AT THE END because current state used for normals
		assert(valid = attribItr == texcoords);


		// indices: 
		// body: closed loops
		for (i = 0, j = 0; j < subdivsR; ++j, i += slices)
			indexItr = a3proceduralInternalStoreRingIndices(indexItr, indexSize, slices + 1, i, 1);

		// close loop by storing first
		indexItr = a3proceduralInternalStoreIndex(indexItr, indexSize, 0);

		// closed loops about the slices
		for (i = 1, j = 1; j < slices; ++j, ++i)
		{
			for (k = 0; k < subdivsR; ++k, i += slices)
				indexItr = a3proceduralInternalStoreIndex(indexItr, indexSize, i);
			indexItr = a3proceduralInternalStoreIndex(indexItr, indexSize, (i -= slices*subdivsR));
		}

		// done
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