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

	a3interpolation_impl.inl
	Implementations for interpolation algorithms.
*/

#ifdef __ANIMAL3D_A3DM_INTERPOLATION_H
#ifndef __ANIMAL3D_A3DM_INTERPOLATION_IMPL_INL
#define __ANIMAL3D_A3DM_INTERPOLATION_IMPL_INL


//-----------------------------------------------------------------------------
// CUT FROM HEADER

//////// just below hermite
// inverse Hermite/cspline: given known value and constants, compute the 't'
//	parameter that would yield 'n' as a result if used in Hermite
// safe version ensures n0 != n1
A3_INLINE a3real a3HermiteControlInverse(const a3real n0, const a3real n1, const a3real nControl0, const a3real nControl1, const a3real value);
A3_INLINE a3real a3HermiteTangentInverse(const a3real n0, const a3real n1, const a3real nTangent0, const a3real nTangent1, const a3real value);
A3_INLINE a3real a3HermiteControlSafeInverse(const a3real n0, const a3real n1, const a3real nControl0, const a3real nControl1, const a3real value);
A3_INLINE a3real a3HermiteTangentSafeInverse(const a3real n0, const a3real n1, const a3real nTangent0, const a3real nTangent1, const a3real value);

//-----------------------------------------------------------------------------


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// linear interpolation ("LERP"): calculate linear interpolation between 
//	reference values 'n0' and 'n1' using a interpolation parameter 'param'
//	(sometimes referred to as 't' parameter or value)
// t values between 0 and 1 yield interpolation, others yield extrapolation
// result when param = 0 is n0, and result when param = 1 is n1
A3_INLINE a3real a3lerpFunc(const a3real n0, const a3real n1, const a3real param)
{
	const a3real n = a3lerp(n0, n1, param);
	return n;
}

// inverse linear interpolation: given a known value 'value' and reference 
//	values 'n0' and 'n1' (same as above), compute the 't' parameter 
//	that would yield 'n' as a result if used in LERP
// alternatively, the safe version makes sure n0 != n1
A3_INLINE a3real a3lerpInverse(const a3real n0, const a3real n1, const a3real value)
{
	const a3real t = a3unlerp(n0, n1, value);
	return t;
}

A3_INLINE a3real a3lerpSafeInverse(const a3real n0, const a3real n1, const a3real value)
{
	a3real t;
	if (n0 != n1)
		t = a3unlerp(n0, n1, value);
	else
		t = a3real_zero;
//	const a3real t = (n0 != n1 ? a3unlerp(n0, n1, value) : a3real_zero);
	return t;
}


// remap: convert a value contained in one range to another range
//	values 'n0' and 'n1' are the old range limits, 'n0_new' and 'n1_new' are 
//	the new range limits
// safe version makes sure n1 != n0
A3_INLINE a3real a3remapFunc(const a3real n0_new, const a3real n1_new, const a3real n0, const a3real n1, const a3real value)
{
	const a3real n = a3remap(n0_new, n1_new, n0, n1, value);
	return n;
}

A3_INLINE a3real a3remapSafe(const a3real n0_new, const a3real n1_new, const a3real n0, const a3real n1, const a3real value)
{
	a3real n;
	if (n0 != n1)
		n = a3remap(n0_new, n1_new, n0, n1, value);
	else
		n = a3real_zero;
//	const a3real n = (n0 != n1 ? a3remap(n0_new, n1_new, n0, n1, value) : a3real_zero);
	return n;
}


// bi-linear interpolation: calculate lerp between each pair of numbers 
//	(n00, n01 and n10, n11) and then calculate lerp between the results
A3_INLINE a3real a3bilerp(const a3real n00, const a3real n01, const a3real n10, const a3real n11, const a3real param0, const a3real param1)
{
	const a3real l0 = a3lerp(n00, n01, param0);
	const a3real l1 = a3lerp(n10, n11, param0);
	const a3real n = a3lerp(l0, l1, param1);
	return n;
}


