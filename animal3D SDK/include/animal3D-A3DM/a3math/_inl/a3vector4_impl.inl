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

	a3vector4_impl.inl
	Implementations for 4D vector functions.
*/

#ifdef __ANIMAL3D_A3DM_VECTOR_H
#ifndef __ANIMAL3D_A3DM_VECTOR4_IMPL_INL
#define __ANIMAL3D_A3DM_VECTOR4_IMPL_INL


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// preset vectors
A3_GLOBAL const a3vec4 a3vec4_zero = { a3real_zero, a3real_zero, a3real_zero, a3real_zero };
A3_GLOBAL const a3vec4 a3vec4_one = { a3real_one, a3real_one, a3real_one, a3real_one };
A3_GLOBAL const a3vec4 a3vec4_x = { a3real_one, a3real_zero, a3real_zero, a3real_zero };
A3_GLOBAL const a3vec4 a3vec4_y = { a3real_zero, a3real_one, a3real_zero, a3real_zero };
A3_GLOBAL const a3vec4 a3vec4_z = { a3real_zero, a3real_zero, a3real_one, a3real_zero };
A3_GLOBAL const a3vec4 a3vec4_w = { a3real_zero, a3real_zero, a3real_zero, a3real_one };


//-----------------------------------------------------------------------------

// create 4D vector as array
///
A3_INLINE a3real4r a3real4Set(a3real4p v_out, const a3real x, const a3real y, const a3real z, const a3real w)
{
	v_out[0] = x;
	v_out[1] = y;
	v_out[2] = z;
	v_out[3] = w;
	return v_out;
}

A3_INLINE a3real4r a3real4SetReal2(a3real4p v_out, const a3real2p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = a3real_zero;
	v_out[3] = a3real_zero;
	return v_out;
}

A3_INLINE a3real4r a3real4SetReal2Z(a3real4p v_out, const a3real2p v, const a3real z)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = z;
	v_out[3] = a3real_zero;
	return v_out;
}

A3_INLINE a3real4r a3real4SetReal2ZW(a3real4p v_out, const a3real2p v, const a3real z, const a3real w)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = z;
	v_out[3] = w;
	return v_out;
}

A3_INLINE a3real4r a3real4SetReal3(a3real4p v_out, const a3real3p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = v[2];
	v_out[3] = a3real_zero;
	return v_out;
}

A3_INLINE a3real4r a3real4SetReal3W(a3real4p v_out, const a3real3p v, const a3real w)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = v[2];
	v_out[3] = w;
	return v_out;
}

A3_INLINE a3real4r a3real4SetReal4(a3real4p v_out, const a3real4p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = v[2];
	v_out[3] = v[3];
	return v_out;
}


// unary operations
///
A3_INLINE a3real a3real4LengthSquared(const a3real4p v)
{
	const a3real lenSq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
	return lenSq;
}

A3_INLINE a3real a3real4LengthSquaredInverse(const a3real4p v)
{
	const a3real lenSq = a3real4LengthSquared(v);
	return a3recip(lenSq);
}

A3_INLINE a3real a3real4Length(const a3real4p v)
{
	const a3real lenSq = a3real4LengthSquared(v);
	return a3sqrt(lenSq);
}

A3_INLINE a3real a3real4LengthInverse(const a3real4p v)
{
	const a3real lenSq = a3real4LengthSquared(v);
	return a3sqrtInverse(lenSq);
}

A3_INLINE a3real4r a3real4GetUnit(a3real4p v_out, const a3real4p v)
{
	const a3real lenInv = a3real4LengthInverse(v);
	v_out[0] = v[0] * lenInv;
	v_out[1] = v[1] * lenInv;
	v_out[2] = v[2] * lenInv;
	v_out[3] = v[3] * lenInv;
	return v_out;
}

A3_INLINE a3real4r a3real4Normalize(a3real4p v_inout)
{
	const a3real lenInv = a3real4LengthInverse(v_inout);
	v_inout[0] *= lenInv;
	v_inout[1] *= lenInv;
	v_inout[2] *= lenInv;
	v_inout[3] *= lenInv;
	return v_inout;
}

A3_INLINE a3real4r a3real4GetUnitInvLength(a3real4p v_out, const a3real4p v, a3real *invLength_out)
{
	const a3real lenInv = *invLength_out = a3real4LengthInverse(v);
	v_out[0] = v[0] * lenInv;
	v_out[1] = v[1] * lenInv;
	v_out[2] = v[2] * lenInv;
	v_out[3] = v[3] * lenInv;
	return v_out;
}

