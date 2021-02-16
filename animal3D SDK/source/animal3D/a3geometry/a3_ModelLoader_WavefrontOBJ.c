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
	
	a3_ModelLoader_WavefrontOBJ.c
	Definitions for OBJ model loader.

	**DO NOT MODIFY THIS FILE**
*/

#include "animal3D/a3geometry/a3_ModelLoader_WavefrontOBJ.h"

#include "animal3D-A3DM/a3math/a3sqrt.h"
#include "animal3D-A3DM/a3math/a3vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef NDEBUG
#include <assert.h>
#else	// NDEBUG
#define assert( expr )	( (void)0 )
#endif	// !NDEBUG


// type of influence in case it needs to change quickly
// i.e. integer vertex attributes decide not to work
#define INFL	a3i32


//-----------------------------------------------------------------------------
// external math and storage utilities

inline void a3proceduralInternalCalculateNormal(a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2);
inline void a3proceduralInternalCalculateTangentBasis(a3f32 *tangent_out, a3f32 *bitangent_out, a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2, const a3f32 *t0, const a3f32 *t1, const a3f32 *t2);
inline void a3proceduralInternalCalculateTangentBasisOrtho(a3f32 *tangent_out, a3f32 *bitangent_out, a3f32 *normal_out, const a3f32 *p0, const a3f32 *p1, const a3f32 *p2, const a3f32 *t0, const a3f32 *t1, const a3f32 *t2);

inline a3ubyte *a3proceduralInternalStoreIndex(a3ubyte *index, const a3ui32 indexSize, const a3ui32 i);


//-----------------------------------------------------------------------------

// internal structure to hold data pointers
typedef struct a3_ModelLoadDataOBJ	a3_ModelLoadDataOBJ;
typedef struct a3_ModelLoadDataSkin	a3_ModelLoadDataSkin;
struct a3_ModelLoadDataOBJ
{
	void *data;
	const a3f32 *positions;
	const a3f32 *texcoords;
	const a3f32 *normals;
	const a3i32 *faces;
	a3ui32 numPositions;
	a3ui32 numTexcoords;
	a3ui32 numNormals;
	a3ui32 numFaces;
	a3i32 faceMode;
};

struct a3_ModelLoadDataSkin
{
	void *data;
	const a3f32 *positions;
	const a3f32 *skinWeights;
	const INFL *skinInfluences;
	const a3ubyte *influenceCounts;
	a3ui32 numVertices;
};


// load flags
enum a3_ModelAttributeLoadedFlag
{
	a3model_texcoordLoaded = 2,
	a3model_normalLoaded = 4,
};

// internal flags
enum a3_ModelAttributeInternalFlag
{
	a3model_internalFaceTangentFlag = 8,
	a3model_internalVertexNormalFlag = 16,
	a3model_internalVertexTangentFlag = 32,
};


//-----------------------------------------------------------------------------
// internal parsing utilities

inline a3ret a3modelInternalParseFaceLine(const a3byte *str, a3i32 *face, const a3ui32 vertexComponents)
{
	// return number of components written
	a3i32 *start = face;
	a3byte *endStr = 0, **endStrPtr = &endStr;
	a3byte c = *str;
	a3ui32 vertexCount = 0, i;
	a3i32 index = -1;
	a3i32 ret = 1;

	// while string is not finished
	while (index && vertexCount < 4)
	{
		// grab indices for current vertex
		for (i = 0; index && i < vertexComponents; ++i, str = endStr + 1)
		{
			// forward to next digit if we must
			while ((c = *str) && !isdigit(c))
				++str;

			// read next index
			// subtract 1 at the same time because OBJ adds 1 to every index
			*(face++) = (index = strtol(str, endStrPtr, 0)) - 1;
		}
		if (index)
			++vertexCount;
	}

	// if a quad was found, triangulate
	// two triangles from order 0,1,2,3 is 0,1,2,3,0,2
	// do not subtract 1 here; already been done
	if (vertexCount == 4)
	{
		for (i = 0; i < vertexComponents; ++i)
			*(face++) = (index = *(start + i));
		for (start += vertexComponents + vertexComponents, 
			i = 0; i < vertexComponents; ++i)
			*(face++) = (index = *(start + i));
		vertexCount += 2;
		ret += 1;
	}

	// done, return triangles stored
	return ret;
}


inline a3ret a3modelInternalParseSkinVertex(const a3byte *str, a3f32 *vertex)
{
	// look for value keyword, grab 3 floats after that
	static const a3byte valueKeyword[] = "value";
	static const a3ui32 valueKeywordSize = sizeof(valueKeyword) - 1;

	while (memcmp(str++, valueKeyword, valueKeywordSize));
	str += valueKeywordSize;
	while (*(str++) != '\"');
	return sscanf(str, "%f %f %f", vertex + 0, vertex + 1, vertex + 2);
}

inline a3ret a3modelInternalParseSkinWeight(const a3byte *str, a3ui32 *index_out, a3f32 *value_out)
{
	static const a3byte indexKeyword[] = "index";
	static const a3byte valueKeyword[] = "value";
	static const a3ui32 indexKeywordSize = sizeof(indexKeyword) - 1;
	static const a3ui32 valueKeywordSize = sizeof(valueKeyword) - 1;

	while (memcmp(str++, indexKeyword, indexKeywordSize));
	str += indexKeywordSize;
	while (*(str++) != '\"');
	sscanf(str, "%u", index_out);

	while (memcmp(str++, valueKeyword, valueKeywordSize));
	str += valueKeywordSize;
	while (*(str++) != '\"');
	return sscanf(str, "%f", value_out);
}


