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
	
	a3_ProceduralGeometry_Circle.inl
	Internal generation functions for Circle.

	**DO NOT MODIFY THIS FILE**
*/

#ifdef __ANIMAL3D_PROCEDURALGEOMETRY_C


//-----------------------------------------------------------------------------
// utility to generate circle vertices (this will be super useful later when 
//	the 3D round objects need a billion circles)

inline a3f32 *a3proceduralInternalCreateCircle(a3f32 *position, const a3f32 radius, const a3f32 xOffset, const a3f32 yOffset, const a3f32 zOffset, const a3f32 angleOffset, const a3ui32 slices, const a3ui32 attribElems, const a3boolean repeatFirst, const a3i32 flipVertical)
{
	// fill positions array algorithmically
	a3f32 theta, deltaTheta = a3real_threesixty / (a3f32)slices;
	const a3f32 verticalSign = flipVertical ? -1.0f : +1.0f;
	a3ui32 i;

	// first
//	position[0] = xOffset + radius;
//	position[1] = yOffset;
//	position[2] = zOffset;
//	position += attribElems;

	// create a circle
//	for (i = 1, theta = deltaTheta; i < slices; theta = deltaTheta * (a3f32)(++i), position += attribElems)
	for (i = 0, theta = angleOffset; i < slices; theta = angleOffset + deltaTheta * (a3f32)(++i), position += attribElems)
	{
		a3trigTaylor_sind_cosd(theta, position + 1, position);
		position[0] = xOffset + radius*position[0];
		position[1] = yOffset + radius*position[1] * verticalSign;
		position[2] = zOffset;
	}

	// repeat
	if (repeatFirst)
	{
		a3trigTaylor_sind_cosd(angleOffset, position + 1, position);
		position[0] = xOffset + radius*position[0];
		position[1] = yOffset + radius*position[1] * verticalSign;
	//	position[0] = xOffset + radius;
	//	position[1] = yOffset;
		position[2] = zOffset;
		position += attribElems;
	}

	// done, return where we end off
	return position;
}


// circle indices
inline a3ubyte *a3proceduralInternalStoreRingIndices(a3ubyte *index, const a3ui32 indexSize, const a3ui32 slices, const a3ui32 ringIndex, const a3i32 firstIsLast)
{
	a3ui32 sl;
	a3ui32 i = ringIndex;
	const a3ui32 j = firstIsLast ? slices - 1 : slices;
	const a3i32 direction = +1;

	for (sl = 0; sl < j; ++sl, i += direction)
		index = a3proceduralInternalStoreIndex(index, indexSize, i);
	if (firstIsLast)
		index = a3proceduralInternalStoreIndex(index, indexSize, ringIndex);

	// done
	return index;
}

inline a3ubyte *a3proceduralInternalStoreStripIndices(a3ubyte *index, const a3ui32 indexSize, const a3ui32 slices, const a3ui32 outerRingIndex, const a3i32 firstIsLast)
{
	a3ui32 sl;
	a3ui32 i = outerRingIndex;
	const a3ui32 j = firstIsLast ? slices - 1 : slices;
	const a3ui32 k = j + 1;	// delta to next ring index
	const a3i32 direction = +1;

	for (sl = 0; sl < j; ++sl, i += direction)
	{
		// form 2 triangles
		index = a3proceduralInternalStoreTriangleIndices(index, indexSize, i, i + 1, i + k);
		index = a3proceduralInternalStoreTriangleIndices(index, indexSize, i + 1 + k, i + k, i + 1);
	}
	if (firstIsLast)
	{
		index = a3proceduralInternalStoreTriangleIndices(index, indexSize, i, outerRingIndex, i + k);
		index = a3proceduralInternalStoreTriangleIndices(index, indexSize, outerRingIndex + k, i + k, outerRingIndex);
	}

	//done
	return index;
}

inline a3ubyte *a3proceduralInternalStoreCircleCapIndices(a3ubyte *index, const a3ui32 indexSize, const a3ui32 slices, const a3ui32 centerIndex, const a3ui32 ringIndex, const a3i32 firstIsLast)
{
	a3ui32 sl;
	a3ui32 i = ringIndex;
	const a3ui32 j = firstIsLast ? slices - 1 : slices;
	const a3i32 direction = +1;

	// per slice
	for (sl = 0; sl < j; ++sl, i += direction)
		index = a3proceduralInternalStoreTriangleIndices(index, indexSize, centerIndex, i, i + direction);
	if (firstIsLast)
		index = a3proceduralInternalStoreTriangleIndices(index, indexSize, centerIndex, i, ringIndex);

	// done
	return index;
}