A3_INLINE a3real4r a3real4NormalizeGetInvLength(a3real4p v_inout, a3real *invLength_out)
{
	const a3real lenInv = *invLength_out = a3real4LengthInverse(v_inout);
	v_inout[0] *= lenInv;
	v_inout[1] *= lenInv;
	v_inout[2] *= lenInv;
	v_inout[3] *= lenInv;
	return v_inout;
}

A3_INLINE a3real4r a3real4GetNegative(a3real4p v_out, const a3real4p v)
{
	v_out[0] = -v[0];
	v_out[1] = -v[1];
	v_out[2] = -v[2];
	v_out[3] = -v[3];
	return v_out;
}

A3_INLINE a3real4r a3real4Negate(a3real4p v_inout)
{
	v_inout[0] = -v_inout[0];
	v_inout[1] = -v_inout[1];
	v_inout[2] = -v_inout[2];
	v_inout[3] = -v_inout[3];
	return v_inout;
}


// binary operations
///
A3_INLINE a3real a3real4Dot(const a3real4p vL, const a3real4p vR)
{
	const a3real d = vL[0] * vR[0] + vL[1] * vR[1] + vL[2] * vR[2] + vL[3] * vR[3];
	return d;
}

A3_INLINE a3real a3real4DistanceSquared(const a3real4p v0, const a3real4p v1)
{
	a3real4 diff;
	a3real4Diff(diff, v0, v1);
	return a3real4LengthSquared(diff);
}

A3_INLINE a3real a3real4Distance(const a3real4p v0, const a3real4p v1)
{
	const a3real distSq = a3real4DistanceSquared(v0, v1);
	return a3sqrt(distSq);
}

A3_INLINE a3real a3real4ProjRatio(const a3real4p v, const a3real4p vBase)
{
	const a3real d = a3real4Dot(v, vBase);
	const a3real lenSq = a3real4LengthSquared(vBase);
	return (d / lenSq);
}

A3_INLINE a3real4r a3real4Projected(a3real4p v_out, const a3real4p v, const a3real4p vBase)
{
	const a3real ratio = a3real4ProjRatio(v, vBase);
	return a3real4ProductS(v_out, vBase, ratio);
}

A3_INLINE a3real4r a3real4Proj(a3real4p v_inout, const a3real4p vBase)
{
	const a3real ratio = a3real4ProjRatio(v_inout, vBase);
	return a3real4ProductS(v_inout, vBase, ratio);
}

A3_INLINE a3real4r a3real4ProjectedGetRatio(a3real4p v_out, const a3real4p v, const a3real4p vBase, a3real *ratio_out)
{
	const a3real ratio = *ratio_out = a3real4ProjRatio(v, vBase);
	return a3real4ProductS(v_out, vBase, ratio);
}

A3_INLINE a3real4r a3real4ProjGetRatio(a3real4p v_inout, const a3real4p vBase, a3real *ratio_out)
{
	const a3real ratio = *ratio_out = a3real4ProjRatio(v_inout, vBase);
	return a3real4ProductS(v_inout, vBase, ratio);
}

A3_INLINE a3real4r a3real4Sum(a3real4p v_out, const a3real4p vL, const a3real4p vR)
{
	v_out[0] = vL[0] + vR[0];
	v_out[1] = vL[1] + vR[1];
	v_out[2] = vL[2] + vR[2];
	v_out[3] = vL[3] + vR[3];
	return v_out;
}

A3_INLINE a3real4r a3real4Diff(a3real4p v_out, const a3real4p vL, const a3real4p vR)
{
	v_out[0] = vL[0] - vR[0];
	v_out[1] = vL[1] - vR[1];
	v_out[2] = vL[2] - vR[2];
	v_out[3] = vL[3] - vR[3];
	return v_out;
}

A3_INLINE a3real4r a3real4ProductS(a3real4p v_out, const a3real4p v, const a3real s)
{
	v_out[0] = v[0] * s;
	v_out[1] = v[1] * s;
	v_out[2] = v[2] * s;
	v_out[3] = v[3] * s;
	return v_out;
}

A3_INLINE a3real4r a3real4QuotientS(a3real4p v_out, const a3real4p v, const a3real s)
{
	const a3real sInv = a3recip(s);
	v_out[0] = v[0] * sInv;
	v_out[1] = v[1] * sInv;
	v_out[2] = v[2] * sInv;
	v_out[3] = v[3] * sInv;
	return v_out;
}