// get index from a list of strings
inline a3ret a3modelInternalGetInfluenceIndex(const a3byte *name, const a3byte *nameList[], const a3ui32 numNames)
{
	a3ui32 i;
	for (i = 0; i < numNames; ++i)
		if (!strcmp(name, nameList[i]))
			return i;
	return -1;
}


//-----------------------------------------------------------------------------

// load OBJ
a3ret a3modelInternalLoadOBJ(a3_ModelLoadDataOBJ *obj, FILE *fp, const a3_ModelLoaderFlag flags, const a3f32 *transform_opt)
{
	// 1. load file and count contents: 
	//	- vertices
	//	- texture coordinates
	//	- normals
	//	- faces
	// 2. allocate space to store raw data
	// 3. read data

	const a3ui32 bufferSize = 256;
	a3byte line[256], *linePtr;
	a3byte c;
	void *dataEnd, *dataEndConfirm;
	a3f32 *positionPtr, *positionPtr2;
	a3f32 *texcoordPtr;
	a3f32 *normalPtr, *normalPtr2;
	a3i32 *facePtr;
	a3ui32 dataSize = 0;
	a3ui32 numLines = 0;
	a3ui32 numPositionElements = 0;
	a3ui32 numTexcoordElements = 0;
	a3ui32 numNormalElements = 0;
	a3ui32 numFaceElements = 0;
	const a3i32 loadTexcoords = flags & a3model_loadTexcoords;
	const a3i32 loadNormals = flags & a3model_loadNormals;
	const a3ui32 positionComponents = 3;
	const a3ui32 texcoordComponents = loadTexcoords ? 2 : 0;
	const a3ui32 normalComponents = loadNormals ? 3 : 0;
	a3ui32 vertexComponents = 0;
	a3ui32 faceComponents = 0;
	a3ui32 triCount = 0;
	a3ui32 i = 0;


	// read file
	linePtr = fgets(line, bufferSize, fp);
	while (!feof(fp))
	{
		// check first character in line
		c = *(linePtr++);
		if (c == 'v')
		{
			// check if position, texcoord or normal
			c = *(linePtr++);
			if (isspace(c))
			{
				++obj->numPositions;
				++numLines;
			}
			else if (c == 't' && isspace(*linePtr))
			{
				++obj->numTexcoords;
				++numLines;
			}
			else if (c == 'n' && isspace(*linePtr))
			{
				++obj->numNormals;
				++numLines;
			}
		}
		// check if face
		else if (c == 'f' && isspace(*linePtr))
		{
			++obj->numFaces;
			++numLines;
		}

		// get next line
		linePtr = fgets(line, bufferSize, fp);
	}


	// check if valid load
	if (obj->numPositions && obj->numFaces)
	{
		// determine face format
		// bit 0 (+1) is positions (on)
		// bit 1 (+2) is texcoords
		// bit 2 (+4) is normals
		obj->faceMode = 1;
		obj->faceMode += (obj->numTexcoords ? a3model_texcoordLoaded : 0);
		obj->faceMode += (obj->numNormals ? a3model_normalLoaded : 0);

		// determine the number of indices in the faces
		// add extra room in case there are quads to be split
		// 3 components is a tri, 4 appear if quad, split becomes 6
		// multiply by the number of active attributes
		vertexComponents = 1;
		vertexComponents += (obj->numTexcoords ? 1 : 0);
		vertexComponents += (obj->numNormals ? 1 : 0);
		faceComponents = 6 * vertexComponents;

		// allocate space for data
		dataSize += (numPositionElements = positionComponents * obj->numPositions) * sizeof(a3f32);
		dataSize += (numTexcoordElements = texcoordComponents * obj->numTexcoords) * sizeof(a3f32);
		dataSize += (numNormalElements = normalComponents * obj->numNormals) * sizeof(a3f32);
		dataSize += (numFaceElements = faceComponents * obj->numFaces) * sizeof(a3i32);
		obj->data = malloc(dataSize);
		obj->positions = positionPtr = positionPtr2 = (a3f32 *)obj->data;
		obj->texcoords = texcoordPtr = (positionPtr + numPositionElements);
		obj->normals = normalPtr = normalPtr2 = (texcoordPtr + numTexcoordElements);
		obj->faces = facePtr = (a3i32 *)(normalPtr + numNormalElements);
		dataEnd = (facePtr + numFaceElements);
		dataEndConfirm = (a3byte *)obj->data + dataSize;
		memset(obj->data, 0, dataSize);


		// faces should be re-counted as tris only
		obj->numFaces = 0;

		
		// warnings if requested attributes are not in the file
		if (loadTexcoords && !obj->numTexcoords)
			printf("\n A3 Warning: OBJ load texcoord flag specified but file does not contain texcoords.");
		if (loadNormals && !obj->numNormals)
			printf("\n A3 Warning: OBJ load normal flag specified but file does not contain normals.");


		// rewind file and actually read values
		rewind(fp);
		linePtr = fgets(line, bufferSize, fp);
		while (!feof(fp))
		{
			c = *(linePtr++);
			if (c == 'v')
			{
				c = *(linePtr++);
				if (isspace(c))
					positionPtr += sscanf(linePtr, "%f %f %f", positionPtr + 0, positionPtr + 1, positionPtr + 2);
				else if (c == 't' && isspace(*linePtr))
				{
					if (loadTexcoords)
						texcoordPtr += sscanf(linePtr, "%f %f", texcoordPtr + 0, texcoordPtr + 1);
				}
				else if (c == 'n' && isspace(*linePtr))
				{
					if (loadNormals)
						normalPtr += sscanf(linePtr, "%f %f %f", normalPtr + 0, normalPtr + 1, normalPtr + 2);
				}
			}
			// check if face
			else if (c == 'f' && isspace(*linePtr))
			{
				// check how many triangles were generated, offset 
				//	by that many indices worth of data: 
				//	triangles x vertices per triangle (3) x components per vertex
				triCount = a3modelInternalParseFaceLine(linePtr, facePtr, vertexComponents);
				facePtr += (triCount * 3 * vertexComponents);
				obj->numFaces += triCount;
			}

			// next line
			linePtr = fgets(line, bufferSize, fp);
		}


		// transform vertices
		if (transform_opt)
		{
			a3f32 tmpVec[3] = { 0 };

			for (i = 0; i < obj->numPositions; ++i, positionPtr2 += positionComponents)
			{
				tmpVec[0] = transform_opt[0] * positionPtr2[0] + transform_opt[4] * positionPtr2[1] + transform_opt[8] * positionPtr2[2] + transform_opt[12];
				tmpVec[1] = transform_opt[1] * positionPtr2[0] + transform_opt[5] * positionPtr2[1] + transform_opt[9] * positionPtr2[2] + transform_opt[13];
				tmpVec[2] = transform_opt[2] * positionPtr2[0] + transform_opt[6] * positionPtr2[1] + transform_opt[10] * positionPtr2[2] + transform_opt[14];
				memcpy(positionPtr2, tmpVec, sizeof(tmpVec));
			}
			assert(positionPtr2 == obj->texcoords);

			// if using normals, transform by normal matrix: inverse transpose of transform
			if (loadNormals)
			{
				const a3f32 invScale0 = a3real3LengthSquaredInverse(transform_opt + 0);
				const a3f32 invScale1 = a3real3LengthSquaredInverse(transform_opt + 4);
				const a3f32 invScale2 = a3real3LengthSquaredInverse(transform_opt + 8);
				a3f32 transform_nrm[12] = { 0 };

				a3real3ProductS(transform_nrm + 0, transform_opt + 0, invScale0);
				a3real3ProductS(transform_nrm + 4, transform_opt + 4, invScale1);
				a3real3ProductS(transform_nrm + 8, transform_opt + 8, invScale1);
		
				for (i = 0; i < obj->numNormals; ++i, normalPtr2 += normalComponents)
				{
					tmpVec[0] = transform_nrm[0] * normalPtr2[0] + transform_nrm[4] * normalPtr2[1] + transform_nrm[8] * normalPtr2[2];
					tmpVec[1] = transform_nrm[1] * normalPtr2[0] + transform_nrm[5] * normalPtr2[1] + transform_nrm[9] * normalPtr2[2];
					tmpVec[2] = transform_nrm[2] * normalPtr2[0] + transform_nrm[6] * normalPtr2[1] + transform_nrm[10] * normalPtr2[2];
					memcpy(normalPtr2, tmpVec, sizeof(tmpVec));
				}
				assert((a3i32 *)normalPtr2 == obj->faces);
			}
		}


		// verify pointers ended up where they're supposed to
		assert(positionPtr == obj->texcoords);
		assert(texcoordPtr == obj->normals);
		assert((a3i32 *)normalPtr == obj->faces);
		assert((void *)facePtr <= dataEnd);


		// done
		return 1;
	}
	else
		printf("\n A3 ERROR: OBJ file does not contain vertex positions and faces.");
	
	// fail
	return 0;
}