inline a3ubyte *a3proceduralInternalStoreCircleCapIndicesSolid(a3ubyte *index, const a3ui32 indexSize, const a3ui32 slices, const a3ui32 centerIndex, const a3ui32 ringIndex, const a3i32 reverse)
{
	a3ui32 sl;

	// start counting at last body strip
	a3ui32 i = reverse ? ringIndex + slices : ringIndex;
	a3ui32 j = reverse ? centerIndex + slices - 1 : centerIndex;
	const a3i32 direction = reverse ? -1 : +1;

	// per slice
	for (sl = 0; sl < slices; ++sl, i += direction, j += direction)
		index = a3proceduralInternalStoreTriangleIndices(index, indexSize, i, i + direction, j);

	// done
	return index;
}


// circle indices wireframe
inline a3ubyte *a3proceduralInternalStoreCircleIndicesWireSpokes(a3ubyte *index, const a3ui32 indexSize, const a3ui32 slices, const a3ui32 subdivsBase, const a3ui32 centerIndex, const a3ui32 innerRingIndex, const a3ui32 outerRingIndex, const a3i32 centerDirection, const a3i32 innerRingDirection, const a3i32 outerRingDirection, const a3i32 returnToFirst)
{
	const a3ui32 secondRingIndex = (innerRingIndex - slices * (subdivsBase - 2));
	a3ui32 sl, st;
	a3ui32 i, i_outer = outerRingIndex + (outerRingDirection + slices) % slices, i_inner = innerRingIndex + (innerRingDirection + slices) % slices, i_second = secondRingIndex + (innerRingDirection + slices) % slices, i_center = centerIndex + (centerDirection + slices) % slices;
	a3i32 direction = -1;

	// do all slices from 1 to last, reversing direction each time
	for (sl = 1; sl < slices; ++sl, i_outer += outerRingDirection, i_inner += innerRingDirection, i_second += innerRingDirection, i_center += centerDirection, direction = -direction)
	{
		// if moving away from center, start with center
		// otherwise, start with outer
		index = a3proceduralInternalStoreIndex(index, indexSize, (direction < 0 ? i_center : i_outer));
	
		// spoke
		for (st = 1, i = (direction < 0 ? i_inner : i_second); st < subdivsBase; ++st, i += direction * slices)
			index = a3proceduralInternalStoreIndex(index, indexSize, i);
		
		// complete the current strip depending on the direction
		if (outerRingDirection && direction < 0)
			index = a3proceduralInternalStoreIndex(index, indexSize, i_outer);
		if (centerDirection && direction > 0)
			index = a3proceduralInternalStoreIndex(index, indexSize, i_center);
	}

	// close off
	if (returnToFirst)
	{
		if (slices % 2)
		{
			// odd number of slices: 
			// final slice is the first one (slice 0)
			index = a3proceduralInternalStoreIndex(index, indexSize, centerIndex);
			for (st = 1, i = innerRingIndex; st < subdivsBase; ++st, i -= slices)
				index = a3proceduralInternalStoreIndex(index, indexSize, i);

			// test: set 'st' to 2 above and uncomment this
			// otherwise leave this out and 'st' should be 1
			// reveals where the last two indices point to
		//	index = a3proceduralInternalStoreIndex(index, indexSize, centerIndex);
		}

		// append first index
		index = a3proceduralInternalStoreIndex(index, indexSize, outerRingIndex);
	}

	// done
	return index;
}


//-----------------------------------------------------------------------------

// calculate number of vertices and indices for circle; can be used for 
//	setting up the base parts of round shapes
inline a3ui32 a3proceduralInternalCircleNumVertices(const a3ui32 slices, const a3ui32 subdivsBase, const a3boolean isSolid)
{
	// one ring of verts per subdiv (no repeat), add one for center if either solid or subdivided
	a3ui32 numVertices = slices*subdivsBase + (isSolid || subdivsBase > 1);

	// done
	return numVertices;
}

