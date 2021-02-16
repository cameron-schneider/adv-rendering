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
	animal3D Math (A3DM) SDK
	By Daniel S. Buckstein

	a3vector2_impl.inl
	Implementations for 2D vector functions.
*/

#ifdef __ANIMAL3D_A3DM_VECTOR_H
#ifndef __ANIMAL3D_A3DM_VECTOR2_IMPL_INL
#define __ANIMAL3D_A3DM_VECTOR2_IMPL_INL


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// preset vectors
A3_GLOBAL const a3vec2 a3vec2_zero = { a3real_zero, a3real_zero };
A3_GLOBAL const a3vec2 a3vec2_one = { a3real_one, a3real_one };
A3_GLOBAL const a3vec2 a3vec2_x = { a3real_one, a3real_zero };
A3_GLOBAL const a3vec2 a3vec2_y = { a3real_zero, a3real_one };
//
//


//-----------------------------------------------------------------------------

// create 2D vector as array
///
A3_INLINE a3real2r a3real2Set(a3real2p v_out, const a3real x, const a3real y)
{
	v_out[0] = x;
	v_out[1] = y;
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2SetReal2(a3real2p v_out, const a3real2p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	// 
	// 
	return v_out;
}

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

A3_INLINE a3real2r a3real2SetReal3(a3real2p v_out, const a3real3p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	// 
	// 
	return v_out;
}

//
//
//
//
//
//
//
//

A3_INLINE a3real2r a3real2SetReal4(a3real2p v_out, const a3real4p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	// 
	// 
	return v_out;
}


// unary operations
///
A3_INLINE a3real a3real2LengthSquared(const a3real2p v)
{
	const a3real lenSq = v[0] * v[0] + v[1] * v[1];
	return lenSq;
}

A3_INLINE a3real a3real2LengthSquaredInverse(const a3real2p v)
{
	const a3real lenSq = a3real2LengthSquared(v);
	return a3recip(lenSq);
}

A3_INLINE a3real a3real2Length(const a3real2p v)
{
	const a3real lenSq = a3real2LengthSquared(v);
	return a3sqrt(lenSq);
}

A3_INLINE a3real a3real2LengthInverse(const a3real2p v)
{
	const a3real lenSq = a3real2LengthSquared(v);
	return a3sqrtInverse(lenSq);
}

A3_INLINE a3real2r a3real2GetUnit(a3real2p v_out, const a3real2p v)
{
	const a3real lenInv = a3real2LengthInverse(v);
	v_out[0] = v[0] * lenInv;
	v_out[1] = v[1] * lenInv;
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Normalize(a3real2p v_inout)
{
	const a3real lenInv = a3real2LengthInverse(v_inout);
	v_inout[0] *= lenInv;
	v_inout[1] *= lenInv;
	// 
	// 
	return v_inout;
}

A3_INLINE a3real2r a3real2GetUnitInvLength(a3real2p v_out, const a3real2p v, a3real *invLength_out)
{
	const a3real lenInv = *invLength_out = a3real2LengthInverse(v);
	v_out[0] = v[0] * lenInv;
	v_out[1] = v[1] * lenInv;
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2NormalizeGetInvLength(a3real2p v_inout, a3real *invLength_out)
{
	const a3real lenInv = *invLength_out = a3real2LengthInverse(v_inout);
	v_inout[0] *= lenInv;
	v_inout[1] *= lenInv;
	// 
	// 
	return v_inout;
}

A3_INLINE a3real2r a3real2GetNegative(a3real2p v_out, const a3real2p v)
{
	v_out[0] = -v[0];
	v_out[1] = -v[1];
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Negate(a3real2p v_inout)
{
	v_inout[0] = -v_inout[0];
	v_inout[1] = -v_inout[1];
	// 
	// 
	return v_inout;
}


// binary operations
///
A3_INLINE a3real a3real2Dot(const a3real2p vL, const a3real2p vR)
{
	const a3real d = vL[0] * vR[0] + vL[1] * vR[1];
	return d;
}

A3_INLINE a3real a3real2DistanceSquared(const a3real2p v0, const a3real2p v1)
{
	a3real2 diff;
	a3real2Diff(diff, v0, v1);
	return a3real2LengthSquared(diff);
}

A3_INLINE a3real a3real2Distance(const a3real2p v0, const a3real2p v1)
{
	const a3real distSq = a3real2DistanceSquared(v0, v1);
	return a3sqrt(distSq);
}

A3_INLINE a3real a3real2ProjRatio(const a3real2p v, const a3real2p vBase)
{
	const a3real d = a3real2Dot(v, vBase);
	const a3real lenSq = a3real2LengthSquared(vBase);
	return (d / lenSq);
}

A3_INLINE a3real2r a3real2Projected(a3real2p v_out, const a3real2p v, const a3real2p vBase)
{
	const a3real ratio = a3real2ProjRatio(v, vBase);
	return a3real2ProductS(v_out, vBase, ratio);
}

A3_INLINE a3real2r a3real2Proj(a3real2p v_inout, const a3real2p vBase)
{
	const a3real ratio = a3real2ProjRatio(v_inout, vBase);
	return a3real2ProductS(v_inout, vBase, ratio);
}

A3_INLINE a3real2r a3real2ProjectedGetRatio(a3real2p v_out, const a3real2p v, const a3real2p vBase, a3real *ratio_out)
{
	const a3real ratio = *ratio_out = a3real2ProjRatio(v, vBase);
	return a3real2ProductS(v_out, vBase, ratio);
}

A3_INLINE a3real2r a3real2ProjGetRatio(a3real2p v_inout, const a3real2p vBase, a3real *ratio_out)
{
	const a3real ratio = *ratio_out = a3real2ProjRatio(v_inout, vBase);
	return a3real2ProductS(v_inout, vBase, ratio);
}

A3_INLINE a3real2r a3real2Sum(a3real2p v_out, const a3real2p vL, const a3real2p vR)
{
	v_out[0] = vL[0] + vR[0];
	v_out[1] = vL[1] + vR[1];
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Diff(a3real2p v_out, const a3real2p vL, const a3real2p vR)
{
	v_out[0] = vL[0] - vR[0];
	v_out[1] = vL[1] - vR[1];
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2ProductS(a3real2p v_out, const a3real2p v, const a3real s)
{
	v_out[0] = v[0] * s;
	v_out[1] = v[1] * s;
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2QuotientS(a3real2p v_out, const a3real2p v, const a3real s)
{
	const a3real sInv = a3recip(s);
	v_out[0] = v[0] * sInv;
	v_out[1] = v[1] * sInv;
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2ProductComp(a3real2p v_out, const a3real2p vL, const a3real2p vR)
{
	v_out[0] = vL[0] * vR[0];
	v_out[1] = vL[1] * vR[1];
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2QuotientComp(a3real2p v_out, const a3real2p vL, const a3real2p vR)
{
	v_out[0] = vL[0] / vR[0];
	v_out[1] = vL[1] / vR[1];
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Add(a3real2p vL_inout, const a3real2p vR)
{
	vL_inout[0] += vR[0];
	vL_inout[1] += vR[1];
	// 
	// 
	return vL_inout;
}

A3_INLINE a3real2r a3real2Sub(a3real2p vL_inout, const a3real2p vR)
{
	vL_inout[0] -= vR[0];
	vL_inout[1] -= vR[1];
	// 
	// 
	return vL_inout;
}

A3_INLINE a3real2r a3real2MulS(a3real2p v_inout, const a3real s)
{
	v_inout[0] *= s;
	v_inout[1] *= s;
	// 
	// 
	return v_inout;
}

A3_INLINE a3real2r a3real2DivS(a3real2p v_inout, const a3real s)
{
	const a3real sInv = a3recip(s);
	v_inout[0] *= sInv;
	v_inout[1] *= sInv;
	// 
	// 
	return v_inout;
}

A3_INLINE a3real2r a3real2MulComp(a3real2p vL_inout, const a3real2p vR)
{
	vL_inout[0] *= vR[0];
	vL_inout[1] *= vR[1];
	// 
	// 
	return vL_inout;
}

A3_INLINE a3real2r a3real2DivComp(a3real2p vL_inout, const a3real2p vR)
{
	vL_inout[0] /= vR[0];
	vL_inout[1] /= vR[1];
	// 
	// 
	return vL_inout;
}


// interpolation algorithms
///
A3_INLINE a3real2r a3real2Lerp(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2NLerp(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	// 
	// 
	return a3real2Normalize(v_out);
}

A3_INLINE a3real2r a3real2Bilerp(a3real2p v_out, const a3real2p v00, const a3real2p v01, const a3real2p v10, const a3real2p v11, const a3real param0, const a3real param1)
{
	v_out[0] = a3bilerp(v00[0], v01[0], v10[0], v11[0], param0, param1);
	v_out[1] = a3bilerp(v00[1], v01[1], v10[1], v11[1], param0, param1);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Trilerp(a3real2p v_out, const a3real2p v000, const a3real2p v001, const a3real2p v010, const a3real2p v011, const a3real2p v100, const a3real2p v101, const a3real2p v110, const a3real2p v111, const a3real param0, const a3real param1, const a3real param2)
{
	v_out[0] = a3trilerp(v000[0], v001[0], v010[0], v011[0], v100[0], v101[0], v110[0], v111[0], param0, param1, param2);
	v_out[1] = a3trilerp(v000[1], v001[1], v010[1], v011[1], v100[1], v101[1], v110[1], v111[1], param0, param1, param2);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2CatmullRom(a3real2p v_out, const a3real2p vPrev, const a3real2p v0, const a3real2p v1, const a3real2p vNext, const a3real param)
{
	v_out[0] = a3CatmullRom(vPrev[0], v0[0], v1[0], vNext[0], param);
	v_out[1] = a3CatmullRom(vPrev[1], v0[1], v1[1], vNext[1], param);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2HermiteControl(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real2p vControl0, const a3real2p vControl1, const a3real param)
{
	v_out[0] = a3HermiteControl(v0[0], v1[0], vControl0[0], vControl1[0], param);
	v_out[1] = a3HermiteControl(v0[1], v1[1], vControl0[1], vControl1[1], param);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2HermiteTangent(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real2p vTangent0, const a3real2p vTangent1, const a3real param)
{
	v_out[0] = a3HermiteTangent(v0[0], v1[0], vTangent0[0], vTangent1[0], param);
	v_out[1] = a3HermiteTangent(v0[1], v1[1], vTangent0[1], vTangent1[1], param);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Bezier0(a3real2p v_out, const a3real2p v0, const a3real param)
{
	v_out[0] = v0[0];
	v_out[1] = v0[1];
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Bezier1(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Bezier2(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real2p v2, const a3real param)
{
	v_out[0] = a3Bezier2(v0[0], v1[0], v2[0], param);
	v_out[1] = a3Bezier2(v0[1], v1[1], v2[1], param);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Bezier3(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real2p v2, const a3real2p v3, const a3real param)
{
	v_out[0] = a3Bezier3(v0[0], v1[0], v2[0], v3[0], param);
	v_out[1] = a3Bezier3(v0[1], v1[1], v2[1], v3[1], param);
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2BezierN(a3real2p v_out, a3count order_N, const a3real2 v[], const a3real param)
{
	a3real2 Bn0, Bn1;

	if (order_N == 0)
		return a3real2SetReal2(v_out, *v);
	
	if (--order_N == 0)
		return a3real2Lerp(v_out, *(v + 0), *(v + 1), param);
	
	a3real2BezierN(Bn0, order_N, v + 0, param);
	a3real2BezierN(Bn1, order_N, v + 1, param);
	return a3real2Lerp(v_out, Bn0, Bn1, param);
}

A3_INLINE a3real2r a3real2Slerp(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real param)
{
	const a3real lenSq = a3real2LengthSquared(v0) * a3real2LengthSquared(v1);
	const a3real d = a3real2Dot(v0, v1) * a3sqrtInverse(lenSq);
	if (a3isBetweenOnes(d))
	{
		const a3real a = a3acosd(d);
		const a3real s = a3cscd(a);
		const a3real s0 = s * a3sind(a - param*a);
		const a3real s1 = s * a3sind(param*a);
		v_out[0] = v0[0] * s0 + v1[0] * s1;
		v_out[1] = v0[1] * s0 + v1[1] * s1;
		// 
		// 
		return v_out;
	}
	return a3real2SetReal2(v_out, v0);
}

A3_INLINE a3real2r a3real2SlerpUnit(a3real2p v_out, const a3real2p v0, const a3real2p v1, const a3real param)
{
	const a3real d = a3real2Dot(v0, v1);
	if (a3isBetweenOnes(d))
	{
		const a3real a = a3acosd(d);
		const a3real s = a3cscd(a);
		const a3real s0 = s * a3sind(a - param*a);
		const a3real s1 = s * a3sind(param*a);
		v_out[0] = v0[0] * s0 + v1[0] * s1;
		v_out[1] = v0[1] * s0 + v1[1] * s1;
		// 
		// 
		return v_out;
	}
	return a3real2SetReal2(v_out, v0);
}

A3_INLINE a3real a3real2CalculateArcLengthCatmullRom(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p vPrev, const a3real2p v0, const a3real2p v1, const a3real2p vNext)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real2 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real2SetReal2(vvPrev, v0);
		a3real2SetReal2(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2CatmullRom(vv, vPrev, v0, v1, vNext, t);
			arclen_delta = a3real2Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real2SetReal2(vvPrev, vv);
			a3real2SetReal2(*(++sampleTable_out), vvPrev);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real2CalculateArcLengthHermiteControl(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p v0, const a3real2p v1, const a3real2p vControl0, const a3real2p vControl1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real2 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real2SetReal2(vvPrev, v0);
		a3real2SetReal2(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2HermiteControl(vv, v0, v1, vControl0, vControl1, t);
			arclen_delta = a3real2Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real2SetReal2(vvPrev, vv);
			a3real2SetReal2(*(++sampleTable_out), vvPrev);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real2CalculateArcLengthHermiteTangent(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p v0, const a3real2p v1, const a3real2p vTangent0, const a3real2p vTangent1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real2 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real2SetReal2(vvPrev, v0);
		a3real2SetReal2(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2HermiteTangent(vv, v0, v1, vTangent0, vTangent1, t);
			arclen_delta = a3real2Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real2SetReal2(vvPrev, vv);
			a3real2SetReal2(*(++sampleTable_out), vvPrev);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real2CalculateArcLengthBezier0(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p v0)
{
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = a3real_zero;
		a3real2SetReal2(*sampleTable_out, v0);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		for (i = 1; i <= numDivisions; ++i)
		{
			*(++arclenTable_out) = a3real_zero;
			a3real2SetReal2(*(++sampleTable_out), v0);
			if (paramTable_out)
				*(++paramTable_out) = a3real_zero;
		}
	}
	return a3real_zero;
}

A3_INLINE a3real a3real2CalculateArcLengthBezier1(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p v0, const a3real2p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real2Distance(v0, v1), arclen_delta = arclen / (a3real)numDivisions, arc = a3real_zero, t = a3real_zero, dt;
	a3real2 vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arc;
		a3real2SetReal2(*sampleTable_out, v0);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2Lerp(vv, v0, v1, t);
			arc += arclen_delta;
			*(++arclenTable_out) = autoNormalize ? t : arc;
			a3real2SetReal2(*(++sampleTable_out), vv);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real2CalculateArcLengthBezier2(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p v0, const a3real2p v1, const a3real2p v2)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real2 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real2SetReal2(vvPrev, v0);
		a3real2SetReal2(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2Bezier2(vv, v0, v1, v2, t);
			arclen_delta = a3real2Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real2SetReal2(vvPrev, vv);
			a3real2SetReal2(*(++sampleTable_out), vvPrev);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real2CalculateArcLengthBezier3(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p v0, const a3real2p v1, const a3real2p v2, const a3real2p v3)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real2 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real2SetReal2(vvPrev, v0);
		a3real2SetReal2(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2Bezier3(vv, v0, v1, v2, v3, t);
			arclen_delta = a3real2Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real2SetReal2(vvPrev, vv);
			a3real2SetReal2(*(++sampleTable_out), vvPrev);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real2CalculateArcLengthBezierN(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3count order_N, const a3real2 v[])
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real2 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real2SetReal2(vvPrev, *v);
		a3real2SetReal2(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2BezierN(vv, order_N, v, t);
			arclen_delta = a3real2Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real2SetReal2(vvPrev, vv);
			a3real2SetReal2(*(++sampleTable_out), vvPrev);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real2CalculateArcLengthSlerp(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p v0, const a3real2p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real2 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real2SetReal2(vvPrev, v0);
		a3real2SetReal2(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2Slerp(vv, v0, v1, t);
			arclen_delta = a3real2Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real2SetReal2(vvPrev, vv);
			a3real2SetReal2(*(++sampleTable_out), vvPrev);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real2CalculateArcLengthSlerpUnit(a3real2 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real2p v0, const a3real2p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real2 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real2SetReal2(vvPrev, v0);
		a3real2SetReal2(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real2SlerpUnit(vv, v0, v1, t);
			arclen_delta = a3real2Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real2SetReal2(vvPrev, vv);
			a3real2SetReal2(*(++sampleTable_out), vvPrev);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}


// Gram-Schmidt orthonormalization
///
A3_INLINE a3real2r a3real2GramSchmidtOrtho(a3real2p v_out, const a3real2p v, const a3real2p vBase)
{
	a3real2 proj;
	a3real2Projected(proj, v, vBase);
	return a3real2Diff(v_out, v, proj);
}

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

A3_INLINE a3real2r a3real2GramSchmidt(a3real2p v_inout, const a3real2p vBase)
{
	a3real2 proj;
	a3real2Projected(proj, v_inout, vBase);
	return a3real2Sub(v_inout, proj);
}

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//


// 2D cross product: get the signed length of the perpendicular axis (Z)
///
A3_INLINE a3real a3real2CrossZ(const a3real2p vL, const a3real2p vR)
{
	return (vL[0] * vR[1] - vL[1] * vR[0]);
}

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//


// triangle normal calculations not applicable in 2D
// scroll down
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//


// area of a triangle
///
A3_INLINE a3real a3real2TriangleAreaDoubled(const a3real2p v0, const a3real2p v1, const a3real2p v2)
{
	const a3real z = (v1[0] - v0[0])*(v2[1] - v0[1]) - (v1[1] - v0[1])*(v2[0] - v0[0]);
	return z;
}

A3_INLINE a3real a3real2TriangleAreaSquared(const a3real2p v0, const a3real2p v1, const a3real2p v2)
{
	const a3real z = a3real2TriangleAreaDoubled(v0, v1, v2);
	return (z*z*a3real_quarter);
}

A3_INLINE a3real a3real2TriangleArea(const a3real2p v0, const a3real2p v1, const a3real2p v2)
{
	const a3real z = a3real2TriangleAreaDoubled(v0, v1, v2);
	return (z*a3real_half);
}


// point is in triangle (barycentric coordinates)
///
A3_INLINE a3boolean a3real2PointIsInTriangle(const a3real2p p, const a3real2p v0, const a3real2p v1, const a3real2p v2)
{
	a3real2 a, b, q;
	a3real aa, bb, ab, qa, qb, alpha, beta, gamma;
	a3real2Diff(a, v0, v2);
	a3real2Diff(b, v1, v2);
	a3real2Diff(q, p, v2);
	aa = a3real2Dot(a, a);
	bb = a3real2Dot(b, b);
	ab = a3real2Dot(a, b);
	qa = a3real2Dot(q, a);
	qb = a3real2Dot(q, b);
	beta = (aa*qb - ab*qa) / (aa*bb - ab*ab);
	alpha = (qa - beta*ab) / aa;
	gamma = alpha + beta;
	return (alpha >= a3real_zero && alpha <= a3real_one && beta >= a3real_zero && beta <= a3real_one && gamma <= a3real_one);
}

A3_INLINE a3boolean a3real2PointIsInTriangleBarycentric(const a3real2p p, const a3real2p v0, const a3real2p v1, const a3real2p v2, a3real *param0, a3real *param1, a3real *param2)
{
	a3real2 a, b, q;
	a3real aa, bb, ab, qa, qb, alpha, beta, gamma;
	a3real2Diff(a, v0, v2);
	a3real2Diff(b, v1, v2);
	a3real2Diff(q, p, v2);
	aa = a3real2Dot(a, a);
	bb = a3real2Dot(b, b);
	ab = a3real2Dot(a, b);
	qa = a3real2Dot(q, a);
	qb = a3real2Dot(q, b);
	beta = (aa*qb - ab*qa) / (aa*bb - ab*ab);
	alpha = (qa - beta*ab) / aa;
	gamma = alpha + beta;
	*param0 = alpha;
	*param1 = beta;
	*param2 = a3real_one - gamma;
	return (alpha >= a3real_zero && alpha <= a3real_one && beta >= a3real_zero && beta <= a3real_one && gamma <= a3real_one);
}


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_VECTOR2_IMPL_INL
#endif	// __ANIMAL3D_A3DM_VECTOR_H