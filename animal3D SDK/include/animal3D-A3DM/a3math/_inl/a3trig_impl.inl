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

	a3trig_impl.inl
	Implementations for fast trigonometry functions.
*/

#ifdef __ANIMAL3D_A3DM_TRIG_H
#ifndef __ANIMAL3D_A3DM_TRIG_IMPL_INL
#define __ANIMAL3D_A3DM_TRIG_IMPL_INL


#include "../a3interpolation.h"
#include "../a3sqrt.h"


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// values for fast trig
A3_GLOBAL a3index a3_TaylorItrs = 20;	// optimized at 16, converges at 20
A3_GLOBAL a3index a3_trigSamplesPerDegree = 0;
A3_GLOBAL const a3real *a3_trigParamTablePtr = 0;
A3_GLOBAL const a3real *a3_trigSinSampleTablePtr = 0;
A3_GLOBAL const a3real *a3_trigCosSampleTablePtr = 0;
A3_GLOBAL const a3index *a3_asinIndexTable = 0; //[768]
A3_GLOBAL const a3index *a3_acosIndexTable = 0; //[768]


//-----------------------------------------------------------------------------

// initialize data tables for fast trigonometry
// 'samplesPerDegree' is the number of divisions in a single degree, 
//	must be 1 or greater
// 'table_out' is an externally-allocated, sufficiently-sized array to 
//	hold the function samples and params
// total number of samples in each function will be samplesPerDegree x 720 + 1
// samples for sin and cos, and the parameters will be retained, so the total 
//	number of elements in the table should be (samplesPerDegree x 720 + 1) x 3
// returns 1 if success, 0 if fail
// can also just check the number of samples that would be required
// alternatively, just set existing tables
A3_INLINE a3index a3trigInit(const a3index samplesPerDegree, a3real table_out[])
{
	const a3index n_func = (samplesPerDegree * 720) + 1;
	const a3index n = (n_func * 3) + (768 * 2);
	a3index i, j;
	a3integer t;
	a3real x;
	a3index *asinIndexItr, *acosIndexItr;
	a3real *paramTableItr, *sinTableItr, *cosTableItr;

	if (samplesPerDegree && table_out)
	{
		// set internals
		a3_trigSamplesPerDegree = samplesPerDegree;
		a3_trigParamTablePtr = paramTableItr = table_out;
		a3_trigSinSampleTablePtr = sinTableItr = paramTableItr + n_func;
		a3_trigCosSampleTablePtr = cosTableItr = sinTableItr + n_func;
		a3_asinIndexTable = asinIndexItr = (a3index *)(cosTableItr + n_func);
		a3_acosIndexTable = acosIndexItr = asinIndexItr + 768;


		// use interpolation utilities to calculate tables
		for (t = -360; t < +360; ++t)
		{
			for (i = 0; i < samplesPerDegree; ++i)
			{
				x = *(paramTableItr++) = (a3real)t + (a3real)i / (a3real)samplesPerDegree;
				a3trigTaylor_sind_cosd(x, sinTableItr++, cosTableItr++);
			}
		}
		// copy first values to end
		*paramTableItr = (a3real)t;
		*sinTableItr = *a3_trigSinSampleTablePtr;
		*cosTableItr = *a3_trigCosSampleTablePtr;


		// prepare a3index tables for inverse trig
		// run the algorithm on sin only and use identities 
		//	to determine what the indices will be for cos
		for (t = -360, i = 270 * samplesPerDegree, j = i + 1; t <= +360; ++t)
		{
			// this is our "threshold" value: 
			// don't want our a3index to be higher than the sample a3index 
			//	that would yield this number when computing sin
			x = (a3real)t / a3real_threesixty;

			// as long as the pre-calculated value is less than table data 
			//	at 'j', continue to increment selector a3index 'i'
			while (*(a3_trigSinSampleTablePtr + j) < x) 
				i = j++;

			// assign index
			*(asinIndexItr++) = i;
			*(acosIndexItr++) = i - (90 * samplesPerDegree);
		}

		// done
		return n;
	}
	return 0;
}