inline a3ui32 a3proceduralInternalCircleSpokeNumIndices(const a3ui32 slices, const a3ui32 subdivsBase)
{
	return ((slices - 1)*subdivsBase + (slices + 1) / 2 + (slices % 2 ? subdivsBase : 1));
}

inline a3ui32 a3proceduralInternalCircleNumIndices(const a3ui32 slices, const a3ui32 subdivsBase, const a3boolean isSolid)
{
	a3ui32 numIndices = 0;
	if (isSolid)
		// indices needed to make triangles: 
		//	inner ring = 3*slices + 6*slices*additional subdivisions
		numIndices = 3 * slices + (subdivsBase - 1) * slices * 6;
	else if (subdivsBase > 1)
		// if subdivided, (number of slices + 1) * subdivs for 'spiral loops', then add 'spokes'
		// spokes: e.g. 5 slices, 3 divs: center, slice1_1,2,3, slice2_3,2,1, center, slice3_1,2,3, slice4_3,2,1, center, sliceLast_1,2,3
		// spokes: e.g. 6 slices, 3 divs: center, slice1_1,2,3, slice2_3,2,1, center, slice3_1,2,3, slice4_3,2,1, center, slice5_1,2,3, sliceLast_3
		// spokes: center occurs (slices + 1)/2 times, number of spoke indices is (slices)*(divs - 1)
		//	after all slices (i < slices), if even, repeat first, if odd, repeat first slice as spoke
		numIndices = ( (slices + 1)*subdivsBase ) + a3proceduralInternalCircleSpokeNumIndices(slices, subdivsBase);
	else
		// otherwise, number of slices + 1 (closed loop)
		numIndices = slices + 1;

	// done
	return numIndices;
}


