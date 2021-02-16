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

	a3vector3_impl.inl
	Implementations for 3D vector functions.
*/

#ifdef __ANIMAL3D_A3DM_VECTOR_H
#ifndef __ANIMAL3D_A3DM_VECTOR3_IMPL_INL
#define __ANIMAL3D_A3DM_VECTOR3_IMPL_INL


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// preset vectors
A3_GLOBAL const a3vec3 a3vec3_zero = { a3real_zero, a3real_zero, a3real_zero  A3PAD0 };
A3_GLOBAL const a3vec3 a3vec3_one = { a3real_one, a3real_one, a3real_one  A3PAD0 };
A3_GLOBAL const a3vec3 a3vec3_x = { a3real_one, a3real_zero, a3real_zero  A3PAD0 };
A3_GLOBAL const a3vec3 a3vec3_y = { a3real_zero, a3real_one, a3real_zero  A3PAD0 };
A3_GLOBAL const a3vec3 a3vec3_z = { a3real_zero, a3real_zero, a3real_one  A3PAD0 };
//


//-----------------------------------------------------------------------------

// create 3D vector as array
///
A3_INLINE a3real3r a3real3Set(a3real3p v_out, const a3real x, const a3real y, const a3real z)
{
	v_out[0] = x;
	v_out[1] = y;
	v_out[2] = z;
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3SetReal2(a3real3p v_out, const a3real2p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = a3real_zero;
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3SetReal2Z(a3real3p v_out, const a3real2p v, const a3real z)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = z;
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

A3_INLINE a3real3r a3real3SetReal3(a3real3p v_out, const a3real3p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = v[2];
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

A3_INLINE a3real3r a3real3SetReal4(a3real3p v_out, const a3real4p v)
{
	v_out[0] = v[0];
	v_out[1] = v[1];
	v_out[2] = v[2];
	// 
	return v_out;
}


// unary operations
///
A3_INLINE a3real a3real3LengthSquared(const a3real3p v)
{
	const a3real lenSq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	return lenSq;
}

A3_INLINE a3real a3real3LengthSquaredInverse(const a3real3p v)
{
	const a3real lenSq = a3real3LengthSquared(v);
	return a3recip(lenSq);
}

A3_INLINE a3real a3real3Length(const a3real3p v)
{
	const a3real lenSq = a3real3LengthSquared(v);
	return a3sqrt(lenSq);
}

A3_INLINE a3real a3real3LengthInverse(const a3real3p v)
{
	const a3real lenSq = a3real3LengthSquared(v);
	return a3sqrtInverse(lenSq);
}

A3_INLINE a3real3r a3real3GetUnit(a3real3p v_out, const a3real3p v)
{
	const a3real lenInv = a3real3LengthInverse(v);
	v_out[0] = v[0] * lenInv;
	v_out[1] = v[1] * lenInv;
	v_out[2] = v[2] * lenInv;
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Normalize(a3real3p v_inout)
{
	const a3real lenInv = a3real3LengthInverse(v_inout);
	v_inout[0] *= lenInv;
	v_inout[1] *= lenInv;
	v_inout[2] *= lenInv;
	// 
	return v_inout;
}

A3_INLINE a3real3r a3real3GetUnitInvLength(a3real3p v_out, const a3real3p v, a3real *invLength_out)
{
	const a3real lenInv = *invLength_out = a3real3LengthInverse(v);
	v_out[0] = v[0] * lenInv;
	v_out[1] = v[1] * lenInv;
	v_out[2] = v[2] * lenInv;
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3NormalizeGetInvLength(a3real3p v_inout, a3real *invLength_out)
{
	const a3real lenInv = *invLength_out = a3real3LengthInverse(v_inout);
	v_inout[0] *= lenInv;
	v_inout[1] *= lenInv;
	v_inout[2] *= lenInv;
	// 
	return v_inout;
}

A3_INLINE a3real3r a3real3GetNegative(a3real3p v_out, const a3real3p v)
{
	v_out[0] = -v[0];
	v_out[1] = -v[1];
	v_out[2] = -v[2];
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Negate(a3real3p v_inout)
{
	v_inout[0] = -v_inout[0];
	v_inout[1] = -v_inout[1];
	v_inout[2] = -v_inout[2];
	// 
	return v_inout;
}


// binary operations
///
A3_INLINE a3real a3real3Dot(const a3real3p vL, const a3real3p vR)
{
	const a3real d = vL[0] * vR[0] + vL[1] * vR[1] + vL[2] * vR[2];
	return d;
}

A3_INLINE a3real a3real3DistanceSquared(const a3real3p v0, const a3real3p v1)
{
	a3real3 diff;
	a3real3Diff(diff, v0, v1);
	return a3real3LengthSquared(diff);
}

A3_INLINE a3real a3real3Distance(const a3real3p v0, const a3real3p v1)
{
	const a3real distSq = a3real3DistanceSquared(v0, v1);
	return a3sqrt(distSq);
}

A3_INLINE a3real a3real3ProjRatio(const a3real3p v, const a3real3p vBase)
{
	const a3real d = a3real3Dot(v, vBase);
	const a3real lenSq = a3real3LengthSquared(vBase);
	return (d / lenSq);
}

A3_INLINE a3real3r a3real3Projected(a3real3p v_out, const a3real3p v, const a3real3p vBase)
{
	const a3real ratio = a3real3ProjRatio(v, vBase);
	return a3real3ProductS(v_out, vBase, ratio);
}

A3_INLINE a3real3r a3real3Proj(a3real3p v_inout, const a3real3p vBase)
{
	const a3real ratio = a3real3ProjRatio(v_inout, vBase);
	return a3real3ProductS(v_inout, vBase, ratio);
}

A3_INLINE a3real3r a3real3ProjectedGetRatio(a3real3p v_out, const a3real3p v, const a3real3p vBase, a3real *ratio_out)
{
	const a3real ratio = *ratio_out = a3real3ProjRatio(v, vBase);
	return a3real3ProductS(v_out, vBase, ratio);
}

A3_INLINE a3real3r a3real3ProjGetRatio(a3real3p v_inout, const a3real3p vBase, a3real *ratio_out)
{
	const a3real ratio = *ratio_out = a3real3ProjRatio(v_inout, vBase);
	return a3real3ProductS(v_inout, vBase, ratio);
}

A3_INLINE a3real3r a3real3Sum(a3real3p v_out, const a3real3p vL, const a3real3p vR)
{
	v_out[0] = vL[0] + vR[0];
	v_out[1] = vL[1] + vR[1];
	v_out[2] = vL[2] + vR[2];
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Diff(a3real3p v_out, const a3real3p vL, const a3real3p vR)
{
	v_out[0] = vL[0] - vR[0];
	v_out[1] = vL[1] - vR[1];
	v_out[2] = vL[2] - vR[2];
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3ProductS(a3real3p v_out, const a3real3p v, const a3real s)
{
	v_out[0] = v[0] * s;
	v_out[1] = v[1] * s;
	v_out[2] = v[2] * s;
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3QuotientS(a3real3p v_out, const a3real3p v, const a3real s)
{
	const a3real sInv = a3recip(s);
	v_out[0] = v[0] * sInv;
	v_out[1] = v[1] * sInv;
	v_out[2] = v[2] * sInv;
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3ProductComp(a3real3p v_out, const a3real3p vL, const a3real3p vR)
{
	v_out[0] = vL[0] * vR[0];
	v_out[1] = vL[1] * vR[1];
	v_out[2] = vL[2] * vR[2];
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3QuotientComp(a3real3p v_out, const a3real3p vL, const a3real3p vR)
{
	v_out[0] = vL[0] / vR[0];
	v_out[1] = vL[1] / vR[1];
	v_out[2] = vL[2] / vR[2];
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Add(a3real3p vL_inout, const a3real3p vR)
{
	vL_inout[0] += vR[0];
	vL_inout[1] += vR[1];
	vL_inout[2] += vR[2];
	// 
	return vL_inout;
}

A3_INLINE a3real3r a3real3Sub(a3real3p vL_inout, const a3real3p vR)
{
	vL_inout[0] -= vR[0];
	vL_inout[1] -= vR[1];
	vL_inout[2] -= vR[2];
	// 
	return vL_inout;
}

A3_INLINE a3real3r a3real3MulS(a3real3p v_inout, const a3real s)
{
	v_inout[0] *= s;
	v_inout[1] *= s;
	v_inout[2] *= s;
	// 
	return v_inout;
}

A3_INLINE a3real3r a3real3DivS(a3real3p v_inout, const a3real s)
{
	const a3real sInv = a3recip(s);
	v_inout[0] *= sInv;
	v_inout[1] *= sInv;
	v_inout[2] *= sInv;
	// 
	return v_inout;
}

A3_INLINE a3real3r a3real3MulComp(a3real3p vL_inout, const a3real3p vR)
{
	vL_inout[0] *= vR[0];
	vL_inout[1] *= vR[1];
	vL_inout[2] *= vR[2];
	// 
	return vL_inout;
}

A3_INLINE a3real3r a3real3DivComp(a3real3p vL_inout, const a3real3p vR)
{
	vL_inout[0] /= vR[0];
	vL_inout[1] /= vR[1];
	vL_inout[2] /= vR[2];
	// 
	return vL_inout;
}


// interpolation algorithms
///
A3_INLINE a3real3r a3real3Lerp(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	v_out[2] = a3lerp(v0[2], v1[2], param);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3NLerp(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	v_out[2] = a3lerp(v0[2], v1[2], param);
	// 
	return a3real3Normalize(v_out);
}

A3_INLINE a3real3r a3real3Bilerp(a3real3p v_out, const a3real3p v00, const a3real3p v01, const a3real3p v10, const a3real3p v11, const a3real param0, const a3real param1)
{
	v_out[0] = a3bilerp(v00[0], v01[0], v10[0], v11[0], param0, param1);
	v_out[1] = a3bilerp(v00[1], v01[1], v10[1], v11[1], param0, param1);
	v_out[2] = a3bilerp(v00[2], v01[2], v10[2], v11[2], param0, param1);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Trilerp(a3real3p v_out, const a3real3p v000, const a3real3p v001, const a3real3p v010, const a3real3p v011, const a3real3p v100, const a3real3p v101, const a3real3p v110, const a3real3p v111, const a3real param0, const a3real param1, const a3real param2)
{
	v_out[0] = a3trilerp(v000[0], v001[0], v010[0], v011[0], v100[0], v101[0], v110[0], v111[0], param0, param1, param2);
	v_out[1] = a3trilerp(v000[1], v001[1], v010[1], v011[1], v100[1], v101[1], v110[1], v111[1], param0, param1, param2);
	v_out[2] = a3trilerp(v000[2], v001[2], v010[2], v011[2], v100[2], v101[2], v110[2], v111[2], param0, param1, param2);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3CatmullRom(a3real3p v_out, const a3real3p vPrev, const a3real3p v0, const a3real3p v1, const a3real3p vNext, const a3real param)
{
	v_out[0] = a3CatmullRom(vPrev[0], v0[0], v1[0], vNext[0], param);
	v_out[1] = a3CatmullRom(vPrev[1], v0[1], v1[1], vNext[1], param);
	v_out[2] = a3CatmullRom(vPrev[2], v0[2], v1[2], vNext[2], param);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3HermiteControl(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real3p vControl0, const a3real3p vControl1, const a3real param)
{
	v_out[0] = a3HermiteControl(v0[0], v1[0], vControl0[0], vControl1[0], param);
	v_out[1] = a3HermiteControl(v0[1], v1[1], vControl0[1], vControl1[1], param);
	v_out[2] = a3HermiteControl(v0[2], v1[2], vControl0[2], vControl1[2], param);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3HermiteTangent(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real3p vTangent0, const a3real3p vTangent1, const a3real param)
{
	v_out[0] = a3HermiteTangent(v0[0], v1[0], vTangent0[0], vTangent1[0], param);
	v_out[1] = a3HermiteTangent(v0[1], v1[1], vTangent0[1], vTangent1[1], param);
	v_out[2] = a3HermiteTangent(v0[2], v1[2], vTangent0[2], vTangent1[2], param);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Bezier0(a3real3p v_out, const a3real3p v0, const a3real param)
{
	v_out[0] = v0[0];
	v_out[1] = v0[1];
	v_out[2] = v0[2];
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Bezier1(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real param)
{
	v_out[0] = a3lerp(v0[0], v1[0], param);
	v_out[1] = a3lerp(v0[1], v1[1], param);
	v_out[2] = a3lerp(v0[2], v1[2], param);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Bezier2(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real3p v2, const a3real param)
{
	v_out[0] = a3Bezier2(v0[0], v1[0], v2[0], param);
	v_out[1] = a3Bezier2(v0[1], v1[1], v2[1], param);
	v_out[2] = a3Bezier2(v0[2], v1[2], v2[2], param);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Bezier3(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real3p v2, const a3real3p v3, const a3real param)
{
	v_out[0] = a3Bezier3(v0[0], v1[0], v2[0], v3[0], param);
	v_out[1] = a3Bezier3(v0[1], v1[1], v2[1], v3[1], param);
	v_out[2] = a3Bezier3(v0[2], v1[2], v2[2], v3[2], param);
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3BezierN(a3real3p v_out, a3count order_N, const a3real3 v[], const a3real param)
{
	a3real3 Bn0, Bn1;

	if (order_N == 0)
		return a3real3SetReal3(v_out, *v);

	if (--order_N == 0)
		return a3real3Lerp(v_out, *(v + 0), *(v + 1), param);
	
	a3real3BezierN(Bn0, order_N, v + 0, param);
	a3real3BezierN(Bn1, order_N, v + 1, param);
	return a3real3Lerp(v_out, Bn0, Bn1, param);
}

A3_INLINE a3real3r a3real3Slerp(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real param)
{
	const a3real lenSq = a3real3LengthSquared(v0) * a3real3LengthSquared(v1);
	const a3real d = a3real3Dot(v0, v1) * a3sqrtInverse(lenSq);
	if (a3isBetweenOnes(d))
	{
		const a3real a = a3acosd(d);
		const a3real s = a3cscd(a);
		const a3real s0 = s * a3sind(a - param*a);
		const a3real s1 = s * a3sind(param*a);
		v_out[0] = v0[0] * s0 + v1[0] * s1;
		v_out[1] = v0[1] * s0 + v1[1] * s1;
		v_out[2] = v0[2] * s0 + v1[2] * s1;
		// 
		return v_out;
	}
	return a3real3SetReal3(v_out, v0);
}

A3_INLINE a3real3r a3real3SlerpUnit(a3real3p v_out, const a3real3p v0, const a3real3p v1, const a3real param)
{
	const a3real d = a3real3Dot(v0, v1);
	if (a3isBetweenOnes(d))
	{
		const a3real a = a3acosd(d);
		const a3real s = a3cscd(a);
		const a3real s0 = s * a3sind(a - param*a);
		const a3real s1 = s * a3sind(param*a);
		v_out[0] = v0[0] * s0 + v1[0] * s1;
		v_out[1] = v0[1] * s0 + v1[1] * s1;
		v_out[2] = v0[2] * s0 + v1[2] * s1;
		// 
		return v_out;
	}
	return a3real3SetReal3(v_out, v0);
}

A3_INLINE a3real a3real3CalculateArcLengthCatmullRom(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p vPrev, const a3real3p v0, const a3real3p v1, const a3real3p vNext)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real3 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real3SetReal3(vvPrev, v0);
		a3real3SetReal3(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3CatmullRom(vv, vPrev, v0, v1, vNext, t);
			arclen_delta = a3real3Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real3SetReal3(vvPrev, vv);
			a3real3SetReal3(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real3CalculateArcLengthHermiteControl(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p v0, const a3real3p v1, const a3real3p vControl0, const a3real3p vControl1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real3 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real3SetReal3(vvPrev, v0);
		a3real3SetReal3(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3HermiteControl(vv, v0, v1, vControl0, vControl1, t);
			arclen_delta = a3real3Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real3SetReal3(vvPrev, vv);
			a3real3SetReal3(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real3CalculateArcLengthHermiteTangent(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p v0, const a3real3p v1, const a3real3p vTangent0, const a3real3p vTangent1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real3 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real3SetReal3(vvPrev, v0);
		a3real3SetReal3(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3HermiteTangent(vv, v0, v1, vTangent0, vTangent1, t);
			arclen_delta = a3real3Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real3SetReal3(vvPrev, vv);
			a3real3SetReal3(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real3CalculateArcLengthBezier0(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p v0)
{
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = a3real_zero;
		a3real3SetReal3(*sampleTable_out, v0);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		for (i = 1; i <= numDivisions; ++i)
		{
			*(++arclenTable_out) = a3real_zero;
			a3real3SetReal3(*(++sampleTable_out), v0);
			if (paramTable_out)
				*(++paramTable_out) = a3real_zero;
		}
	}
	return a3real_zero;
}

A3_INLINE a3real a3real3CalculateArcLengthBezier1(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p v0, const a3real3p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real3Distance(v0, v1), arclen_delta = arclen / (a3real)numDivisions, arc = a3real_zero, t = a3real_zero, dt;
	a3real3 vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arc;
		a3real3SetReal3(*sampleTable_out, v0);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3Lerp(vv, v0, v1, t);
			arc += arclen_delta;
			*(++arclenTable_out) = autoNormalize ? t : arc;
			a3real3SetReal3(*(++sampleTable_out), vv);
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
	}
	return arclen;
}

A3_INLINE a3real a3real3CalculateArcLengthBezier2(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real3 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real3SetReal3(vvPrev, v0);
		a3real3SetReal3(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3Bezier2(vv, v0, v1, v2, t);
			arclen_delta = a3real3Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real3SetReal3(vvPrev, vv);
			a3real3SetReal3(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real3CalculateArcLengthBezier3(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p v0, const a3real3p v1, const a3real3p v2, const a3real3p v3)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real3 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real3SetReal3(vvPrev, v0);
		a3real3SetReal3(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3Bezier3(vv, v0, v1, v2, v3, t);
			arclen_delta = a3real3Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real3SetReal3(vvPrev, vv);
			a3real3SetReal3(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real3CalculateArcLengthBezierN(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3count order_N, const a3real3 v[])
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real3 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real3SetReal3(vvPrev, *v);
		a3real3SetReal3(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3BezierN(vv, order_N, v, t);
			arclen_delta = a3real3Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real3SetReal3(vvPrev, vv);
			a3real3SetReal3(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real3CalculateArcLengthSlerp(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p v0, const a3real3p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real3 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real3SetReal3(vvPrev, v0);
		a3real3SetReal3(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3Slerp(vv, v0, v1, t);
			arclen_delta = a3real3Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real3SetReal3(vvPrev, vv);
			a3real3SetReal3(*(++sampleTable_out), vvPrev);
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

A3_INLINE a3real a3real3CalculateArcLengthSlerpUnit(a3real3 sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real3p v0, const a3real3p v1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real3 vvPrev, vv;
	a3index i;

	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		a3real3SetReal3(vvPrev, v0);
		a3real3SetReal3(*sampleTable_out, vvPrev);
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			a3real3SlerpUnit(vv, v0, v1, t);
			arclen_delta = a3real3Distance(vv, vvPrev);
			arclen += arclen_delta;
			*(++arclenTable_out) = arclen;
			a3real3SetReal3(vvPrev, vv);
			a3real3SetReal3(*(++sampleTable_out), vvPrev);
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
A3_INLINE a3real3r a3real3GramSchmidtOrtho(a3real3p v_out, const a3real3p v, const a3real3p vBase)
{
	a3real3 proj;
	a3real3Projected(proj, v, vBase);
	return a3real3Diff(v_out, v, proj);
}

A3_INLINE a3real3r a3real3GramSchmidtOrtho2(a3real3p v0_out, a3real3p v1_out, const a3real3p v0, const a3real3p v1, const a3real3p vBase)
{
	a3real3 proj0, proj1;
	a3real3Projected(proj0, v0, vBase);
	a3real3Diff(v0_out, v0, proj0);
	a3real3Projected(proj0, v1, vBase);
	a3real3Projected(proj1, v1, v0_out);
	a3real3Add(proj1, proj0);
	return a3real3Diff(v1_out, v1, proj1);
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

A3_INLINE a3real3r a3real3GramSchmidt(a3real3p v_inout, const a3real3p vBase)
{
	a3real3 proj;
	a3real3Projected(proj, v_inout, vBase);
	return a3real3Sub(v_inout, proj);
}

A3_INLINE a3real3r a3real3GramSchmidt2(a3real3p v0_inout, a3real3p v1_inout, const a3real3p vBase)
{
	a3real3 proj0, proj1;
	a3real3Projected(proj0, v0_inout, vBase);
	a3real3Sub(v0_inout, proj0);
	a3real3Projected(proj0, v1_inout, vBase);
	a3real3Projected(proj1, v1_inout, v0_inout);
	a3real3Add(proj1, proj0);
	return a3real3Sub(v1_inout, proj1);
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


// cross product and applications (3D only)
///
A3_INLINE a3real3r a3real3Cross(a3real3p v_out, const a3real3p vL, const a3real3p vR)
{
	// x = yLzR - zLyR
	// y = zLxR - xLzR
	// z = xLyR - yLxR
	v_out[0] = vL[1] * vR[2] - vL[2] * vR[1];
	v_out[1] = vL[2] * vR[0] - vL[0] * vR[2];
	v_out[2] = vL[0] * vR[1] - vL[1] * vR[0];
	return v_out;
}

A3_INLINE a3real3r a3real3CrossUnit(a3real3p v_out, const a3real3p vL, const a3real3p vR)
{
	return a3real3Normalize(a3real3Cross(v_out, vL, vR));
}

A3_INLINE a3real3r a3real3TriangleNormal(a3real3p n_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real3 dv1, dv2;
	a3real3Diff(dv1, v1, v0);
	a3real3Diff(dv2, v2, v0);
	return a3real3Cross(n_out, dv1, dv2);
}

A3_INLINE a3real3r a3real3TriangleNormalUnit(a3real3p n_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real3 dv1, dv2;
	a3real3Diff(dv1, v1, v0);
	a3real3Diff(dv2, v2, v0);
	return a3real3CrossUnit(n_out, dv1, dv2);
}


// normal and area of triangle
///
A3_INLINE a3real a3real3TriangleAreaDoubledNormal(a3real3p n_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real len;
	a3real3TriangleNormal(n_out, v0, v1, v2);
	len = a3real3Length(n_out);
	return (len);
}

A3_INLINE a3real a3real3TriangleAreaSquaredNormal(a3real3p n_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real lenSq;
	a3real3TriangleNormal(n_out, v0, v1, v2);
	lenSq = a3real3LengthSquared(n_out);
	return (lenSq * a3real_quarter);
}

A3_INLINE a3real a3real3TriangleAreaNormal(a3real3p n_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real len;
	a3real3TriangleNormal(n_out, v0, v1, v2);
	len = a3real3Length(n_out);
	return (len * a3real_half);
}

A3_INLINE a3real a3real3TriangleAreaDoubledNormalUnit(a3real3p n_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real lenSq, lenInv;
	a3real3TriangleNormal(n_out, v0, v1, v2);
	lenSq = a3real3LengthSquared(n_out);
	lenInv = a3sqrtInverse(lenSq);
	a3real3MulS(n_out, lenInv);
	return (lenSq*lenInv);
}

A3_INLINE a3real a3real3TriangleAreaSquaredNormalUnit(a3real3p n_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real lenSq, lenInv;
	a3real3TriangleNormal(n_out, v0, v1, v2);
	lenSq = a3real3LengthSquared(n_out);
	lenInv = a3sqrtInverse(lenSq);
	a3real3MulS(n_out, lenInv);
	return (lenSq*a3real_quarter);
}

A3_INLINE a3real a3real3TriangleAreaNormalUnit(a3real3p n_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real lenSq, lenInv;
	a3real3TriangleNormal(n_out, v0, v1, v2);
	lenSq = a3real3LengthSquared(n_out);
	lenInv = a3sqrtInverse(lenSq);
	a3real3MulS(n_out, lenInv);
	return (lenSq*lenInv*a3real_half);
}


// area of a triangle
///
A3_INLINE a3real a3real3TriangleAreaDoubled(const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real3 n;
	return a3real3TriangleAreaDoubledNormal(n, v0, v1, v2);
}

A3_INLINE a3real a3real3TriangleAreaSquared(const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real3 n;
	return a3real3TriangleAreaSquaredNormal(n, v0, v1, v2);
}

A3_INLINE a3real a3real3TriangleArea(const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real3 n;
	return a3real3TriangleAreaNormal(n, v0, v1, v2);
}


// point is in triangle (barycentric coordinates)
///
A3_INLINE a3boolean a3real3PointIsInTriangle(const a3real3p p, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	a3real3 a, b, q;
	a3real aa, bb, ab, qa, qb, alpha, beta, gamma;
	a3real3Diff(a, v0, v2);
	a3real3Diff(b, v1, v2);
	a3real3Diff(q, p, v2);
	aa = a3real3Dot(a, a);
	bb = a3real3Dot(b, b);
	ab = a3real3Dot(a, b);
	qa = a3real3Dot(q, a);
	qb = a3real3Dot(q, b);
	beta = (aa*qb - ab*qa) / (aa*bb - ab*ab);
	alpha = (qa - beta*ab) / aa;
	gamma = alpha + beta;
	return (alpha >= a3real_zero && alpha <= a3real_one && beta >= a3real_zero && beta <= a3real_one && gamma <= a3real_one);
}

A3_INLINE a3boolean a3real3PointIsInTriangleBarycentric(const a3real3p p, const a3real3p v0, const a3real3p v1, const a3real3p v2, a3real *param0, a3real *param1, a3real *param2)
{
	a3real3 a, b, q;
	a3real aa, bb, ab, qa, qb, alpha, beta, gamma;
	a3real3Diff(a, v0, v2);
	a3real3Diff(b, v1, v2);
	a3real3Diff(q, p, v2);
	aa = a3real3Dot(a, a);
	bb = a3real3Dot(b, b);
	ab = a3real3Dot(a, b);
	qa = a3real3Dot(q, a);
	qb = a3real3Dot(q, b);
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


#endif	// !__ANIMAL3D_A3DM_VECTOR3_IMPL_INL
#endif	// __ANIMAL3D_A3DM_VECTOR_H