// tri-linear interpolation: extensions of the above
A3_INLINE a3real a3trilerp(const a3real n000, const a3real n001, const a3real n010, const a3real n011, const a3real n100, const a3real n101, const a3real n110, const a3real n111, const a3real param0, const a3real param1, const a3real param2)
{
	const a3real bl0 = a3bilerp(n000, n001, n010, n011, param0, param1);
	const a3real bl1 = a3bilerp(n100, n101, n110, n111, param0, param1);
	const a3real n = a3lerp(bl0, bl1, param2);
	return n;
}


// Catmull-Rom spline interpolation
A3_INLINE a3real a3CatmullRom(const a3real nPrev, const a3real n0, const a3real n1, const a3real nNext, const a3real param)
{
	// n = [p-1, p0, p1, p2] K [1, t, t2, t3]transpose
	//		| 0 -1  2 -1 |
	//	K =	| 2  0 -5  3 |
	//		| 0  1  4 -3 |
	//		| 0  0 -1  1 |
	// fully expanded: 
	const a3real n = n0 + a3real_half*param*(n1 - nPrev + param*(a3real_two*nPrev - a3real_five*n0 + a3real_four*n1 - nNext + param*(nNext - nPrev + a3real_three*n0 - a3real_three*n1)));
	return n;
}


// two versions of cubic hermite spline (c-spline) interpolation: 
// use control values/handles (q) or tangents/derivatives (m) as inputs
A3_INLINE a3real a3HermiteControl(const a3real n0, const a3real n1, const a3real nControl0, const a3real nControl1, const a3real param)
{
	// n = [p0, q0, p1, q1] K [1, t, t2, t3]transpose
	//		| 1 -1 -1  1 |
	//	K =	| 0  1 -2  1 |
	//		| 0  0  4 -3 |
	//		| 0  0 -1  1 |
	// fully expanded: 
	const a3real n = n0 + param*(nControl0 - n0 + param*(a3real_four*n1 - n0 - nControl1 - a3real_two*nControl0 + param*(nControl0 + nControl1 + n0 - a3real_three*n1)));
	return n;
}

A3_INLINE a3real a3HermiteTangent(const a3real n0, const a3real n1, const a3real nTangent0, const a3real nTangent1, const a3real param)
{
	// n = [p0, m0, p1, m1] K [1, t, t2, t3]transpose
	//		| 1  0 -3  2 |
	//	K =	| 0  1 -2  1 |
	//		| 0  0  3 -2 |
	//		| 0  0 -1  1 |
	// fully expanded: 
	const a3real n = n0 + param*(nTangent0 + param*(a3real_three*n1 - a3real_three*n0 - nTangent1 - a3real_two*nTangent0 + param*(nTangent0 + nTangent1 + a3real_two*n0 - a3real_two*n1)));
	return n;
}


// inverse Hermite/cspline: given known value and constants, compute the 't' 
//	parameter that would yield 'n' as a result if used in Hermite
// safe version ensures n0 != n1
A3_INLINE a3real a3HermiteControlInverse(const a3real n0, const a3real n1, const a3real nControl0, const a3real nControl1, const a3real value)
{
	const a3real dn = a3real_one / (n1 - n0);
	const a3real n = (value - n0) * dn;
	const a3real m0 = (nControl0 - n0) * dn;
	const a3real q1 = (nControl1 - n0) * dn;

	// TO DO: calculate inverse
	return a3real_zero;
}

A3_INLINE a3real a3HermiteTangentInverse(const a3real n0, const a3real n1, const a3real nTangent0, const a3real nTangent1, const a3real value)
{
	const a3real dn = a3real_one / (n1 - n0);
	const a3real n = (value - n0) * dn;
	const a3real m0 = nTangent0 * dn;
	const a3real q1 = (n1 + nTangent1) * dn;

	// TO DO: calculate inverse
	return a3real_zero;
}

A3_INLINE a3real a3HermiteControlSafeInverse(const a3real n0, const a3real n1, const a3real nControl0, const a3real nControl1, const a3real value)
{
	a3real t;
	if (n0 != n1)
		t = a3HermiteControlInverse(n0, n1, nControl0, nControl1, value);
	else
		t = a3real_zero;
	return t;
}