// load skin
a3ret a3modelInternalLoadSkin(a3_ModelLoadDataSkin *skin, const a3byte *influenceNames[], const a3ui32 numInfluences, FILE *fp)
{
	// 1. get vertex count from header
	// 2. allocate space for all attributes based on this number
	// 3. copy vertices
	// 4. locate and copy influence data until the end of the file

	const a3ui32 bufferSize = 512;
	a3byte line[512], *linePtr;
	a3byte c;
	void *dataEnd, *dataEndConfirm;
	a3f32 *positionPtr;
	a3f32 *weightPtr, *currentWeightPtr;
	INFL *influencePtr, *currentInfluencePtr;
	a3ubyte *countPtr, *currentCountPtr;
	const a3f32 *maxWeightAddr;
	const a3i32 *maxInfluenceAddr;
	const a3ubyte *maxCountAddr;
	a3ui32 dataSize = 0;
	a3ui32 numLines = 0;
	a3ui32 numPositionElements = 0;
	a3ui32 numWeightElements = 0;
	a3ui32 numInfluenceElements = 0;
	a3ui32 numCountElements = 0;
	const a3ui32 positionComponents = 0;	// 3;	// can exclude since positions are loaded with OBJ
	const a3ui32 weightComponents = 4;
	const a3ui32 influenceComponents = 4;
	const a3ui32 countComponents = 1;

	const a3byte shapeKeyword[] = "shape";
	const a3byte sizeKeyword[] = "size";
	const a3byte pointKeyword[] = "point";
	const a3byte weightsKeyword[] = "weights";
	const a3byte sourceKeyword[] = "source";
	const a3ui32 shapeKeywordLen = sizeof(shapeKeyword) - 1;
	const a3ui32 sizeKeywordLen = sizeof(sizeKeyword) - 1;
	const a3ui32 pointKeywordLen = sizeof(pointKeyword) - 1;
	const a3ui32 weightsKeywordLen = sizeof(weightsKeyword) - 1;
	const a3ui32 sourceKeywordLen = sizeof(sourceKeyword) - 1;

	a3byte jointName[64], *jointNamePtr;

	a3ui32 i, weightCount, vertexIndex, *vertexIndexPtr = &vertexIndex;
	a3ubyte vertexInfluenceCount;
	a3i32 j;
	a3f32 newWeight, *newWeightPtr = &newWeight;


	// read file looking for shape info
	linePtr = fgets(line, bufferSize, fp);
	while (!feof(fp))
	{
		// find beginning of tag
		while ((c = *(linePtr++)) != '<');

		// search for "shape" keyword
		if (!memcmp(linePtr, shapeKeyword, shapeKeywordLen))
		{
			// find the size keyword
			while (memcmp(linePtr++, sizeKeyword, sizeKeywordLen));

			// find the quote
			linePtr += sizeKeywordLen;
			while (!isdigit(*linePtr))
				++linePtr;

			// get count
			skin->numVertices += strtol(linePtr, 0, 0);
			++numLines;
		}

		// next line
		linePtr = fgets(line, bufferSize, fp);
	}


	// check if valid load
	if (skin->numVertices)
	{
		// allocate space
		dataSize += (numPositionElements = positionComponents * skin->numVertices) * sizeof(a3f32);
		dataSize += (numWeightElements = weightComponents * skin->numVertices) * sizeof(a3f32);
		dataSize += (numInfluenceElements = influenceComponents * skin->numVertices) * sizeof(INFL);
		dataSize += (numCountElements = countComponents * skin->numVertices) * sizeof(a3ubyte);

		skin->data = malloc(dataSize);
		skin->positions = positionPtr = (a3f32 *)skin->data;
		skin->skinWeights = maxWeightAddr = weightPtr = (positionPtr + numPositionElements);
		skin->skinInfluences = maxInfluenceAddr = influencePtr = (INFL *)(weightPtr + numWeightElements);
		skin->influenceCounts = maxCountAddr = countPtr = (a3ubyte *)(influencePtr + numInfluenceElements);
		dataEnd = (void *)(countPtr + numCountElements);
		dataEndConfirm = (a3byte *)skin->data + dataSize;
		memset(skin->data, 0, dataSize);

		// set all influences to -1 to start
		for (i = 0; i < numInfluenceElements; ++i)
			influencePtr[i] = (INFL)(-1);


		// rewind and load vertices
	//	rewind(fp);
	//	linePtr = fgets(line, bufferSize, fp);
	//	while (!feof(fp))
	//	{
	//		while ((c = *(linePtr++)) != '<');
	//		if (!memcmp(linePtr, pointKeyword, pointKeywordLen))
	//			positionPtr += a3modelInternalParseSkinVertex(linePtr, positionPtr);
	//		linePtr = fgets(line, bufferSize, fp);
	//	}


		// rewind and load influences
		// 'j' is joint index
		j = 0;
		rewind(fp);
		linePtr = fgets(line, bufferSize, fp);
		while (!feof(fp))
		{
			while ((c = *(linePtr++)) != '<');
			if (!memcmp(linePtr, weightsKeyword, weightsKeywordLen))
			{
				// found weights; need joint index and size
				while (memcmp(linePtr++, sourceKeyword, sourceKeywordLen));
				linePtr += sourceKeywordLen;
				while (*(linePtr++) != '\"');

				// get joint index
				jointNamePtr = jointName;
				while (*linePtr != '\"')
					*(jointNamePtr++) = *(linePtr++);
				*jointNamePtr = 0;
				j = a3modelInternalGetInfluenceIndex(jointName, influenceNames, numInfluences);

				// if the joint is not found, remove character from 
				//	the end of the string and try again
				if (j < 0)
				{
					*(--jointNamePtr) = 0;
					j = a3modelInternalGetInfluenceIndex(jointName, influenceNames, numInfluences);
				}
				
				// get size so we know how many indices we're about to read
				while (memcmp(linePtr++, sizeKeyword, sizeKeywordLen));
				linePtr += sizeKeywordLen;
				while (!isdigit(*linePtr))
					++linePtr;

				// iterate through following weight entries
				for (i = 0, weightCount = strtol(linePtr, 0, 0); i < weightCount; ++i)
				{
					linePtr = fgets(line, bufferSize, fp);
					while ((c = *(linePtr++)) != '<');
					if (!memcmp(linePtr, pointKeyword, pointKeywordLen))
					{
						// read skin influence
						a3modelInternalParseSkinWeight(linePtr, vertexIndexPtr, newWeightPtr);

						// store with the appropriate vertex data if there's room
						currentCountPtr = countPtr + vertexIndex * countComponents;
						if (currentCountPtr > maxCountAddr)
							maxCountAddr = currentCountPtr;

						vertexInfluenceCount = *currentCountPtr;
						if (vertexInfluenceCount < weightComponents)
						{
							// store and increment count for this vertex, move along
							currentWeightPtr = weightPtr + vertexIndex * weightComponents + vertexInfluenceCount;
							currentInfluencePtr = influencePtr + vertexIndex * influenceComponents + vertexInfluenceCount;
							if (currentWeightPtr > maxWeightAddr)
								maxWeightAddr = currentWeightPtr;
							if (currentInfluencePtr > maxInfluenceAddr)
								maxInfluenceAddr = currentInfluencePtr;

							// store weight and ensure index is set to the correct integer
							*currentWeightPtr = newWeight;
							*currentInfluencePtr = (INFL)((a3f32)j + 0.5f);
							++(*currentCountPtr);
						}
					}
				}
			}
			linePtr = fgets(line, bufferSize, fp);
		}


		// verify addresses
		assert(positionPtr == weightPtr);
		assert((INFL *)maxWeightAddr <= influencePtr);
		assert((a3ubyte *)maxInfluenceAddr <= countPtr);
		assert((void *)maxCountAddr <= dataEnd);


		// done
		return 1;
	}
	else
		printf("\n A3 ERROR: XML file does not contain shape data.");

	// fail
	return 0;
}