A3_INLINE a3real4r a3real4ProductComp(a3real4p v_out, const a3real4p vL, const a3real4p vR)
{
	v_out[0] = vL[0] * vR[0];
	v_out[1] = vL[1] * vR[1];
	v_out[2] = vL[2] * vR[2];
	v_out[3] = vL[3] * vR[3];
	return v_out;
}

A3_INLINE a3real4r a3real4QuotientComp(a3real4p v_out, const a3real4p vL, const a3real4p vR)
{
	v_out[0] = vL[0] / vR[0];
	v_out[1] = vL[1] / vR[1];
	v_out[2] = vL[2] / vR[2];
	v_out[3] = vL[3] / vR[3];
	return v_out;
}

A3_INLINE a3real4r a3real4Add(a3real4p vL_inout, const a3real4p vR)
{
	vL_inout[0] += vR[0];
	vL_inout[1] += vR[1];
	vL_inout[2] += vR[2];
	vL_inout[3] += vR[3];
	return vL_inout;
}

A3_INLINE a3real4r a3real4Sub(a3real4p vL_inout, const a3real4p vR)
{
	vL_inout[0] -= vR[0];
	vL_inout[1] -= vR[1];
	vL_inout[2] -= vR[2];
	vL_inout[3] -= vR[3];
	return vL_inout;
}

A3_INLINE a3real4r a3real4MulS(a3real4p v_inout, const a3real s)
{
	v_inout[0] *= s;
	v_inout[1] *= s;
	v_inout[2] *= s;
	v_inout[3] *= s;
	return v_inout;
}

A3_INLINE a3real4r a3real4DivS(a3real4p v_inout, const a3real s)
{
	const a3real sInv = a3recip(s);
	v_inout[0] *= sInv;
	v_inout[1] *= sInv;
	v_inout[2] *= sInv;
	v_inout[3] *= sInv;
	return v_inout;
}

A3_INLINE a3real4r a3real4MulComp(a3real4p vL_inout, const a3real4p vR)
{
	vL_inout[0] *= vR[0];
	vL_inout[1] *= vR[1];
	vL_inout[2] *= vR[2];
	vL_inout[3] *= vR[3];
	return vL_inout;
}

A3_INLINE a3real4r a3real4DivComp(a3real4p vL_inout, const a3real4p vR)
{
	vL_inout[0] /= vR[0];
	vL_inout[1] /= vR[1];
	vL_inout[2] /= vR[2];
	vL_inout[3] /= vR[3];
	return vL_inout;
}


// interpolation algorithms
///
A3_INLINE a3real4r a3real4Lerp(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	v_out[2] = a3lerp(v0[2], v1[2], param);
	v_out[3] = a3lerp(v0[3], v1[3], param);
	return v_out;
}

A3_INLINE a3real4r a3real4NLerp(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	v_out[2] = a3lerp(v0[2], v1[2], param);
	v_out[3] = a3lerp(v0[3], v1[3], param);
	return a3real4Normalize(v_out);
}

A3_INLINE a3real4r a3real4Bilerp(a3real4p v_out, const a3real4p v00, const a3real4p v01, const a3real4p v10, const a3real4p v11, const a3real param0, const a3real param1)
{
	v_out[0] = a3bilerp(v00[0], v01[0], v10[0], v11[0], param0, param1);
	v_out[1] = a3bilerp(v00[1], v01[1], v10[1], v11[1], param0, param1);
	v_out[2] = a3bilerp(v00[2], v01[2], v10[2], v11[2], param0, param1);
	v_out[3] = a3bilerp(v00[3], v01[3], v10[3], v11[3], param0, param1);
	return v_out;
}

A3_INLINE a3real4r a3real4Trilerp(a3real4p v_out, const a3real4p v000, const a3real4p v001, const a3real4p v010, const a3real4p v011, const a3real4p v100, const a3real4p v101, const a3real4p v110, const a3real4p v111, const a3real param0, const a3real param1, const a3real param2)
{
	v_out[0] = a3trilerp(v000[0], v001[0], v010[0], v011[0], v100[0], v101[0], v110[0], v111[0], param0, param1, param2);
	v_out[1] = a3trilerp(v000[1], v001[1], v010[1], v011[1], v100[1], v101[1], v110[1], v111[1], param0, param1, param2);
	v_out[2] = a3trilerp(v000[2], v001[2], v010[2], v011[2], v100[2], v101[2], v110[2], v111[2], param0, param1, param2);
	v_out[3] = a3trilerp(v000[3], v001[3], v010[3], v011[3], v100[3], v101[3], v110[3], v111[3], param0, param1, param2);
	return v_out;
}