// single algorithm to generate all the data for a circle
void a3proceduralInternalCircleGenerateAlgorithm(a3f32 *positions_out, a3f32 *texcoords_out, a3f32 *normals_out, a3f32 *tangents_out, a3f32 *bitangents_out, a3ubyte *indices_out, const a3f32 radius, const a3ui32 slices, const a3ui32 subdivsBase, const a3f32 xOffset, const a3f32 yOffset, const a3f32 zOffset, const a3f32 uOffset, const a3f32 vOffset, const a3f32 uvRadius, const a3ui32 numVerts, const a3ui32 numInds, const a3ui32 indexSize, const a3ui32 firstIndex, const a3boolean isSolid, const a3i32 flipVertical)
{
	const a3f32 *const positions_start = positions_out, *const positions_end = positions_start + numVerts*vElems;
	const a3f32 *const texcoords_start = texcoords_out, *const texcoords_end = texcoords_start + numVerts*tElems;
	const a3f32 *const normals_start = normals_out, *const normals_end = normals_start + numVerts*vElems;
	const a3f32 *const tangents_start = tangents_out, *const tangents_end = tangents_start + numVerts*vElems;
	const a3f32 *const bitangents_start = bitangents_out, *const bitangents_end = bitangents_start + numVerts*vElems;
	const a3ubyte *const indices_start = indices_out, *const indices_end = indices_start + numInds*indexSize;
	a3boolean valid = 0;

	// tangent basis
	a3f32 t[3] = { 1.0f, 0.0f, 0.0f }, b[3] = { 0.0f, 1.0f, 0.0f }, n[3] = { 0.0f, 0.0f, 1.0f };

	// helper data
	// radius change from inner division to inner division
	a3f32 deltaRadius = 1.0f;
	a3f32 r = 1.0f;
	a3ui32 i = 0, j = 0, k = 0;


	//-------------------------------------------------------------------------
	// common

	// fill positions array algorithmically
	// repeat last because it is a closed triangle fan
	// final vertex is center
	for (i = 0, r = radius, deltaRadius = r / (a3f32)subdivsBase; i < subdivsBase; ++i, r = radius - deltaRadius*(a3f32)i)
		positions_out = a3proceduralInternalCreateCircle(positions_out, r, xOffset, yOffset, zOffset, 0.0f, slices, vElems, 0, flipVertical);

	// add center if solid or using subdivisions
	if (isSolid || subdivsBase > 1)
	{
		positions_out[0] = xOffset;
		positions_out[1] = yOffset;
		positions_out[2] = zOffset;
		positions_out += vElems;
	}

	// validate end of position data
	assert(valid = positions_out == positions_end);


	//-------------------------------------------------------------------------
	// solid
	if (isSolid)
	{
		// indices
		if (indices_out && numInds)
		{
			// first do outer rings, then use function to close the center
			for (i = 1, k = firstIndex; i < subdivsBase; ++i, k += slices)
				indices_out = a3proceduralInternalStoreStripIndices(indices_out, indexSize, slices, k, 1);
			indices_out = a3proceduralInternalStoreCircleCapIndices(indices_out, indexSize, slices, k + slices, k, 1);
		
			// valid end index
			assert(valid = indices_out == indices_end);
		}


		// create texture coordinates (DO NOT TRANSFORM)
		if (texcoords_start)
		{
			// use handy algorithm instead of rewriting
			// offset the whole thing by half a unit, radius is half
			// center vertex is half-half
			for (i = 0, r = uvRadius, deltaRadius = r / (a3f32)subdivsBase; i < subdivsBase; ++i, r = uvRadius - deltaRadius*(a3f32)i)
				texcoords_out = a3proceduralInternalCreateCircle(texcoords_out, r, uOffset, vOffset, 0.0f, 0.0f, slices, tElems, 0, 0);
			texcoords_out[0] = uOffset;
			texcoords_out[1] = vOffset;
			texcoords_out += tElems;
		//	const a3f32 *positions_itr = positions_start;
		//	for (i = 0; i < numVerts; ++i, texcoords_out += tElems, positions_itr += vElems)
		//	{
		//		texcoords_out[0] = positions_itr[0] / radius * 0.5f + 0.5f;
		//		texcoords_out[1] = positions_itr[1] / radius * 0.5f + 0.5f;
		//	}

			// validate
			assert(valid = texcoords_out == texcoords_end);

			// test tangent basis function
			a3proceduralInternalCalculateTangentBasis(t, b, n,
				positions_start, positions_start + vElems, positions_start + slices * vElems,
				texcoords_start, texcoords_start + tElems, texcoords_start + slices * tElems
			);
			// ((slices + 2) / 4 + 1)
		}
		else
		{
			// calculate normals
			a3proceduralInternalCalculateNormal(n, positions_start, positions_start + vElems, positions_start + slices * vElems);
		}

		// create normals
		if (normals_start)
		{
			// the whole thing is a copy of the Z axis
			for (i = 0; i < numVerts; ++i, normals_out += vElems)
			{
				normals_out[0] = n[0];
				normals_out[1] = n[1];
				normals_out[2] = n[2];
			}
			assert(valid = normals_out == normals_end);

			// create tangents and bitangents
			if (tangents_start)
			{
				// copy tangent and bitangent
				for (i = 0; i < numVerts; ++i, tangents_out += vElems, bitangents_out += vElems)
				{
					tangents_out[0] = t[0];
					tangents_out[1] = t[1];
					tangents_out[2] = t[2];
					bitangents_out[0] = b[0];
					bitangents_out[1] = b[1];
					bitangents_out[2] = b[2];
				}
				assert(valid = tangents_out == tangents_end);
				assert(valid = bitangents_out == bitangents_end);
			}
		}

	}


	//-------------------------------------------------------------------------
	// wireframe
	else if (indices_out && numInds)
	{
		// indices
		// start with closed loops, then create spokes
		for (i = 0, k = firstIndex; i < subdivsBase; ++i, k += slices)
			indices_out = a3proceduralInternalStoreRingIndices(indices_out, indexSize, slices + 1, k, 1);
		if (subdivsBase > 1)
			indices_out = a3proceduralInternalStoreCircleIndicesWireSpokes(indices_out, indexSize, slices, subdivsBase, k, firstIndex + (subdivsBase - 1)*slices, firstIndex, 0, +1, +1, 1);

		assert(valid = indices_out == indices_end);
	}
}


//-----------------------------------------------------------------------------