A3_INLINE a3index a3trigInitSetTables(const a3index samplesPerDegree, const a3real table[])
{
	const a3index n_func = (samplesPerDegree * 720) + 1;
	const a3index n = (n_func * 3) + (768 * 2);

	if (samplesPerDegree && table)
	{
		a3_trigSamplesPerDegree = samplesPerDegree;
		a3_trigParamTablePtr = table;
		a3_trigSinSampleTablePtr = a3_trigParamTablePtr + n_func;
		a3_trigCosSampleTablePtr = a3_trigSinSampleTablePtr + n_func;
		a3_asinIndexTable = (a3index *)(a3_trigCosSampleTablePtr + n_func);
		a3_acosIndexTable = a3_asinIndexTable + 768;
		return n;
	}
	return 0;
}

A3_INLINE a3index a3trigInitSamplesRequired(const a3index samplesPerDegree)
{
	const a3index n_func = (samplesPerDegree * 720) + 1;
	const a3index n = (n_func * 3) + (768 * 2);
	return n;
}


// delete data tables
// returns 1 if success, 0 if fail
A3_INLINE a3index a3trigFree()
{
	if (a3_trigSamplesPerDegree && a3_trigParamTablePtr)
	{
		a3_trigSamplesPerDegree = 0;
		a3_trigParamTablePtr = 0;
		a3_trigSinSampleTablePtr = 0;
		a3_trigCosSampleTablePtr = 0;
		return 1;
	}
	return 0;
}


//-----------------------------------------------------------------------------

// validate inputs to the functions below
// sin, cos, tan in degrees: input is [-360, 360]
// sin, cos, tan in radians: input is [-2pi, 2pi]
// asin, acos input is [-1, 1]
// atan input is (-pi/2, pi/2)
A3_INLINE a3real a3trigValid_sind(a3real x)
{
	while (x <= -a3real_threesixty)
		x += a3real_threesixty;
	while (x >= a3real_threesixty)
		x -= a3real_threesixty;
	return x;
}

A3_INLINE a3real a3trigValid_sinr(a3real x)
{
	while (x <= -a3real_twopi)
		x += a3real_twopi;
	while (x >= a3real_twopi)
		x -= a3real_twopi;
	return x;
}

A3_INLINE a3real a3trigValid_asin(a3real x)
{
	x = a3clamp(-a3real_one, +a3real_one, x);
	return x;
}

A3_INLINE a3real a3trigValid_atan(a3real x)
{
	x = a3clamp(-a3real_halfpi, +a3real_halfpi, x);
	return x;
}


//-----------------------------------------------------------------------------

// compute more precise trig functions by using interpolation
// sin, cos, tan assume input is [0, 360] degrees or [0, 2pi] radians, 
//	return value in [-1, 1] for sin, cos and (-inf, inf) for tan
A3_INLINE a3real a3sind(const a3real x)
{
	return a3sampleTableLerpIncrement(
		a3_trigSinSampleTablePtr, a3_trigParamTablePtr, x, 
		((a3integer)x + 360)*a3_trigSamplesPerDegree);
}

A3_INLINE a3real a3cosd(const a3real x)
{
	return a3sampleTableLerpIncrement(
		a3_trigCosSampleTablePtr, a3_trigParamTablePtr, x, 
		((a3integer)x + 360)*a3_trigSamplesPerDegree);
}

A3_INLINE a3real a3tand(const a3real x)
{
	// tan = sin / cos
	a3real t;
	a3index i = a3sampleTableLerpIncrementIndex(
		a3_trigParamTablePtr, x, 
		((a3integer)x + 360)*a3_trigSamplesPerDegree, &t);
	const a3real n1s = *(a3_trigSinSampleTablePtr + i);
	const a3real n1c = *(a3_trigCosSampleTablePtr + i);
	const a3real n0s = *(a3_trigSinSampleTablePtr + --i);
	const a3real n0c = *(a3_trigCosSampleTablePtr + i);
	const a3real s = a3lerp(n0s, n1s, t);
	const a3real c = a3lerp(n0c, n1c, t);
	return (s / c);
}