A3_INLINE a3real4r a3real4CatmullRom(a3real4p v_out, const a3real4p vPrev, const a3real4p v0, const a3real4p v1, const a3real4p vNext, const a3real param)
{
	v_out[0] = a3CatmullRom(vPrev[0], v0[0], v1[0], vNext[0], param);
	v_out[1] = a3CatmullRom(vPrev[1], v0[1], v1[1], vNext[1], param);
	v_out[2] = a3CatmullRom(vPrev[2], v0[2], v1[2], vNext[2], param);
	v_out[3] = a3CatmullRom(vPrev[3], v0[3], v1[3], vNext[3], param);
	return v_out;
}

A3_INLINE a3real4r a3real4HermiteControl(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real4p vControl0, const a3real4p vControl1, const a3real param)
{
	v_out[0] = a3HermiteControl(v0[0], v1[0], vControl0[0], vControl1[0], param);
	v_out[1] = a3HermiteControl(v0[1], v1[1], vControl0[1], vControl1[1], param);
	v_out[2] = a3HermiteControl(v0[2], v1[2], vControl0[2], vControl1[2], param);
	v_out[3] = a3HermiteControl(v0[3], v1[3], vControl0[3], vControl1[3], param);
	return v_out;
}

A3_INLINE a3real4r a3real4HermiteTangent(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real4p vTangent0, const a3real4p vTangent1, const a3real param)
{
	v_out[0] = a3HermiteTangent(v0[0], v1[0], vTangent0[0], vTangent1[0], param);
	v_out[1] = a3HermiteTangent(v0[1], v1[1], vTangent0[1], vTangent1[1], param);
	v_out[2] = a3HermiteTangent(v0[2], v1[2], vTangent0[2], vTangent1[2], param);
	v_out[3] = a3HermiteTangent(v0[3], v1[3], vTangent0[3], vTangent1[3], param);
	return v_out;
}

A3_INLINE a3real4r a3real4Bezier0(a3real4p v_out, const a3real4p v0, const a3real param)
{
	v_out[0] = v0[0];
	v_out[1] = v0[1];
	v_out[2] = v0[2];
	v_out[3] = v0[3];
	return v_out;
}

A3_INLINE a3real4r a3real4Bezier1(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	v_out[2] = a3lerp(v0[2], v1[2], param);
	v_out[3] = a3lerp(v0[3], v1[3], param);
	return v_out;
}

A3_INLINE a3real4r a3real4Bezier2(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real4p v2, const a3real param)
{
	v_out[0] = a3Bezier2(v0[0], v1[0], v2[0], param);
	v_out[1] = a3Bezier2(v0[1], v1[1], v2[1], param);
	v_out[2] = a3Bezier2(v0[2], v1[2], v2[2], param);
	v_out[3] = a3Bezier2(v0[3], v1[3], v2[3], param);
	return v_out;
}

A3_INLINE a3real4r a3real4Bezier3(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real4p v2, const a3real4p v3, const a3real param)
{
	v_out[0] = a3Bezier3(v0[0], v1[0], v2[0], v3[0], param);
	v_out[1] = a3Bezier3(v0[1], v1[1], v2[1], v3[1], param);
	v_out[2] = a3Bezier3(v0[2], v1[2], v2[2], v3[2], param);
	v_out[3] = a3Bezier3(v0[3], v1[3], v2[3], v3[3], param);
	return v_out;
}

A3_INLINE a3real4r a3real4BezierN(a3real4p v_out, a3count order_N, const a3real4 v[], const a3real param)
{
	a3real4 Bn0, Bn1;

	if (order_N == 0)
		return a3real4SetReal4(v_out, *v);

	if (--order_N == 0)
		return a3real4Lerp(v_out, *(v + 0), *(v + 1), param);
	
	a3real4BezierN(Bn0, order_N, v + 0, param);
	a3real4BezierN(Bn1, order_N, v + 1, param);
	return a3real4Lerp(v_out, Bn0, Bn1, param);
}

