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
	
	a3_GeometryData.c
	Definitions for geometry data and vertex descriptors.

	**DO NOT MODIFY THIS FILE**
*/

#include "animal3D/a3geometry/a3_GeometryData.h"

#include "animal3D-A3DM/a3math/a3vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------

a3ret a3geometryCreateVertexFormat(a3_VertexFormatDescriptor *vertexFormat_out, const a3_GeometryVertexAttributeName *attribNameList, a3ui32 attribNameCount)
{
	static const a3ui16 secondaryAttribOffset = 2;
	static const a3_VertexAttributeDescriptor fixedAttribs[] = {
		// 6 explicit
		{ a3attrib_position,		a3attrib_vec3	},
		{ a3attrib_normal,			a3attrib_vec3	},
		{ a3attrib_color,			a3attrib_vec4	},
		{ a3attrib_texcoord,		a3attrib_vec2	},
		{ a3attrib_tangent,			a3attrib_vec3	},
		{ a3attrib_blendWeights,	a3attrib_vec4	},
		// 2 implicit
		{ a3attrib_bitangent,		a3attrib_vec3	},
		{ a3attrib_blendIndices,	a3attrib_ivec4	},
	};

	// prepare list of actual attributes
	const a3_VertexAttributeDescriptor *attribOrdered[8] = { 0 };
	a3_VertexAttributeDescriptor attrib[8] = { 0 };
	a3ui32 i, j;
	a3ui32 attribCount = 0;

	if (vertexFormat_out)
	{
		// enable position
		attribOrdered[0] = fixedAttribs + 0;
		++attribCount;

		// configure the rest
		attribNameCount = attribNameCount <= a3attrib_geomNameMax ? attribNameCount : a3attrib_geomNameMax;
		for (i = 0; i < attribNameCount; ++i)
		{
			j = attribNameList[i];
			if (!attribOrdered[j])
			{
				attribOrdered[j] = fixedAttribs + j;
				++attribCount;
			}
			else
				printf("\n A3 Warning: Duplicate geometry attribute (%u) passed to geometry vertex descriptor; ignoring.", (a3ui32)(attribNameList[i]));
		}

		// evaluate extra: tangents also come with bitangents
		if (attribOrdered[a3attrib_geomTangent])
		{
			// normals and texture coordinates must be enabled
			if (attribOrdered[a3attrib_geomNormal] && attribOrdered[a3attrib_geomTexcoord])
			{
				// enable bitangents as well
				j = a3attrib_geomTangent + secondaryAttribOffset;
				attribOrdered[j] = fixedAttribs + j;
				++attribCount;
			}
			else
			{
				printf("\n A3 Warning: Geometry tangents require normals and texture coordinates to be active; disabling.");
				attribOrdered[a3attrib_geomTangent] = 0;
			}
		}

		// blending
		if (attribOrdered[a3attrib_geomBlending])
		{
			j = a3attrib_geomBlending + secondaryAttribOffset;
			attribOrdered[j] = fixedAttribs + j;
			++attribCount;
		}

		// copy fixed to local
		for (i = j = 0; i < attribCount; ++j)
			if (attribOrdered[j])
				attrib[i++] = *attribOrdered[j];

		// create format
		return a3vertexFormatCreateDescriptor(vertexFormat_out, attrib, attribCount);
	}
	return -1;
}

a3ret a3geometryCreateIndexFormat(a3_IndexFormatDescriptor *indexFormat_out, const a3ui32 vertexCount)
{
	const a3_IndexType indexType = (vertexCount ? vertexCount < a3index_countMaxByte ? a3index_byte : vertexCount < a3index_countMaxShort ? a3index_short : a3index_int : a3index_disable);
	return a3indexFormatCreateDescriptor(indexFormat_out, indexType);
}


//-----------------------------------------------------------------------------