A3_INLINE a3real a3sinr(const a3real x)
{
	return a3sind(x * a3real_rad2deg);
}

A3_INLINE a3real a3cosr(const a3real x)
{
	return a3cosd(x * a3real_rad2deg);
}

A3_INLINE a3real a3tanr(const a3real x)
{
	return a3tand(x * a3real_rad2deg);
}


// performs inverse-trig functions and returns value in degrees
// arcsin assumes input is [-1, 1] and returns [-90, 90] degrees or [-pi/2, pi/2] radians
// arccos assumes input is [-1, 1] and returns [0, 180] degrees or [0, pi] radians
// arctan assumes input is (-inf, inf) and returns (-90, 90) degrees or (-pi/4, pi/4) radians
A3_INLINE a3real a3asind(const a3real x)
{
	return +a3sampleTableLerpIncrement(
		a3_trigParamTablePtr, a3_trigSinSampleTablePtr, x, 
		*(a3_asinIndexTable + 360 + (a3integer)(x*a3real_threesixty)));
}

A3_INLINE a3real a3acosd(const a3real x)
{
	return -a3sampleTableLerpIncrement(
		a3_trigParamTablePtr, a3_trigCosSampleTablePtr, x,
		*(a3_acosIndexTable + 360 + (a3integer)(x*a3real_threesixty)));
}

A3_INLINE a3real a3atand(const a3real x)
{
	const a3real xx = a3sqrtInverse(x*x + a3real_one);

	// atan as a function of acos: 
	// atan = acos(1 / sqrt(x^2 + 1))
	// does not work in practice because input is always positive
//	return a3acosd(xx);

	// atan as a function of asin: 
	// atan = asin(x / sqrt(x^2 + 1))
	return a3asind(xx * x);
}

A3_INLINE a3real a3asinr(const a3real x)
{
	return a3asind(x)*a3real_deg2rad;
}

A3_INLINE a3real a3acosr(const a3real x)
{
	return a3acosd(x)*a3real_deg2rad;
}

A3_INLINE a3real a3atanr(const a3real x)
{
	return a3atand(x)*a3real_deg2rad;
}


// reciprocal trig functions (degrees and radians): 
// cosecant (csc) is (1 / sine)
// secant (sec) is (1 / cosine)
// cotangent (cot) is (1 / tan) or (cos / sin)
A3_INLINE a3real a3cscd(const a3real x)
{
	return a3real_one / a3sampleTableLerpIncrement(
		a3_trigSinSampleTablePtr, a3_trigParamTablePtr, x,
		((a3integer)x + 360)*a3_trigSamplesPerDegree);
}

A3_INLINE a3real a3secd(const a3real x)
{
	return a3real_one / a3sampleTableLerpIncrement(
		a3_trigCosSampleTablePtr, a3_trigParamTablePtr, x,
		((a3integer)x + 360)*a3_trigSamplesPerDegree);
}

A3_INLINE a3real a3cotd(const a3real x)
{
	// cot = cos / sin
	a3real t;
	a3index i = a3sampleTableLerpIncrementIndex(
		a3_trigParamTablePtr, x,
		((a3integer)x + 360)*a3_trigSamplesPerDegree, &t);
	const a3real n1s = *(a3_trigSinSampleTablePtr + i);
	const a3real n1c = *(a3_trigCosSampleTablePtr + i);
	const a3real n0s = *(a3_trigSinSampleTablePtr + --i);
	const a3real n0c = *(a3_trigCosSampleTablePtr + i);
	const a3real s = a3lerp(n0s, n1s, t);
	const a3real c = a3lerp(n0c, n1c, t);
	return (c / s);
}

A3_INLINE a3real a3cscr(const a3real x)
{
	return a3cscd(x * a3real_rad2deg);
}

A3_INLINE a3real a3secr(const a3real x)
{
	return a3secd(x * a3real_rad2deg);
}

A3_INLINE a3real a3cotr(const a3real x)
{
	return a3cotd(x * a3real_rad2deg);
}