A3_INLINE a3real4r a3real4Slerp(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real param)
{
	const a3real lenSq = a3real4LengthSquared(v0) * a3real4LengthSquared(v1);
	const a3real d = a3real4Dot(v0, v1) * a3sqrtInverse(lenSq);
	if (a3isBetweenOnes(d))
	{
		const a3real a = a3acosd(d);
		const a3real s = a3cscd(a);
		const a3real s0 = s * a3sind(a - param*a);
		const a3real s1 = s * a3sind(param*a);
		v_out[0] = v0[0] * s0 + v1[0] * s1;
		v_out[1] = v0[1] * s0 + v1[1] * s1;
		v_out[2] = v0[2] * s0 + v1[2] * s1;
		v_out[3] = v0[3] * s0 + v1[3] * s1;
		return v_out;
	}		
	return a3real4SetReal4(v_out, v0);
}

A3_INLINE a3real4r a3real4SlerpUnit(a3real4p v_out, const a3real4p v0, const a3real4p v1, const a3real param)
{
	const a3real d = a3real4Dot(v0, v1);
	if (a3isBetweenOnes(d))
	{
		const a3real a = a3acosd(d);
		const a3real s = a3cscd(a);
		const a3real s0 = s * a3sind(a - param*a);
		const a3real s1 = s * a3sind(param*a);
		v_out[0] = v0[0] * s0 + v1[0] * s1;
		v_out[1] = v0[1] * s0 + v1[1] * s1;
		v_out[2] = v0[2] * s0 + v1[2] * s1;
		v_out[3] = v0[3] * s0 + v1[3] * s1;
		return v_out;
	}
	return a3real4SetReal4(v_out, v0);
}