a3ret a3geometryGenerateDrawable(a3_VertexDrawable *drawable_out, const a3_GeometryData *geom, a3_VertexArrayDescriptor *vertexArray, a3_IndexBuffer *indexBuffer, const a3_IndexFormatDescriptor *commonIndexFormat_opt, a3ui32 *vertexBufferOffset_out_opt, a3ui32 *indexBufferOffset_out_opt)
{
	static const a3_VertexAttributeName fixedAttribName[] = {
		a3attrib_position,
		a3attrib_normal,
		a3attrib_color,
		a3attrib_texcoord,
		a3attrib_tangent,
		a3attrib_blendWeights,
		a3attrib_bitangent,
		a3attrib_blendIndices,
	};

	// create geometry attribute list
	// attribute data to store
	a3_VertexAttributeDataDescriptor attribData[a3attrib_geomNameMax + 2] = { 0 }, *attribDataPtr = attribData;
	a3_VertexAttributeName fixedName;
	a3ui32 numAttribs = 0;
	a3ui32 i;
	a3i32 ret = 0;

	// validate params
	if (geom && geom->data && drawable_out && vertexArray)
	{
		// validate init
		if (!drawable_out->vertexArray && vertexArray->handle->handle)
		{
			// validate vertex format compatibility with VAO
			if (memcmp(geom->vertexFormat, vertexArray->vertexFormat, sizeof(a3_VertexFormatDescriptor)))
			{
				printf("\n A3 ERROR: Drawable creation failed; vertex array and geometry data formats do not match.");
				return -1;
			}


			// validate index buffer provided if needed
			if (geom->numIndices && (!indexBuffer || !indexBuffer->handle->handle))
			{
				printf("\n A3 ERROR: Drawable creation failed; missing or uninitialized index buffer.");
				return -1;
			}


			// enable data descriptors
			for (i = 0; i < a3attrib_geomNameMax; ++i)
				if (geom->attribData[i])
				{
					attribData[numAttribs].name = fixedAttribName[i];
					attribData[numAttribs].data = geom->attribData[i];
					++numAttribs;
				}

			// if tangents are enabled, get data
			if (geom->attribData[a3attrib_geomTangent])
			{
				attribData[numAttribs].name = fixedName = fixedAttribName[a3attrib_geomTangent + 2];
				attribData[numAttribs].data
					= (a3f32 *)(geom->attribData[a3attrib_geomTangent])
					+ geom->vertexFormat->attribElements[fixedName] * geom->numVertices;
				++numAttribs;
			}

			// if blend weights are enabled, them too
			if (geom->attribData[a3attrib_geomBlending])
			{
				attribData[numAttribs].name = fixedName = fixedAttribName[a3attrib_geomBlending + 2];
				attribData[numAttribs].data
					= (a3f32 *)(geom->attribData[a3attrib_geomBlending])
					+ geom->vertexFormat->attribElements[fixedName] * geom->numVertices;
				++numAttribs;
			}


			// send data to index buffer and create drawable
			if (geom->numIndices)
			{
				// send data to vertex buffer, index buffer, then create drawable
				ret = a3vertexArrayStore(vertexArray, attribData, geom->numVertices, vertexBufferOffset_out_opt, &i);
				a3indexBufferStore(indexBuffer, geom->indexFormat, geom->indexData, geom->numIndices, i, &i, commonIndexFormat_opt);
				a3vertexDrawableCreateIndexed(drawable_out, vertexArray, indexBuffer,
					(commonIndexFormat_opt ? commonIndexFormat_opt : geom->indexFormat),
					geom->primType, i, geom->numIndices);
				if (indexBufferOffset_out_opt)
					*indexBufferOffset_out_opt = i;
			}

			// no indices, create non-indexed drawable
			else
			{
				// store and create drawable
				ret = a3vertexArrayStore(vertexArray, attribData, geom->numVertices, vertexBufferOffset_out_opt, &i);
				a3vertexDrawableCreate(drawable_out, vertexArray, geom->primType, i, geom->numVertices);
			}

			// done
			return ret;
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------

a3ret a3geometrySaveDataBinary(const a3_GeometryData *geom, const a3_FileStream *fileStream)
{
	FILE *fp = 0;
	a3ui32 ret = 0;
	a3ui32 vertexDataSize = 0, dataSize = 0, indexDataSize = 0;
	a3ui32 i = 0;
	a3i32 offset[a3attrib_geomNameMax + 1] = { 0 };
	if (geom && fileStream)
	{
		if (geom->data)
		{
			fp = fileStream->stream;
			if (fp)
			{
				// write vertex format and size
				ret += (a3ui32)fwrite(geom->vertexFormat, 
					1, sizeof(a3_VertexFormatDescriptor), fp);
				ret += (a3ui32)fwrite(geom->indexFormat,
					1, sizeof(a3_IndexFormatDescriptor), fp);

				// write counts and flags
				ret += (a3ui32)fwrite(&geom->primType, 
					1, sizeof(a3_VertexPrimitiveType), fp);
				ret += (a3ui32)fwrite(&geom->numVertices, 
					1, sizeof(a3ui32), fp);
				ret += (a3ui32)fwrite(&geom->numIndices,
					1, sizeof(a3ui32), fp);

				// write raw data
				vertexDataSize = a3vertexFormatGetStorageSpaceRequired(geom->vertexFormat, geom->numVertices);
				indexDataSize = a3indexFormatGetStorageSpaceRequired(geom->indexFormat, geom->numIndices);
				dataSize = (vertexDataSize + indexDataSize);
				ret += (a3ui32)fwrite(geom->data, 1, dataSize, fp);

				// write attribute offsets for raw data
				for (i = 0; i < a3attrib_geomNameMax; ++i)
					offset[i] = geom->attribData[i] ? (a3i32)((a3byte *)(geom->attribData[i]) - (a3byte *)(geom->data)) : -1;
				offset[i] = geom->indexData ? (a3i32)((a3byte *)(geom->indexData) - (a3byte *)(geom->data)) : -1;
				ret += (a3ui32)fwrite(offset, 1, sizeof(a3i32) * (a3attrib_geomNameMax + 1), fp);
			}
			return ret;
		}
	}
	return -1;
}

a3ret a3geometryLoadDataBinary(a3_GeometryData *geom_out, const a3_FileStream *fileStream)
{
	FILE *fp = 0;
	a3ui32 ret = 0;
	a3ui32 vertexDataSize = 0, indexDataSize = 0, dataSize = 0;
	a3ui32 i = 0;
	a3i32 offset[a3attrib_geomNameMax + 1] = { 0 };
	if (geom_out && fileStream)
	{
		if (!geom_out->data)
		{
			fp = fileStream->stream;
			if (fp)
			{
				// read vertex format and size
				ret += (a3ui32)fread(geom_out->vertexFormat,
					1, sizeof(a3_VertexFormatDescriptor), fp);
				ret += (a3ui32)fread(geom_out->indexFormat,
					1, sizeof(a3_IndexFormatDescriptor), fp);

				// read counts and flags
				ret += (a3ui32)fread(&geom_out->primType,
					1, sizeof(a3_VertexPrimitiveType), fp);
				ret += (a3ui32)fread(&geom_out->numVertices,
					1, sizeof(a3ui32), fp);
				ret += (a3ui32)fread(&geom_out->numIndices,
					1, sizeof(a3ui32), fp);

				// read raw data
				vertexDataSize = a3vertexFormatGetStorageSpaceRequired(geom_out->vertexFormat, geom_out->numVertices);
				indexDataSize = a3indexFormatGetStorageSpaceRequired(geom_out->indexFormat, geom_out->numIndices);
				dataSize = (vertexDataSize + indexDataSize);
				geom_out->data = malloc(dataSize);
				ret += (a3ui32)fread(geom_out->data, 1, dataSize, fp);

				// read attribute offsets for raw data and 
				//	set proper pointers
				ret += (a3ui32)fread(offset, 1, sizeof(a3i32) * (a3attrib_geomNameMax + 1), fp);
				for (i = 0; i < a3attrib_geomNameMax; ++i)
					geom_out->attribData[i] = (offset[i] >= 0) ? ((a3byte *)(geom_out->data) + offset[i]) : 0;
				geom_out->indexData = (offset[i] >= 0) ? ((a3byte *)(geom_out->data) + offset[i]) : 0;
			}
			return ret;
		}
	}
	return -1;
}


a3ret a3geometryCopyDataToString(const a3_GeometryData *geom, a3byte *str)
{
	const a3byte *const start = str;
	a3ui32 vertexDataSize = 0, indexDataSize = 0, dataSize = 0;
	a3ui32 i = 0;
	a3i32 offset[a3attrib_geomNameMax + 1] = { 0 };
	if (geom && str)
	{
		if (geom->data)
		{
			// write vertex format and size
			str = (a3byte *)((a3_VertexFormatDescriptor *)memcpy(str, geom->vertexFormat, sizeof(geom->vertexFormat)) + 1);
			str = (a3byte *)((a3_IndexFormatDescriptor *)memcpy(str, geom->indexFormat, sizeof(geom->indexFormat)) + 1);

			// write counts and flags
			str = (a3byte *)((a3_VertexPrimitiveType *)memcpy(str, &geom->primType, sizeof(geom->primType)) + 1);
			str = (a3byte *)((a3ui32 *)memcpy(str, &geom->numVertices, sizeof(geom->numVertices)) + 1);
			str = (a3byte *)((a3ui32 *)memcpy(str, &geom->numIndices, sizeof(geom->numIndices)) + 1);

			// write raw data
			vertexDataSize = a3vertexFormatGetStorageSpaceRequired(geom->vertexFormat, geom->numVertices);
			indexDataSize = a3indexFormatGetStorageSpaceRequired(geom->indexFormat, geom->numIndices);
			dataSize = (vertexDataSize + indexDataSize);
			str = (a3byte *)memcpy(str, geom->data, dataSize) + dataSize;

			// write attribute offsets for raw data
			// also write index offset
			for (i = 0; i < a3attrib_geomNameMax; ++i)
				offset[i] = geom->attribData[i] ? (a3i32)((a3byte *)(geom->attribData[i]) - (a3byte *)(geom->data)) : -1;
			offset[i] = geom->indexData ? (a3i32)((a3byte *)(geom->indexData) - (a3byte *)(geom->data)) : -1;
			str = (a3byte *)((a3i32 *)memcpy(str, offset, sizeof(offset)) + a3attrib_geomNameMax + 1);

			// done
			return (a3i32)(str - start);
		}
	}
	return -1;
}

a3ret a3geometryCopyStringToData(a3_GeometryData *geom_out, const a3byte *str)
{
	const a3byte *const start = str;
	a3ui32 vertexDataSize = 0, indexDataSize = 0, dataSize = 0;
	a3ui32 i = 0;
	a3i32 offset[a3attrib_geomNameMax + 1] = { 0 };
	if (geom_out && str)
	{
		if (!geom_out->data)
		{
			// read vertex format and size
			memcpy(geom_out->vertexFormat, str, sizeof(geom_out->vertexFormat));
			str += sizeof(geom_out->vertexFormat);
			memcpy(geom_out->indexFormat, str, sizeof(geom_out->indexFormat));
			str += sizeof(geom_out->indexFormat);

			// read counts and flags
			memcpy(&geom_out->primType, str, sizeof(geom_out->primType));
			str += sizeof(geom_out->primType);
			memcpy(&geom_out->numVertices, str, sizeof(geom_out->numVertices));
			str += sizeof(geom_out->numVertices);
			memcpy(&geom_out->numIndices, str, sizeof(geom_out->numIndices));
			str += sizeof(geom_out->numIndices);

			// read raw data
			vertexDataSize = a3vertexFormatGetStorageSpaceRequired(geom_out->vertexFormat, geom_out->numVertices);
			indexDataSize = a3indexFormatGetStorageSpaceRequired(geom_out->indexFormat, geom_out->numIndices);
			dataSize = (vertexDataSize + indexDataSize);
			geom_out->data = malloc(dataSize);
			memcpy(geom_out->data, str, dataSize);
			str += dataSize;

			// read attribute offsets for raw data and 
			//	set proper pointers
			memcpy(offset, str, sizeof(offset));
			str += sizeof(offset);
			for (i = 0; i < a3attrib_geomNameMax; ++i)
				geom_out->attribData[i] = (offset[i] >= 0) ? ((a3byte *)(geom_out->data) + offset[i]) : 0;
			geom_out->indexData = (offset[i] >= 0) ? ((a3byte *)(geom_out->data) + offset[i]) : 0;

			// done
			return (a3i32)(str - start);
		}
	}
	return -1;
}

a3ret a3geometryGetStringSize(const a3_GeometryData *geom)
{
	if (geom && geom->data)
	{
		const a3ui32 size
			= sizeof(geom->vertexFormat)
			+ sizeof(geom->indexFormat)
			+ sizeof(geom->primType)
			+ sizeof(geom->numVertices)
			+ sizeof(geom->numIndices)
			+ sizeof(a3i32) * (a3attrib_geomNameMax + 1)
			+ a3vertexFormatGetStorageSpaceRequired(geom->vertexFormat, geom->numVertices)
			+ a3indexFormatGetStorageSpaceRequired(geom->indexFormat, geom->numIndices);
		return size;
	}
	return -1;
}


a3ret a3geometryReleaseData(a3_GeometryData *geom)
{
	static const a3_GeometryData reset = { 0 };
	if (geom && geom->data)
	{
		free(geom->data);
		*geom = reset;
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------
// some math utilities hidden from the world

// calculate un-normalized face normal
extern inline void a3proceduralInternalCalculateNormal(a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2)
{
	// deltas
	const a3f32 dp1[3] = {
		p1[0] - p0[0],
		p1[1] - p0[1],
		p1[2] - p0[2],
	}, dp2[3] = {
		p2[0] - p0[0],
		p2[1] - p0[1],
		p2[2] - p0[2],
	};

	// normal: cross product of deltas
	a3real3Cross(normal_out, dp1, dp2);
}

// calculate un-normalized tangent basis for a triangle
extern inline void a3proceduralInternalCalculateTangentBasis(a3f32 *tangent_out, a3f32 *bitangent_out, a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2, const a3f32 *t0, const a3f32 *t1, const a3f32 *t2)
{
	// deltas
	const a3f32 dp1[3] = {
		p1[0] - p0[0],
		p1[1] - p0[1],
		p1[2] - p0[2],
	}, dp2[3] = {
		p2[0] - p0[0],
		p2[1] - p0[1],
		p2[2] - p0[2],
	}, dt1[2] = {
		t1[0] - t0[0],
		t1[1] - t0[1],
	}, dt2[2] = {
		t2[0] - t0[0],
		t2[1] - t0[1],
	};

	// inverse determinant of texture coordinate matrix
	const a3f32 detInv = 1.0f / (dt1[0] * dt2[1] - dt1[1] * dt2[0]);

	// expanded matrix multiplication
	tangent_out[0] = (dp1[0] * dt2[1] - dp2[0] * dt1[1]) * detInv;
	tangent_out[1] = (dp1[1] * dt2[1] - dp2[1] * dt1[1]) * detInv;
	tangent_out[2] = (dp1[2] * dt2[1] - dp2[2] * dt1[1]) * detInv;

	bitangent_out[0] = (dp2[0] * dt1[0] - dp1[0] * dt2[0]) * detInv;
	bitangent_out[1] = (dp2[1] * dt1[0] - dp1[1] * dt2[0]) * detInv;
	bitangent_out[2] = (dp2[2] * dt1[0] - dp1[2] * dt2[0]) * detInv;

	// normal: cross product of deltas
	a3real3Cross(normal_out, dp1, dp2);
}

// orthogonal tangent basis using Gram-Schmidt process
extern inline void a3proceduralInternalCalculateTangentBasisOrtho(a3f32 *tangent_out, a3f32 *bitangent_out, a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2, const a3f32 *t0, const a3f32 *t1, const a3f32 *t2)
{
	a3proceduralInternalCalculateTangentBasis(tangent_out, bitangent_out, normal_out, p0, p1, p2, t0, t1, t2);
	a3real3GramSchmidt2(tangent_out, bitangent_out, normal_out);
}


// general index store
extern inline a3ubyte *a3proceduralInternalStoreIndex(a3ubyte *index, const a3ui32 indexSize, const a3ui32 i)
{
	if (indexSize < 2)
		*(index++) = (a3ubyte)i;
	else if (indexSize < 4)
		*(((a3ui16 *)index)++) = (a3ui16)i;
	else
		*(((a3ui32 *)index)++) = i;
	return index;
}


//-----------------------------------------------------------------------------
// CUT FROM HEADER

/*
// A3: Use this to release any self-contained drawable generated using the 
//		above function; this alternative prevents memory leak since the 
//		components of the drawable must be retained in memory as long as 
//		it exists. Consider storing quick drawables separately from others.
//	param drawable: non-null pointer to initialized drawable to release
//	return: new reference count of vertex array if success
//	return: 0 if permanently released; occurs when vertex array is released
//	return: -1 if invalid param or not initialized
a3ret a3geometryReleaseDrawableSelfContained(a3_VertexDrawable *drawable);

a3ret a3geometryReleaseDrawableSelfContained(a3_VertexDrawable *drawable)
{
	if (drawable && drawable->vertexArray)
	{
		// get pointers to be freed
		a3_BufferObject *vbo_ibo = drawable->indexBuffer;
		a3_VertexArrayDescriptor *vao = drawable->vertexArray;

		// release drawable and free objects
		a3i32 ret = a3vertexDrawableRelease(drawable);
		free(vbo_ibo);
		free(vao);
		return ret;
	}
	return -1;
}
*/

//-----------------------------------------------------------------------------