A3_INLINE a3real a3HermiteTangentSafeInverse(const a3real n0, const a3real n1, const a3real nTangent0, const a3real nTangent1, const a3real value)
{
	a3real t;
	if (n0 != n1)
		t = a3HermiteTangentInverse(n0, n1, nTangent0, nTangent1, value);
	else
		t = a3real_zero;
	return t;
}


// Bezier spline interpolation methods (it's recursive)
// function call is Bezier<order-N>
A3_INLINE a3real a3Bezier0(const a3real n0, const a3real param)
{
	// order 0: return point, parameter does not matter
	return n0;
}

A3_INLINE a3real a3Bezier1(const a3real n0, const a3real n1, const a3real param)
{
	// order 1: lerp
	const a3real B1 = a3lerp(n0, n1, param);
	return B1;
}

A3_INLINE a3real a3Bezier2(const a3real n0, const a3real n1, const a3real n2, const a3real param)
{
	// order 2: recursive using previous order
	const a3real B10 = a3lerp(n0, n1, param);
	const a3real B11 = a3lerp(n1, n2, param);
	const a3real B2 = a3lerp(B10, B11, param);
	return B2;
}

A3_INLINE a3real a3Bezier3(const a3real n0, const a3real n1, const a3real n2, const a3real n3, const a3real param)
{
	// order 3: ditto
	const a3real B20 = a3Bezier2(n0, n1, n2, param);
	const a3real B21 = a3Bezier2(n1, n2, n3, param);
	const a3real B3 = a3lerp(B20, B21, param);
	return B3;
}

A3_INLINE a3real a3BezierN(a3count order_N, const a3real n[], const a3real param)
{
	// recursion: each iteration linear interpolates iterations of the previous degree
	a3real Bn0, Bn1, BN;

	// base case: order 0
	if (order_N == 0)
		return *n;
	
	// special case: order 1 is lerp (decrements order once)
	if (--order_N == 0)
		BN = a3lerp(*(n + 0), *(n + 1), param);
	else
	{
		// full Bezier recursion
		Bn0 = a3BezierN(order_N, n + 0, param);
		Bn1 = a3BezierN(order_N, n + 1, param);
		BN = a3lerp(Bn0, Bn1, param);
	}
	return BN;
}


//-----------------------------------------------------------------------------
// utility to calculate the arc length along a curved segment
// returns the total arc length of the segment
// outputs the samples, sample parameters and arc lengths at samples to arrays
// number of elements in each array must be 'numDivisions' + 1
// user can also choose to auto-normalize parameters
// 'paramTable_out' is an optional input; 't' params will not be stored if null

A3_INLINE a3real a3calculateArcLengthCatmullRom(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real nPrev, const a3real n0, const a3real n1, const a3real nNext)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real vPrev, v;
	a3index i;
	
	// validate inputs
	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		// store first sample
		*arclenTable_out = arclen;
		*sampleTable_out = vPrev = n0;
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		// set sampling interval
		dt = a3recip((a3real)numDivisions);

		// iterate through curve
		for (i = 1; i <= numDivisions; ++i)
		{
			// increment sampling parameter and take next sample
			t += dt;
			v = a3CatmullRom(nPrev, n0, n1, nNext, t);

			// compute delta, make sure it's positive
			// add delta to total, store current sample
			arclen_delta = v - vPrev;
			arclen += a3absolute(arclen_delta);
			*(++arclenTable_out) = arclen;
			*(++sampleTable_out) = vPrev = v;
			if (paramTable_out)
				*(++paramTable_out) = t;
		}

		// if auto-normalizing, divide all arc lengths by the total
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= numDivisions; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
	}
	return arclen;
}