// release functions
void a3modelInternalReleaseOBJ(a3_ModelLoadDataOBJ *obj)
{
	free(obj->data);
}

void a3modelInternalReleaseSkin(a3_ModelLoadDataSkin *skin)
{
	free(skin->data);
}


//-----------------------------------------------------------------------------

// storage: convert to common data format
a3ret a3modelInternalStore(a3_GeometryData *geom, const a3_ModelLoadDataOBJ *obj, const a3_ModelLoadDataSkin *skin, const a3_ModelLoaderFlag flags)
{
	//-------------------------------------------------------------------------
	// internal helper structs
	typedef struct tagA3VERTEXDESCRIPTOROBJ
	{
		a3i32 v, vt, vn;
	} a3_VertexDescriptorOBJ;
	typedef struct tagA3VERTEXTANGENTBASISOBJ
	{
		a3f32 tangent[3];
		a3f32 bitangent[3];
		a3f32 normal[3];
		a3f32 position[3];
	} a3_VertexTangentBasisOBJ;
	typedef struct tagA3VERTEXDATAOBJ
	{
		a3_VertexDescriptorOBJ vertex;
		a3_VertexTangentBasisOBJ *vertexBasis;
		a3_VertexTangentBasisOBJ faceBasis[1];
		a3f32 texcoord[2];
		a3f32 skinWeights[4];
		INFL skinInfluences[4];
	} a3_VertexDataOBJ;


	//-------------------------------------------------------------------------
	// modes: 
	//	1: position only, each entry is a number
	//	3: position/texcoord:			v/vt v/vt ...
	//	5: position/normal:				v//vn v//vn ...
	//	7: position/texcoord/normal:	v/vt/vn v/vt/vn ...

	a3_GeometryVertexAttributeName attribs[a3attrib_geomNameMax + 2];
	a3ui32 numAttribs = 0;
	a3ui32 vertexDataStorage = 0;
	a3ui32 indexDataStorage = 0;


	// number of indices (faces * 3)
	// also the temporary number of initial vertices while 
	//	we get the unique ones sorted out
	const a3ui32 numIndices = obj->numFaces * 3;
	const a3ui32 tmpIndexStorage = numIndices * sizeof(a3ui32);
	const a3ui32 tmpBasisStorage = obj->numPositions * sizeof(a3_VertexTangentBasisOBJ);
	const a3ui32 tmpVertexStorage = numIndices * sizeof(a3_VertexDataOBJ);
	a3_VertexDataOBJ *const vertexData = (a3_VertexDataOBJ *)malloc(tmpVertexStorage + tmpBasisStorage + tmpIndexStorage);
	a3_VertexTangentBasisOBJ *const basisData = (a3_VertexTangentBasisOBJ *)(vertexData + numIndices);
	a3ui32 *const indexData = (a3ui32 *)(basisData + obj->numPositions);
	void *dataEnd = (void *)(indexData + numIndices);
	void *dataEndConfirm = (a3byte *)vertexData + tmpVertexStorage + tmpBasisStorage + tmpIndexStorage;

	a3_VertexDataOBJ *vertexItr = vertexData, *faceVertexPtr[3];
	a3_VertexTangentBasisOBJ *basisItr = basisData;
	a3ui32 *rawIndexItr = indexData;
	a3ubyte *indexItr;
	a3f32 *positionItr, *texcoordItr, *normalItr, *tangentItr, *bitangentItr, *weightItr, *texcoordStart, *normalStart, *tangentStart, *bitangentStart, *weightStart;
	INFL *influenceItr, *influenceStart;
	const a3f32 *actualNormalPtr, *actualTangentPtr, *actualBitangentPtr;


	//-------------------------------------------------------------------------
	// number of unique vertices
	// search the faces for an index combo that has 
	//	already occurred
	a3ui32 numVerticesUnique = 0;

	// calculate normal and tangent flags
	const a3i32 calcFaceNormals = (flags & a3model_calculateFaceNormals);
	const a3i32 calcFaceTangents = (flags & a3model_internalFaceTangentFlag);
	const a3i32 calcVertNormals = (flags & a3model_internalVertexNormalFlag);
	const a3i32 calcVertTangents = (flags & a3model_internalVertexTangentFlag);
	const a3i32 useTexcoords = (flags & a3model_loadTexcoords) && obj->numTexcoords;
	const a3i32 useNormals = (flags & a3model_loadNormals) && obj->numNormals && !calcFaceNormals;


	// iterators
	const a3ui32 positionComponents = 3;
	const a3ui32 texcoordComponents = useTexcoords ? 2 : 0;
	const a3ui32 normalComponents = useNormals || calcFaceNormals ? 3 : 0;
	const a3ui32 tangentComponents = calcFaceTangents ? 3 : 0;
	const a3ui32 weightComponents = skin ? 4 : 0;
	const a3ui32 influenceComponents = skin ? 4 : 0;
	const a3ui32 positionSize = positionComponents * sizeof(a3f32);
	const a3ui32 texcoordSize = texcoordComponents * sizeof(a3f32);
	const a3ui32 normalSize = normalComponents * sizeof(a3f32);
	const a3ui32 weightSize = weightComponents * sizeof(a3f32);
	const a3ui32 influenceSize = influenceComponents * sizeof(INFL);
	const a3i32 *faceIndexItr;
	a3ui32 i, j, k;
	const a3_VertexDescriptorOBJ vertexReset = { -1, -1, -1 };
	a3_VertexDescriptorOBJ vertex = vertexReset;


	// temporary attribute pointers
	const a3f32 *rawPositionPtr = 0, *rawTexcoordPtr = 0, *rawNormalPtr = 0;
	const a3f32 *rawWeightPtr = 0;
	const INFL *rawInfluencePtr = 0;
	a3f32 weightSumInv = a3real_zero;


	// tangent basis if we need it
	a3f32 faceTangent[3] = { 0 }, faceBitangent[3] = { 0 }, faceNormal[3] = { 0 };


	// first, reset all vertex indices
	memset(vertexData, 0, numIndices * sizeof(a3_VertexDataOBJ));
	for (i = 0, vertexItr = vertexData; i < numIndices; ++i, ++vertexItr)
		vertexItr->vertex.v = vertexItr->vertex.vt = vertexItr->vertex.vn = -1;

	// reset all bases, copy positions
	memset(basisData, 0, obj->numPositions * sizeof(a3_VertexTangentBasisOBJ));
	for (i = 0, basisItr = basisData, rawPositionPtr = obj->positions; i < obj->numPositions; ++i, ++basisItr, rawPositionPtr += positionComponents)
		memcpy(basisItr->position, rawPositionPtr, positionSize);


	//-------------------------------------------------------------------------
	// now analyze the faces
	for (i = 0, faceIndexItr = obj->faces; i < obj->numFaces; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			// reset vertex indices
			vertex = vertexReset;

			// get position index, regardless of what happens next
			// also store a basis address
			vertex.v = *(faceIndexItr++);
			basisItr = basisData + vertex.v;

			// if texture coordinates or normals were loaded, 
			//	decide if we are using them; the pointer needs 
			//	to increment regardless, just do we store it
			if (obj->faceMode & a3model_texcoordLoaded)
			{
				if (useTexcoords)
					vertex.vt = *faceIndexItr;
				++faceIndexItr;
			}

			// for normals, we are exclusively using or 
			//	calculating normals; if calculating face 
			//	normals but not vertex, vertex will be unique
			if (obj->faceMode & a3model_normalLoaded)
			{
				if (useNormals)
					vertex.vn = *faceIndexItr;
				else if (calcFaceNormals && !calcVertNormals)
					vertex.vn = -(a3i32)i;
				++faceIndexItr;
			}
			else if (calcFaceNormals && !calcVertNormals)
				vertex.vn = -(a3i32)i;


			// search for an existing vertex with these indices
			for (k = 0, vertexItr = vertexData; k < numVerticesUnique; ++k)
				if ((vertexItr->vertex.v != vertex.v) ||
					(vertexItr->vertex.vt != vertex.vt) ||
					(vertexItr->vertex.vn != vertex.vn))
					++vertexItr;
				else
					break;

			// if we hit the end of the list, we have a unique vertex
			// prepare new vertex by copying from raw OBJ data
			if (k == numVerticesUnique)
			{
				++numVerticesUnique;
				vertexItr->vertex = vertex;
				vertexItr->vertexBasis = basisItr;

			//	memcpy(vertexItr->faceBasis->position, basisItr->position, positionSize);
				if (vertex.vt >= 0)
				{
					rawTexcoordPtr = (obj->texcoords + texcoordComponents * vertex.vt);
					memcpy(vertexItr->texcoord, rawTexcoordPtr, texcoordSize);
				}
				if (vertex.vn >= 0)
				{
					rawNormalPtr = (obj->normals + normalComponents * vertex.vn);
					memcpy(vertexItr->faceBasis->normal, rawNormalPtr, normalSize);
				}
			}

			// save for later and store index
			faceVertexPtr[j] = vertexItr;
			*(rawIndexItr++) = k;
		}


		// calculate face attributes
		// first check normal
		if (calcFaceNormals)
		{
			// tangent basis
			if (calcFaceTangents)
			{
				// calculate tangent
				a3proceduralInternalCalculateTangentBasisOrtho(faceTangent, faceBitangent, faceNormal,
					faceVertexPtr[0]->vertexBasis->position, faceVertexPtr[1]->vertexBasis->position, faceVertexPtr[2]->vertexBasis->position, 
					faceVertexPtr[0]->texcoord, faceVertexPtr[1]->texcoord, faceVertexPtr[2]->texcoord);

				// same principle as normals
				if (calcVertTangents)
					for (j = 0; j < 3; ++j)
					{
						faceVertexPtr[j]->vertexBasis->tangent[0] += faceTangent[0];
						faceVertexPtr[j]->vertexBasis->tangent[1] += faceTangent[1];
						faceVertexPtr[j]->vertexBasis->tangent[2] += faceTangent[2];
						faceVertexPtr[j]->vertexBasis->bitangent[0] += faceBitangent[0];
						faceVertexPtr[j]->vertexBasis->bitangent[1] += faceBitangent[1];
						faceVertexPtr[j]->vertexBasis->bitangent[2] += faceBitangent[2];
						faceVertexPtr[j]->vertexBasis->normal[0] += faceNormal[0];
						faceVertexPtr[j]->vertexBasis->normal[1] += faceNormal[1];
						faceVertexPtr[j]->vertexBasis->normal[2] += faceNormal[2];
					}
				else
					for (j = 0; j < 3; ++j)
					{
						memcpy(faceVertexPtr[j]->faceBasis->tangent, faceTangent, normalSize);
						memcpy(faceVertexPtr[j]->faceBasis->bitangent, faceBitangent, normalSize);
						memcpy(faceVertexPtr[j]->faceBasis->normal, faceNormal, normalSize);
					}
			}
			else
			{
				// calculate normal using a cross product
				a3proceduralInternalCalculateNormal(faceNormal,
					faceVertexPtr[0]->vertexBasis->position, faceVertexPtr[1]->vertexBasis->position, faceVertexPtr[2]->vertexBasis->position);

				// if vertex normals enabled, accumulate normal
				if (calcVertNormals)
					for (j = 0; j < 3; ++j)
					{
						faceVertexPtr[j]->vertexBasis->normal[0] += faceNormal[0];
						faceVertexPtr[j]->vertexBasis->normal[1] += faceNormal[1];
						faceVertexPtr[j]->vertexBasis->normal[2] += faceNormal[2];
					}
				// otherwise hard-set it for all the verts
				else
					for (j = 0; j < 3; ++j)
						memcpy(faceVertexPtr[j]->faceBasis->normal, faceNormal, normalSize);
			}
		}
	}


	//-------------------------------------------------------------------------
	// enable everything indicated in vertex format
	if (useTexcoords)
		attribs[numAttribs++] = a3attrib_geomTexcoord;
	if (useNormals || calcFaceNormals || calcVertNormals)
		attribs[numAttribs++] = a3attrib_geomNormal;
	if (calcFaceTangents || calcVertTangents)
		attribs[numAttribs++] = a3attrib_geomTangent;
	if (skin)
		attribs[numAttribs++] = a3attrib_geomBlending;

	// create vertex format with attributes and index format descriptor 
	//	with best sized index if needed
	a3geometryCreateVertexFormat(geom->vertexFormat, attribs, numAttribs);
	
	// create index format for unique vertices
	a3geometryCreateIndexFormat(geom->indexFormat, numVerticesUnique);


	// create data store
	vertexDataStorage = a3vertexFormatGetStorageSpaceRequired(geom->vertexFormat, numVerticesUnique);
	indexDataStorage = a3indexFormatGetStorageSpaceRequired(geom->indexFormat, numIndices);

	geom->data = malloc(vertexDataStorage + indexDataStorage);
	geom->attribData[a3attrib_geomPosition] = positionItr = (a3f32 *)geom->data;
	texcoordStart = texcoordItr = positionItr + (positionComponents * numVerticesUnique);
	normalStart = normalItr = texcoordItr + (texcoordComponents * numVerticesUnique);
	tangentStart = tangentItr = normalItr + (normalComponents * numVerticesUnique);
	bitangentStart = bitangentItr = tangentItr + (tangentComponents * numVerticesUnique);
	weightStart = weightItr = bitangentItr + (tangentComponents * numVerticesUnique);
	influenceStart = influenceItr = (INFL *)(weightItr + (weightComponents * numVerticesUnique));
	geom->indexData = indexItr = (a3ubyte *)(influenceItr + (influenceComponents * numVerticesUnique));
	dataEnd = (a3byte *)geom->indexData + geom->indexFormat->indexSize * numIndices;
	dataEndConfirm = (a3byte *)geom->data + (vertexDataStorage + indexDataStorage);


	// copy data from local store to persistent geometry data store
	for (i = 0, vertexItr = vertexData; i < numVerticesUnique; ++i, ++vertexItr)
	{
		// store position
		memcpy(positionItr, vertexItr->vertexBasis->position, positionSize);
		positionItr += positionComponents;

		// store enabled attributes
		if (useTexcoords)
		{
			memcpy(texcoordItr, vertexItr->texcoord, texcoordSize);
			texcoordItr += texcoordComponents;
		}
		if (useNormals || calcFaceNormals)
		{
			// do final normal calculation here
			actualNormalPtr = calcVertNormals ? vertexItr->vertexBasis->normal : vertexItr->faceBasis->normal;
			a3real3GetUnit(normalItr, actualNormalPtr);
			normalItr += normalComponents;

			if (calcFaceTangents)
			{
				actualTangentPtr = calcVertTangents ? vertexItr->vertexBasis->tangent : vertexItr->faceBasis->tangent;
				a3real3GetUnit(tangentItr, actualTangentPtr);
				tangentItr += tangentComponents;

				actualBitangentPtr = calcVertTangents ? vertexItr->vertexBasis->bitangent : vertexItr->faceBasis->bitangent;
				a3real3GetUnit(bitangentItr, actualBitangentPtr);
				bitangentItr += tangentComponents;
			}
		}
		if (skin)
		{
			// for every unique vertex, grab the weights data corresponding 
			//	to its original index (v)
			// also normalize: make sure sum is 1
			rawWeightPtr = (skin->skinWeights + weightComponents * vertexItr->vertex.v);
			rawInfluencePtr = (skin->skinInfluences + influenceComponents * vertexItr->vertex.v);
			weightSumInv = rawWeightPtr[0] + rawWeightPtr[1] + rawWeightPtr[2] + rawWeightPtr[3];
			for (j = 0, weightSumInv = 1.0f / weightSumInv; j < 4; ++j)
				weightItr[j] = rawWeightPtr[j] * weightSumInv;
			memcpy(influenceItr, rawInfluencePtr, influenceSize);
			weightItr += weightComponents;
			influenceItr += influenceComponents;
		}
	}

	// store indices
	for (i = 0, j = geom->indexFormat->indexSize, rawIndexItr = indexData; i < numIndices; ++i, ++rawIndexItr)
		indexItr = a3proceduralInternalStoreIndex(indexItr, j, *rawIndexItr);


	// verify pointers
	assert(positionItr == texcoordStart);
	assert(texcoordItr == normalStart);
	assert(normalItr == tangentStart);
	assert(tangentItr == bitangentStart);
	assert(bitangentItr == weightStart);
	assert((INFL *)weightItr == influenceStart);
	assert(influenceItr == geom->indexData);
	assert((indexItr <= (a3byte *)dataEnd) && (indexItr > ((a3byte *)dataEnd - sizeof(a3i32))));


	// finally, set addresses, counts and primitive type
	geom->attribData[a3attrib_geomTexcoord] = useTexcoords ? texcoordStart : 0;
	geom->attribData[a3attrib_geomNormal] = useNormals || calcFaceNormals || calcVertNormals ? normalStart : 0;
	geom->attribData[a3attrib_geomTangent] = calcFaceTangents || calcVertTangents ? tangentStart : 0;
	geom->attribData[a3attrib_geomBlending] = skin ? weightStart : 0;
	geom->numVertices = numVerticesUnique;
	geom->numIndices = numIndices;
	geom->primType = a3prim_triangles;


	//-------------------------------------------------------------------------
	// done
	free(vertexData);
	return 1;
}