// inverse reciprocal trig functions
// same inputs and outputs as above
A3_INLINE a3real a3acscd(const a3real x)
{
	// sin(acsc(x)) = 1/x
	// acsc(x) = asin(1/x)
	return a3asind(a3recip(x));
}

A3_INLINE a3real a3asecd(const a3real x)
{
	// cos(asec(x)) = 1/x
	// asec(x) = acos(1/x)
	return a3acosd(a3recip(x));
}

A3_INLINE a3real a3acotd(const a3real x)
{
	// tan(acot(x)) = 1/x
	// acot(x) = atan(1/x)
	return a3atand(a3recip(x));
}

A3_INLINE a3real a3acscr(const a3real x)
{
	return a3asinr(a3recip(x));
}

A3_INLINE a3real a3asecr(const a3real x)
{
	return a3acosr(a3recip(x));
}

A3_INLINE a3real a3acotr(const a3real x)
{
	return a3atanr(a3recip(x));
}


// atan2: more precise arc tangent that handles atan using two parameters
// since tan(a) = y/x, breaks down inverse solution depending on y and x
A3_INLINE a3real a3atan2d(const a3real y, const a3real x)
{
	a3real result = a3real_zero;
	if (x > a3real_zero)
		result = a3atand(y / x);
	else if (x < a3real_zero)
		result = a3atand(y / x) + (y >= a3real_zero ? a3real_oneeighty : -a3real_oneeighty);
	else
		if (y > a3real_zero)
			result = a3real_ninety;
		else if (y < a3real_zero)
			result = -a3real_ninety;
	return result;
}

A3_INLINE a3real a3atan2r(const a3real y, const a3real x)
{
	a3real result = a3real_zero;
	if (x > a3real_zero)
		result = a3atanr(y / x);
	else if (x < a3real_zero)
		result = a3atanr(y / x) + (y >= a3real_zero ? a3real_pi : -a3real_pi);
	else
		if (y > a3real_zero)
			result = a3real_halfpi;
		else if (y < a3real_zero)
			result = -a3real_halfpi;
	return result;
}


// perform long sine and cosine using a Taylor series
// this is way more precise but very slow
A3_INLINE a3index a3trigSetTaylorIterations(const a3index n)
{
	const a3index oldItrs = a3_TaylorItrs;
	a3_TaylorItrs = n;
	return oldItrs;
}

A3_INLINE a3index a3trigGetTaylorIterations()
{
	return a3_TaylorItrs;
}


A3_INLINE a3real a3sinrTaylor(const a3real x)
{
//	const a3f128 xx = (a3f128)(x*x);
//	a3f128 x2 = (a3f128)x, sum = x2;
//	a3f128 degree = 1.0, f = degree;
//	a3index i;
//	for (i = 1; i <= a3_TaylorItrs; ++i)
//	{
//		// increment degree twice, negating once 
//		//	to change the sign of the operand
//		f *= -(degree += 1.0);
//		f *= +(degree += 1.0);
//		x2 *= xx;
//		sum += x2 / f;
//	}
//	return (a3real)sum;

	const a3f128 X = (a3f128)x;
	a3f128 x2 = X, ssum = X;
	a3f128 degree = 1.0, f = degree;
	a3index i;
	for (i = 1; i <= a3_TaylorItrs; ++i)
	{
		// do cos accumulation
		f *= -(degree += 1.0);
		x2 *= X;
	//	csum += x2 / f;

		// do sin accumulation
		f *= +(degree += 1.0);
		x2 *= X;
		ssum += x2 / f;
	}
	return (a3real)ssum;
}