A3_INLINE a3real a3calculateArcLengthHermiteControl(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real n0, const a3real n1, const a3real nControl0, const a3real nControl1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real vPrev, v;
	a3index i;

	// ditto
	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		*sampleTable_out = vPrev = n0;
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			v = a3HermiteControl(n0, n1, nControl0, nControl1, t);
			arclen_delta = v - vPrev;
			arclen += a3absolute(arclen_delta);
			*(++arclenTable_out) = arclen;
			*(++sampleTable_out) = vPrev = v;
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

A3_INLINE a3real a3calculateArcLengthHermiteTangent(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real n0, const a3real n1, const a3real nTangent0, const a3real nTangent1)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real vPrev, v;
	a3index i;

	// ditto
	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		*sampleTable_out = vPrev = n0;
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			v = a3HermiteTangent(n0, n1, nTangent0, nTangent1, t);
			arclen_delta = v - vPrev;
			arclen += a3absolute(arclen_delta);
			*(++arclenTable_out) = arclen;
			*(++sampleTable_out) = vPrev = v;
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

A3_INLINE a3real a3calculateArcLengthBezier0(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real n0)
{
	a3index i;

	// optimized for a point: all values are the same, result is zero
	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = a3real_zero;
		*sampleTable_out = n0;
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		for (i = 1; i <= numDivisions; ++i)
		{
			*(++arclenTable_out) = a3real_zero;
			*(++sampleTable_out) = n0;
			if (paramTable_out)
				*(++paramTable_out) = a3real_zero;
		}
	}
	return a3real_zero;
}

A3_INLINE a3real a3calculateArcLengthBezier1(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real n0, const a3real n1)
{
	a3real arclen = n1 - n0, arclen_delta, t = a3real_zero, dt;
	a3real v;
	a3index i;

	if (arclen < a3real_zero)
		arclen = -arclen;

	// optimized for a line: evenly divided pieces
	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		arclen_delta = arclen / (a3real)numDivisions;
		dt = a3recip((a3real)numDivisions);

		*arclenTable_out = arclen;
		*sampleTable_out = v = n0;
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			v += arclen_delta;
			arclen = autoNormalize ? t : v;

			*(++arclenTable_out) = arclen;
			*(++sampleTable_out) = v;
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
	}
	return arclen;
}

A3_INLINE a3real a3calculateArcLengthBezier2(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real n0, const a3real n1, const a3real n2)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real vPrev, v;
	a3index i;

	// ditto for curve
	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		*sampleTable_out = vPrev = n0;
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			v = a3Bezier2(n0, n1, n2, t);
			arclen_delta = v - vPrev;
			arclen += a3absolute(arclen_delta);
			*(++arclenTable_out) = arclen;
			*(++sampleTable_out) = vPrev = v;
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

A3_INLINE a3real a3calculateArcLengthBezier3(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3real n0, const a3real n1, const a3real n2, const a3real n3)
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real vPrev, v;
	a3index i;

	// ditto
	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		*sampleTable_out = vPrev = n0;
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			v = a3Bezier3(n0, n1, n2, n3, t);
			arclen_delta = v - vPrev;
			arclen += a3absolute(arclen_delta);
			*(++arclenTable_out) = arclen;
			*(++sampleTable_out) = vPrev = v;
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