//-----------------------------------------------------------------------------

a3ret a3modelLoadOBJ(a3_GeometryData *geom_out, const a3byte *filePath, const a3_ModelLoaderFlag flags, const a3f32 *transform_opt)
{
	a3_GeometryData ret[1] = { 0 };
	a3_ModelLoadDataOBJ obj[1] = { 0 };
	FILE *fp = 0;
	a3i32 result = 0;

	// validate
	if (geom_out && filePath)
	{
		if (!geom_out->data && *filePath)
		{
			fp = fopen(filePath, "r");
			if (fp)
			{
				// load
				result = a3modelInternalLoadOBJ(obj, fp, flags, transform_opt);
				fclose(fp);

				if (result)
				{
					// convert to geometry data
					result = a3modelInternalStore(ret, obj, 0, flags);
					a3modelInternalReleaseOBJ(obj);
					*geom_out = *ret;
				}
			}
			return result;
		}
	}
	return -1;
}

a3ret a3modelLoadOBJSkinWeights(a3_GeometryData *geom_out, const a3byte *filePath, const a3_ModelLoaderFlag flags, const a3byte *weightsFilePath, const a3byte *influenceNames[], const a3ui32 numInfluences, const a3f32 *transform_opt)
{
	a3_GeometryData ret[1] = { 0 };
	a3_ModelLoadDataOBJ obj[1] = { 0 };
	a3_ModelLoadDataSkin skin[1] = { 0 };
	FILE *fp = 0;
	a3i32 result = 0;

	// validate
	if (geom_out && filePath)
	{
		if (!geom_out->data && *filePath)
		{
			fp = fopen(filePath, "r");
			if (fp)
			{
				// load
				result = a3modelInternalLoadOBJ(obj, fp, flags, transform_opt);
				fclose(fp);

				// load skin
				if (result)
				{
					result = 0;
					if (weightsFilePath && *weightsFilePath && influenceNames && numInfluences)
					{
						fp = fopen(weightsFilePath, "r");
						if (fp)
						{
							result = a3modelInternalLoadSkin(skin, influenceNames, numInfluences, fp);
							fclose(fp);
							if (!result)
								printf("\n A3: Warning: Skin weights file load failed; model was loaded.");
						}
						else
							printf("\n A3 Warning: Invalid skin weights file; model was loaded.");
					}
					else
						printf("\n A3 Warning: Invalid skin weights parameters; model was loaded.");

					// convert to geometry data
					result = a3modelInternalStore(ret, obj, (result ? skin : 0), flags);
					a3modelInternalReleaseOBJ(obj);
					a3modelInternalReleaseSkin(skin);
					*geom_out = *ret;
				}
			}
			return result;
		}
	}
	return -1;
}


//-----------------------------------------------------------------------------