A3_INLINE a3real a3cosrTaylor(const a3real x)
{
//	const a3f128 xx = (a3f128)(x*x);
//	a3f128 x2 = 1.0, sum = x2;
//	a3f128 degree = 1.0, f = degree;
//	a3index i;
//	for (i = 0; i < a3_TaylorItrs; ++i)
//	{
//		f *= -(degree += 1.0);
//		x2 *= xx;
//		sum += x2 / f;
//		f *= +(degree += 1.0);
//	}
//	return (a3real)sum;

	const a3f128 X = (a3f128)x;
	a3f128 x2 = X, csum = 1.0;
	a3f128 degree = 1.0, f = degree;
	a3index i;
	for (i = 1; i <= a3_TaylorItrs; ++i)
	{
		// do cos accumulation
		f *= -(degree += 1.0);
		x2 *= X;
		csum += x2 / f;

		// do sin accumulation
		f *= +(degree += 1.0);
		x2 *= X;
	//	ssum += x2 / f;
	}
	return (a3real)csum;
}

A3_INLINE a3real a3tanrTaylor(const a3real x)
{
	// tan = sin / cos
	a3real s[1], c[1];
	a3trigTaylor_sinr_cosr(x, s, c);
	return (*s / *c);
}

A3_INLINE a3real a3sindTaylor(const a3real x)
{
	return a3sinrTaylor(x * a3real_deg2rad);
}

A3_INLINE a3real a3cosdTaylor(const a3real x)
{
	return a3cosrTaylor(x * a3real_deg2rad);
}

A3_INLINE a3real a3tandTaylor(const a3real x)
{
	return a3tanrTaylor(x * a3real_deg2rad);
}


// calculate sin and cos at the same time
A3_INLINE a3real a3trigTaylor_sinr_cosr(const a3real x, a3real *sin_out, a3real *cos_out)
{
	const a3f128 X = (a3f128)x;
	a3f128 x2 = X, ssum = X, csum = 1.0;
	a3f128 degree = 1.0, f = degree;
	a3index i;
	for (i = 1; i <= a3_TaylorItrs; ++i)
	{
		// do cos accumulation
		f *= -(degree += 1.0);
		x2 *= X;
		csum += x2 / f;

		// do sin accumulation
		f *= +(degree += 1.0);
		x2 *= X;
		ssum += x2 / f;
	}

	// copy results to outputs
	*cos_out = (a3real)csum;
	*sin_out = (a3real)ssum;

	// done, return param for reuse
	return x;
}

A3_INLINE a3real a3trigTaylor_sind_cosd(const a3real x, a3real *sin_out, a3real *cos_out)
{
	a3trigTaylor_sinr_cosr((x * a3real_deg2rad), sin_out, cos_out);
	return x;
}


//-----------------------------------------------------------------------------
// other trig-related operations

// compute the error ratio occurring from discrete geometry sampling
// e.g. since a sphere in graphics is not continuous, face centers 
//	are closer to the center than the vertices; calculate the 
//	uniform scale required to expand or contract geometry to match 
//	true continuous radii (application: volume lighting)
// total azimuth should be between [0, 360] degrees
// total elevation should be between [0, 180] degrees
A3_INLINE a3real a3trigPointToEdgeRatio(const a3real totalAzimuth, const a3count numSlices)
{
	const a3real sepAzim = a3divide(totalAzimuth, (a3real)numSlices);
	const a3real ratio = a3cosd(a3real_half * sepAzim);
	return ratio;
}

A3_INLINE a3real a3trigEdgeToPointRatio(const a3real totalAzimuth, const a3count numSlices)
{
	const a3real ratio = a3trigPointToEdgeRatio(totalAzimuth, numSlices);
	return a3recip(ratio);
}

A3_INLINE a3real a3trigPointToFaceRatio(const a3real totalAzimuth, const a3real totalElevation, const a3count numSlices, const a3count numStacks)
{
	const a3real ratioA = a3trigPointToEdgeRatio(totalAzimuth, numSlices);
	const a3real ratioE = a3trigPointToEdgeRatio(totalElevation, numStacks);
	return (ratioA * ratioE);
}

A3_INLINE a3real a3trigFaceToPointRatio(const a3real totalAzimuth, const a3real totalElevation, const a3count numSlices, const a3count numStacks)
{
	const a3real ratio = a3trigPointToFaceRatio(totalAzimuth, totalElevation, numSlices, numStacks);
	return a3recip(ratio);
}


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_TRIG_IMPL_INL
#endif	// __ANIMAL3D_A3DM_TRIG_H