A3_INLINE a3real a3calculateArcLengthBezierN(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3count order_N, const a3real n[])
{
	a3real *arclen_ptr = arclenTable_out;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv, t = a3real_zero, dt;
	a3real vPrev, v;
	a3index i;

	// ditto
	if (numDivisions && arclenTable_out && sampleTable_out)
	{
		*arclenTable_out = arclen;
		*sampleTable_out = vPrev = *n;
		if (paramTable_out)
			*paramTable_out = a3real_zero;

		dt = a3recip((a3real)numDivisions);
		for (i = 1; i <= numDivisions; ++i)
		{
			t += dt;
			v = a3BezierN(order_N, n, t);
			arclen_delta = v - vPrev;
			arclen += a3absolute(arclen_delta);
			*(++arclenTable_out) = arclen;
			*(++sampleTable_out) = vPrev = v;
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


//-----------------------------------------------------------------------------
// utilities to sample from a table using linear interpolation, where 
//	'valueTable' is the table of sampled values, 
//	'paramTable' is the table of sampling parameters used, 
//	'param' is the input value we are searching for, and 
//	'i' is the a3index to start searching from
// assumes all values are valid (does not check)

// find a3index and calculate value using lerp
// assumes that parameter values increase as we step through the table
A3_INLINE a3real a3sampleTableLerpIncrement(const a3real valueTable[], const a3real paramTable[], const a3real param, a3index i)
{
	// searching for a set of parameters (t0 and t1) that encompass input param
	a3real t, n0, n1, n;
	a3real t0 = *(paramTable + i);
	a3real t1 = *(paramTable + ++i);

	// while the upper limit is still less than our input
	while (t1 < param)
	{
		// advance to next parameter entry
		t0 = t1;
		t1 = *(paramTable + ++i);
	}

	// found previous and next param, determine where we are relative to them
	// use that parameter for linear interpolation between the known samples
	t = a3unlerp(t0, t1, param);
	n1 = *(valueTable + i);
	n0 = *(valueTable + i - 1);
	n = a3lerp(n0, n1, t);
	return n;
}


// same as the above but accounts for interpolation parameters that decrease 
//	while searching instead of increasing
A3_INLINE a3real a3sampleTableLerpDecrement(const a3real valueTable[], const a3real paramTable[], const a3real param, a3index i)
{
	a3real t, n0, n1, n;
	a3real t0 = *(paramTable + i);
	a3real t1 = *(paramTable + ++i);

	// we're working in reverse, so we are looking for where t1 becomes less
	//	than our input
	while (t1 > param)
	{
		t0 = t1;
		t1 = *(paramTable + ++i);
	}
	t = a3unlerp(t0, t1, param);
	n1 = *(valueTable + i);
	n0 = *(valueTable + i - 1);
	n = a3lerp(n0, n1, t);
	return n;
}


// these are the same but use a step other than 1
A3_INLINE a3real a3sampleTableLerpIncrementStep(const a3real valueTable[], const a3real paramTable[], const a3real param, const a3count step, a3index i)
{
	// a3index increments by indicated step
	a3real t, n0, n1, n;
	a3real t0 = *(paramTable + i);
	a3real t1 = *(paramTable + (i += step));
	while (t1 < param)
	{
		t0 = t1;
		t1 = *(paramTable + (i += step));
	}
	t = a3unlerp(t0, t1, param);
	n1 = *(valueTable + i);
	n0 = *(valueTable + i - step);
	n = a3lerp(n0, n1, t);
	return n;
}

A3_INLINE a3real a3sampleTableLerpDecrementStep(const a3real valueTable[], const a3real paramTable[], const a3real param, const a3count step, a3index i)
{
	a3real t, n0, n1, n;
	a3real t0 = *(paramTable + i);
	a3real t1 = *(paramTable + (i += step));
	while (t1 > param)
	{
		t0 = t1;
		t1 = *(paramTable + (i += step));
	}
	t = a3unlerp(t0, t1, param);
	n1 = *(valueTable + i);
	n0 = *(valueTable + i - step);
	n = a3lerp(n0, n1, t);
	return n;
}


// same as the above but used to only determine at what a3index the search ends
// does not perform interpolation, but returns the a3index that should be used 
//	to grab n1 (n1 is valueTable[i], n0 is valueTable[i-1] or [i-step])
// also returns the interpolation parameter through a pointer
A3_INLINE a3index a3sampleTableLerpIncrementIndex(const a3real paramTable[], const a3real param, a3index i, a3real *param_out)
{
	a3real t0 = *(paramTable + i);
	a3real t1 = *(paramTable + ++i);
	while (t1 < param)
	{
		t0 = t1;
		t1 = *(paramTable + ++i);
	}
	if (param_out)
		*param_out = a3unlerp(t0, t1, param);
	return i;
}

A3_INLINE a3index a3sampleTableLerpDecrementIndex(const a3real paramTable[], const a3real param, a3index i, a3real *param_out)
{
	a3real t0 = *(paramTable + i);
	a3real t1 = *(paramTable + ++i);
	while (t1 > param)
	{
		t0 = t1;
		t1 = *(paramTable + ++i);
	}
	if (param_out)
		*param_out = a3unlerp(t0, t1, param);
	return i;
}

A3_INLINE a3index a3sampleTableLerpIncrementStepIndex(const a3real paramTable[], const a3real param, const a3count step, a3index i, a3real *param_out)
{
	a3real t0 = *(paramTable + i);
	a3real t1 = *(paramTable + (i += step));
	while (t1 < param)
	{
		t0 = t1;
		t1 = *(paramTable + (i += step));
	}
	if (param_out)
		*param_out = a3unlerp(t0, t1, param);
	return i;
}

A3_INLINE a3index a3sampleTableLerpDecrementStepIndex(const a3real paramTable[], const a3real param, const a3count step, a3index i, a3real *param_out)
{
	a3real t0 = *(paramTable + i);
	a3real t1 = *(paramTable + (i += step));
	while (t1 > param)
	{
		t0 = t1;
		t1 = *(paramTable + (i += step));
	}
	if (param_out)
		*param_out = a3unlerp(t0, t1, param);
	return i;
}


// generate a sample table by sampling a function that takes in one argument
// returns the total number of samples (numDivisions*numSubdivisions + 1)
// also a version that will calculate arc length while sampling
A3_INLINE a3index a3sampleTableGenerate(a3real sampleTable_out[], a3real paramTable_out[], const a3count numDivisions, const a3count numSubdivisions, const a3real paramMin, const a3real paramMax, const a3realfunc func)
{
	const a3index n = numDivisions*numSubdivisions;
	const a3real paramRange = paramMax - paramMin;
	const a3real dt = paramRange / (a3real)n;
	a3real v, t = paramMin;
	a3index i;

	// validate
	if (n && paramRange != a3real_zero && func && sampleTable_out && paramTable_out)
	{
		for (i = 0; i <= n; ++i, t += dt)
		{
			// sample from function, store param and sample
			v = func(t);
			*(sampleTable_out++) = v;
			*(paramTable_out++) = t;
		}
		return i;
	}
	return 0;
}

A3_INLINE a3index a3sampleTableGenerateArcLength(a3real sampleTable_out[], a3real paramTable_out[], a3real arclenTable_out[], const a3boolean autoNormalize, const a3count numDivisions, const a3count numSubdivisions, const a3real paramMin, const a3real paramMax, const a3realfunc func)
{
	a3real *arclen_ptr = arclenTable_out;
	const a3index n = numDivisions*numSubdivisions;
	const a3real paramRange = paramMax - paramMin;
	const a3real dt = paramRange / (a3real)n;
	a3real vPrev, v, t = paramMin;
	a3real arclen = a3real_zero, arclen_delta, arclen_inv;
	a3index i;

	// ditto but with arc length calculations; params optional
	if (n && paramRange != a3real_zero && func && sampleTable_out && arclenTable_out)
	{
		// first sample
		vPrev = func(t);
		*sampleTable_out = vPrev;
		*arclenTable_out = arclen;
		if (paramTable_out)
			*paramTable_out = t;

		for (i = 1; i <= n; ++i)
		{
			t += dt;
			v = func(t);
			arclen_delta = v - vPrev;
			arclen += a3absolute(arclen_delta);
			*(++sampleTable_out) = vPrev = v;
			*(++arclenTable_out) = arclen;
			if (paramTable_out)
				*(++paramTable_out) = t;
		}
		if (autoNormalize)
		{
			arclen_inv = a3recip(arclen);
			for (i = 1; i <= n; ++i)
				*(++arclen_ptr) *= arclen_inv;
		}
		return i;
	}
	return 0;
}

A3_INLINE a3index a3sampleTableGenerateNumSamplesRequired(const a3index numDivisions, const a3count numSubdivisions)
{
	const a3index n = numDivisions*numSubdivisions + 1;
	return n;
}


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_INTERPOLATION_IMPL_INL
#endif	// __ANIMAL3D_A3DM_INTERPOLATION_H