a3ret a3proceduralCreateDescriptorCircle(a3_ProceduralGeometryDescriptor *geom_out, const a3_ProceduralGeometryFlag flags, const a3_ProceduralGeometryAxis axis, a3f32 radius, a3ubyte slices, a3ubyte subdivsBase)
{
	a3_ProceduralGeometryDescriptor ret = { 0 };
	if (geom_out && !geom_out->shape)
	{
		// validate params
		if (radius <= 0.0f)
			radius = 1.0f;
		if (slices < 3)
			slices = 8;
		if (subdivsBase < 1)
			subdivsBase = 4;
		if (radius > 0.0f && slices >= 3 && subdivsBase)
		{
			// store variables
			ret.fParams[fRadius] = radius;
			ret.bParams[bSlices] = slices;
			ret.bParams[bSubdivsB] = subdivsBase;

			// create descriptor
			ret.shape = a3geomShape_circle;
			*geom_out = ret;
			return a3proceduralSetDescriptorFlagsAxis(geom_out, flags, axis);
		}
	}
	return -1;
}


a3ret a3proceduralInternalGenerateCircle(a3_GeometryData *geomData, const a3_ProceduralGeometryDescriptor *geom)
{
	// recover params and flags
	const a3f32 radius = geom->fParams[fRadius];
	const a3ubyte slices = geom->bParams[bSlices];
	const a3ubyte subdivsBase = geom->bParams[bSubdivsB];
	const a3_ProceduralGeometryAxis axis = a3proceduralGetDescriptorAxis(geom);
	const a3_ProceduralGeometryFlag flags = a3proceduralGetDescriptorFlags(geom);
	const a3boolean isSolid = (flags != a3geomFlag_wireframe);
	const a3_VertexPrimitiveType primType = geomData->primType = isSolid ? a3prim_triangles : a3prim_lineStrip;

	// only use indices if using subdivisions
	const a3ui32 numVertices = geomData->numVertices = a3proceduralInternalCircleNumVertices(slices, subdivsBase, isSolid);
	const a3ui32 numIndices = geomData->numIndices = (isSolid || subdivsBase > 1) ? a3proceduralInternalCircleNumIndices(slices, subdivsBase, isSolid) : 0;
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
	a3f32 *attribItr;
	a3ui32 i = 0;
	a3boolean valid = 0;


	//-------------------------------------------------------------------------
	// for just a circle, wireframe is special case
	if (isSolid)
	{
		// using algorithm
		a3proceduralInternalCircleGenerateAlgorithm(positions, (flags & a3geomFlag_texcoords ? texcoords : 0), (flags & a3geomFlag_normals ? normals : 0), (flags & a3geomFlag_tangentFlag ? tangents : 0), (flags & a3geomFlag_tangentFlag ? bitangents : 0),
			indices, radius, slices, subdivsBase, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, numVertices, numIndices, indexSize, 0, isSolid, 0);

		a3proceduralInternalTransformVectors(positions, numVertices, axis);

		if (flags & a3geomFlag_texcoords)
		{
			geomData->attribData[a3attrib_geomTexcoord] = texcoords;
		}

		if (flags & a3geomFlag_normals)
		{
			geomData->attribData[a3attrib_geomNormal] = normals;

			a3proceduralInternalTransformVectors(normals, numVertices, axis);

			if (flags & a3geomFlag_tangentFlag)
			{
				geomData->attribData[a3attrib_geomTangent] = tangents;

				a3proceduralInternalTransformVectors(tangents, numVertices, axis);
				a3proceduralInternalTransformVectors(bitangents, numVertices, axis);
			}
		}
	}
	else
	{
		if (subdivsBase > 1)
		{
			// using algo
			a3proceduralInternalCircleGenerateAlgorithm(positions, (flags & a3geomFlag_texcoords ? texcoords : 0), (flags & a3geomFlag_normals ? normals : 0), (flags & a3geomFlag_tangentFlag ? tangents : 0), (flags & a3geomFlag_tangentFlag ? bitangents : 0),
				indices, radius, slices, subdivsBase, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, numVertices, numIndices, indexSize, 0, isSolid, 0);
		}
		else
		{
			// just do positions, algorithmically
			// no center or extra at end
			attribItr = a3proceduralInternalCreateCircle(positions, radius, 0.0f, 0.0f, 0.0f, 0.0f, slices, vElems, 0, 0);
			assert(valid = attribItr == vertexDataEnd);
			geomData->primType = a3prim_lineLoop;
		}

		a3proceduralInternalTransformVectors(positions, numVertices, axis);
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