A3_INLINE a3real a3real4CalculateArcLengthCatmullRom(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p vPrev, const a3real4p v0, const a3real4p v1, const a3real4p vNext)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real4 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real4SetReal4(vvPrev, v0);
		a3real4SetReal4(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4CatmullRom(vv, vPrev, v0, v1, vNext, t);
			arclen_delta = a3real4Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real4SetReal4(vvPrev, vv);
			a3real4SetReal4(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real4CalculateArcLengthHermiteControl(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p v0, const a3real4p v1, const a3real4p vControl0, const a3real4p vControl1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real4 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real4SetReal4(vvPrev, v0);
		a3real4SetReal4(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4HermiteControl(vv, v0, v1, vControl0, vControl1, t);
			arclen_delta = a3real4Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real4SetReal4(vvPrev, vv);
			a3real4SetReal4(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real4CalculateArcLengthHermiteTangent(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p v0, const a3real4p v1, const a3real4p vTangent0, const a3real4p vTangent1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real4 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real4SetReal4(vvPrev, v0);
		a3real4SetReal4(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4HermiteTangent(vv, v0, v1, vTangent0, vTangent1, t);
			arclen_delta = a3real4Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real4SetReal4(vvPrev, vv);
			a3real4SetReal4(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real4CalculateArcLengthBezier0(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p v0)
{
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = a3real_zero;
		a3real4SetReal4(*sampleTable_out, v0);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		for (i = 1; i <= numDivisions; ++i)
		{
			*(++arclenTable_out) = a3real_zero;
			a3real4SetReal4(*(++sampleTable_out), v0);
			if (paramTable_out)
				*(++paramTable_out) = a3real_zero;
		}
	}
	return a3real_zero;
}

A3_INLINE a3real a3real4CalculateArcLengthBezier1(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p v0, const a3real4p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real4Distance(v0, v1), arclen_delta = arclen / (a3real)numDivisions, arc = a3real_zero, t = a3real_zero, dt;
	a3real4 vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arc;
		a3real4SetReal4(*sampleTable_out, v0);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4Lerp(vv, v0, v1, t);
			arc += arclen_delta;
			*(++arclenTable_out) = autoNormalize ? t : arc;
			a3real4SetReal4(*(++sampleTable_out), vv);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real4CalculateArcLengthBezier2(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p v0, const a3real4p v1, const a3real4p v2)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real4 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real4SetReal4(vvPrev, v0);
		a3real4SetReal4(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4Bezier2(vv, v0, v1, v2, t);
			arclen_delta = a3real4Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real4SetReal4(vvPrev, vv);
			a3real4SetReal4(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real4CalculateArcLengthBezier3(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p v0, const a3real4p v1, const a3real4p v2, const a3real4p v3)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real4 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real4SetReal4(vvPrev, v0);
		a3real4SetReal4(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4Bezier3(vv, v0, v1, v2, v3, t);
			arclen_delta = a3real4Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real4SetReal4(vvPrev, vv);
			a3real4SetReal4(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real4CalculateArcLengthBezierN(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3count order_N, const a3real4 v[])
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real4 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real4SetReal4(vvPrev, *v);
		a3real4SetReal4(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4BezierN(vv, order_N, v, t);
			arclen_delta = a3real4Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real4SetReal4(vvPrev, vv);
			a3real4SetReal4(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real4CalculateArcLengthSlerp(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p v0, const a3real4p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real4 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real4SetReal4(vvPrev, v0);
		a3real4SetReal4(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4Slerp(vv, v0, v1, t);
			arclen_delta = a3real4Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real4SetReal4(vvPrev, vv);
			a3real4SetReal4(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real4CalculateArcLengthSlerpUnit(a3real4 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real4p v0, const a3real4p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real4 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real4SetReal4(vvPrev, v0);
		a3real4SetReal4(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real4SlerpUnit(vv, v0, v1, t);
			arclen_delta = a3real4Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real4SetReal4(vvPrev, vv);
			a3real4SetReal4(*(++sampleTable_out), vvPrev);
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
A3_INLINE a3real4r a3real4GramSchmidtOrtho(a3real4p v_out, const a3real4p v, const a3real4p vBase)
{
	a3real4 proj;
	a3real4Projected(proj, v, vBase);
	return a3real4Diff(v_out, v, proj);
}

A3_INLINE a3real4r a3real4GramSchmidtOrtho2(a3real4p v0_out, a3real4p v1_out, const a3real4p v0, const a3real4p v1, const a3real4p vBase)
{
	a3real4 proj0, proj1;
	a3real4Projected(proj0, v0, vBase);
	a3real4Diff(v0_out, v0, proj0);
	a3real4Projected(proj0, v1, vBase);
	a3real4Projected(proj1, v1, v0_out);
	a3real3Add(proj1, proj0);
	return a3real4Diff(v1_out, v1, proj1);
}

A3_INLINE a3real4r a3real4GramSchmidtOrtho3(a3real4p v0_out, a3real4p v1_out, a3real4p v2_out, const a3real4p v0, const a3real4p v1, const a3real4p v2, const a3real4p vBase)
{
	a3real4 proj0, proj1, proj2;
	a3real4Projected(proj0, v0, vBase);
	a3real4Diff(v0_out, v0, proj0);
	a3real4Projected(proj0, v1, vBase);
	a3real4Projected(proj1, v1, v0_out);
	a3real3Add(proj1, proj0);
	a3real4Diff(v1_out, v1, proj1);
	a3real4Projected(proj0, v2, vBase);
	a3real4Projected(proj1, v2, v0_out);
	a3real4Projected(proj2, v2, v1_out);
	a3real3Add(proj2, proj0);
	a3real3Add(proj2, proj1);
	return a3real4Diff(v2_out, v2, proj2);
}

A3_INLINE a3real4r a3real4GramSchmidt(a3real4p v_inout, const a3real4p vBase)
{
	a3real4 proj;
	a3real4Projected(proj, v_inout, vBase);
	return a3real4Sub(v_inout, proj);
}

A3_INLINE a3real4r a3real4GramSchmidt2(a3real4p v0_inout, a3real4p v1_inout, const a3real4p vBase)
{
	a3real4 proj0, proj1;
	a3real4Projected(proj0, v0_inout, vBase);
	a3real4Sub(v0_inout, proj0);
	a3real4Projected(proj0, v1_inout, vBase);
	a3real4Projected(proj1, v1_inout, v0_inout);
	a3real3Add(proj1, proj0);
	return a3real4Sub(v1_inout, proj1);
}

A3_INLINE a3real4r a3real4GramSchmidt3(a3real4p v0_inout, a3real4p v1_inout, a3real4p v2_inout, const a3real4p vBase)
{
	a3real4 proj0, proj1, proj2;
	a3real4Projected(proj0, v0_inout, vBase);
	a3real4Sub(v0_inout, proj0);
	a3real4Projected(proj0, v1_inout, vBase);
	a3real4Projected(proj1, v1_inout, v0_inout);
	a3real3Add(proj1, proj0);
	a3real4Sub(v1_inout, proj1);
	a3real4Projected(proj0, v2_inout, vBase);
	a3real4Projected(proj1, v2_inout, v0_inout);
	a3real4Projected(proj2, v2_inout, v1_inout);
	a3real3Add(proj2, proj0);
	a3real3Add(proj2, proj1);
	return a3real4Sub(v2_inout, proj2);
}


// cross product not applicable in 4D
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


// triangle normal calculations not applicable in 4D
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


// area of a triangle not applicable in 4D
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


// point contained in triangle not applicable in 4D
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


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_VECTOR4_IMPL_INL
#endif	// __ANIMAL3D_A3DM_VECTOR_H