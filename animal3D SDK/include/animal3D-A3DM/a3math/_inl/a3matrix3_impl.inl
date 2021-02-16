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

	a3matrix3_impl.inl
	Implementations for 3D matrix functions.
*/

#ifdef __ANIMAL3D_A3DM_MATRIX_H
#ifndef __ANIMAL3D_A3DM_MATRIX3_IMPL_INL
#define __ANIMAL3D_A3DM_MATRIX3_IMPL_INL


#include "../a3trig.h"


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// identity matrix
A3_GLOBAL const a3mat3 a3mat3_identity = { 
	a3real_one, a3real_zero, a3real_zero  A3PAD0, 
	a3real_zero, a3real_one, a3real_zero  A3PAD0, 
	a3real_zero, a3real_zero, a3real_one  A3PAD0 
//
};


//-----------------------------------------------------------------------------

// create 3x3 matrix
///
A3_INLINE a3real3x3r a3real3x3SetIdentity(a3real3x3p m_out)
{
	m_out[0][0] = m_out[1][1] = m_out[2][2] = a3real_one;
	m_out[0][1] = m_out[0][2] = m_out[1][0] = m_out[1][2] = m_out[2][0] = m_out[2][1] = a3real_zero;
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3Set(a3real3x3p m_out, const a3real x0, const a3real y0, const a3real z0, const a3real x1, const a3real y1, const a3real z1, const a3real x2, const a3real y2, const a3real z2)
{
	m_out[0][0] = x0;				m_out[1][0] = x1;				m_out[2][0] = x2;
	m_out[0][1] = y0;				m_out[1][1] = y1;				m_out[2][1] = y2;
	m_out[0][2] = z0;				m_out[1][2] = z1;				m_out[2][2] = z2;
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetMajors(a3real3x3p m_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	m_out[0][0] = v0[0];			m_out[1][0] = v1[0];			m_out[2][0] = v2[0];
	m_out[0][1] = v0[1];			m_out[1][1] = v1[1];			m_out[2][1] = v2[1];
	m_out[0][2] = v0[2];			m_out[1][2] = v1[2];			m_out[2][2] = v2[2];
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetMinors(a3real3x3p m_out, const a3real3p v0, const a3real3p v1, const a3real3p v2)
{
	m_out[0][0] = v0[0];			m_out[1][0] = v0[1];			m_out[2][0] = v0[2];
	m_out[0][1] = v1[0];			m_out[1][1] = v1[1];			m_out[2][1] = v1[2];
	m_out[0][2] = v2[0];			m_out[1][2] = v2[1];			m_out[2][2] = v2[2];
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetReal2x2(a3real3x3p m_out, const a3real2x2p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];			m_out[2][0] = a3real_zero;
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];			m_out[2][1] = a3real_zero;
	m_out[0][2] = a3real_zero;		m_out[1][2] = a3real_zero;		m_out[2][2] = a3real_one;
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetReal3x3(a3real3x3p m_out, const a3real3x3p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];			m_out[2][0] = m[2][0];
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];			m_out[2][1] = m[2][1];
	m_out[0][2] = m[0][2];			m_out[1][2] = m[1][2];			m_out[2][2] = m[2][2];
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetReal4x4(a3real3x3p m_out, const a3real4x4p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];			m_out[2][0] = m[2][0];
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];			m_out[2][1] = m[2][1];
	m_out[0][2] = m[0][2];			m_out[1][2] = m[1][2];			m_out[2][2] = m[2][2];
	// 
	return m_out;
}


// unary operations
///
A3_INLINE a3real a3real3x3Determinant(const a3real3x3p m)
{
	const a3real d = (m[0][0] * m[1][1] * m[2][2] + m[1][0] * m[2][1] * m[0][2] + m[2][0] * m[0][1] * m[1][2]) - (m[2][0] * m[1][1] * m[0][2] + m[1][0] * m[0][1] * m[2][2] + m[0][0] * m[2][1] * m[1][2]);
	return d;
}

A3_INLINE a3real a3real3x3DeterminantInverse(const a3real3x3p m)
{
	const a3real d = a3real3x3Determinant(m);
	return a3recip(d);
}

A3_INLINE a3real3x3r a3real3x3GetNegative(a3real3x3p m_out, const a3real3x3p m)
{
	m_out[0][0] = -m[0][0];			m_out[1][0] = -m[1][0];			m_out[2][0] = -m[2][0];
	m_out[0][1] = -m[0][1];			m_out[1][1] = -m[1][1];			m_out[2][1] = -m[2][1];
	m_out[0][2] = -m[0][2];			m_out[1][2] = -m[1][2];			m_out[2][2] = -m[2][2];
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3GetTransposed(a3real3x3p m_out, const a3real3x3p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[0][1];			m_out[2][0] = m[0][2];
	m_out[0][1] = m[1][0];			m_out[1][1] = m[1][1];			m_out[2][1] = m[1][2];
	m_out[0][2] = m[2][0];			m_out[1][2] = m[2][1];			m_out[2][2] = m[2][2];
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3GetInverse(a3real3x3p m_out, const a3real3x3p m)
{
	// do cross products and transpose simultaneously
	const a3real dInv = a3real3x3DeterminantInverse(m);
	a3real3x3Set(m_out, 
		dInv * (m[1][1] * m[2][2] - m[1][2] * m[2][1]), 
		dInv * (m[2][1] * m[0][2] - m[2][2] * m[0][1]), 
		dInv * (m[0][1] * m[1][2] - m[0][2] * m[1][1]), 
		dInv * (m[1][2] * m[2][0] - m[1][0] * m[2][2]), 
		dInv * (m[2][2] * m[0][0] - m[2][0] * m[0][2]), 
		dInv * (m[0][2] * m[1][0] - m[0][0] * m[1][2]), 
		dInv * (m[1][0] * m[2][1] - m[1][1] * m[2][0]), 
		dInv * (m[2][0] * m[0][1] - m[2][1] * m[0][0]), 
		dInv * (m[0][0] * m[1][1] - m[0][1] * m[1][0])
	);
	return m_out;
}
//
//
//
//
//

A3_INLINE a3real3x3r a3real3x3Negate(a3real3x3p m_inout)
{
	m_inout[0][0] = -m_inout[0][0];	m_inout[1][0] = -m_inout[1][0];	m_inout[2][0] = -m_inout[2][0];
	m_inout[0][1] = -m_inout[0][1];	m_inout[1][1] = -m_inout[1][1];	m_inout[2][1] = -m_inout[2][1];
	m_inout[0][2] = -m_inout[0][2];	m_inout[1][2] = -m_inout[1][2];	m_inout[2][2] = -m_inout[2][2];
	// 
	return m_inout;
}

A3_INLINE a3real3x3r a3real3x3Transpose(a3real3x3p m_inout)
{
	a3real tmp;
	a3swap2(tmp, m_inout[0][1], m_inout[1][0]);
	a3swap2(tmp, m_inout[0][2], m_inout[2][0]);
	a3swap2(tmp, m_inout[1][2], m_inout[2][1]);
	// 
	// 
	// 
	return m_inout;
}

A3_INLINE a3real3x3r a3real3x3Invert(a3real3x3p m_inout)
{
	a3real3x3 tmp;
	a3real3x3GetInverse(tmp, m_inout);
	a3real3x3SetReal3x3(m_inout, tmp);
	return m_inout;
}
//


// binary operations
///
A3_INLINE a3real3x3r a3real3x3Sum(a3real3x3p m_out, const a3real3x3p mL, const a3real3x3p mR)
{
	m_out[0][0] = mL[0][0] + mR[0][0];	m_out[1][0] = mL[1][0] + mR[1][0];	m_out[2][0] = mL[2][0] + mR[2][0];
	m_out[0][1] = mL[0][1] + mR[0][1];	m_out[1][1] = mL[1][1] + mR[1][1];	m_out[2][1] = mL[2][1] + mR[2][1];
	m_out[0][2] = mL[0][2] + mR[0][2];	m_out[1][2] = mL[1][2] + mR[1][2];	m_out[2][2] = mL[2][2] + mR[2][2];
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3Diff(a3real3x3p m_out, const a3real3x3p mL, const a3real3x3p mR)
{
	m_out[0][0] = mL[0][0] - mR[0][0];	m_out[1][0] = mL[1][0] - mR[1][0];	m_out[2][0] = mL[2][0] - mR[2][0];
	m_out[0][1] = mL[0][1] - mR[0][1];	m_out[1][1] = mL[1][1] - mR[1][1];	m_out[2][1] = mL[2][1] - mR[2][1];
	m_out[0][2] = mL[0][2] - mR[0][2];	m_out[1][2] = mL[1][2] - mR[1][2];	m_out[2][2] = mL[2][2] - mR[2][2];
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3ProductS(a3real3x3p m_out, const a3real3x3p m, const a3real s)
{
	m_out[0][0] = m[0][0] * s;			m_out[1][0] = m[1][0] * s;			m_out[2][0] = m[2][0] * s;
	m_out[0][1] = m[0][1] * s;			m_out[1][1] = m[1][1] * s;			m_out[2][1] = m[2][1] * s;
	m_out[0][2] = m[0][2] * s;			m_out[1][2] = m[1][2] * s;			m_out[2][2] = m[2][2] * s;
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3QuotientS(a3real3x3p m_out, const a3real3x3p m, const a3real s)
{
	const a3real sInv = a3recip(s);
	m_out[0][0] = m[0][0] * sInv;		m_out[1][0] = m[1][0] * sInv;		m_out[2][0] = m[2][0] * sInv;
	m_out[0][1] = m[0][1] * sInv;		m_out[1][1] = m[1][1] * sInv;		m_out[2][1] = m[2][1] * sInv;
	m_out[0][2] = m[0][2] * sInv;		m_out[1][2] = m[1][2] * sInv;		m_out[2][2] = m[2][2] * sInv;
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3Add(a3real3x3p mL_inout, const a3real3x3p mR)
{
	mL_inout[0][0] += mR[0][0];			mL_inout[1][0] += mR[1][0];			mL_inout[2][0] += mR[2][0];
	mL_inout[0][1] += mR[0][1];			mL_inout[1][1] += mR[1][1];			mL_inout[2][1] += mR[2][1];
	mL_inout[0][2] += mR[0][2];			mL_inout[1][2] += mR[1][2];			mL_inout[2][2] += mR[2][2];
	// 
	return mL_inout;
}

A3_INLINE a3real3x3r a3real3x3Sub(a3real3x3p mL_inout, const a3real3x3p mR)
{
	mL_inout[0][0] -= mR[0][0];			mL_inout[1][0] -= mR[1][0];			mL_inout[2][0] -= mR[2][0];
	mL_inout[0][1] -= mR[0][1];			mL_inout[1][1] -= mR[1][1];			mL_inout[2][1] -= mR[2][1];
	mL_inout[0][2] -= mR[0][2];			mL_inout[1][2] -= mR[1][2];			mL_inout[2][2] -= mR[2][2];
	// 
	return mL_inout;
}

A3_INLINE a3real3x3r a3real3x3MulS(a3real3x3p m_inout, const a3real s)
{
	m_inout[0][0] *= s;					m_inout[1][0] *= s;					m_inout[2][0] *= s;
	m_inout[0][1] *= s;					m_inout[1][1] *= s;					m_inout[2][1] *= s;
	m_inout[0][2] *= s;					m_inout[1][2] *= s;					m_inout[2][2] *= s;
	// 
	return m_inout;
}

A3_INLINE a3real3x3r a3real3x3DivS(a3real3x3p m_inout, const a3real s)
{
	const a3real sInv = a3recip(s);
	m_inout[0][0] *= sInv;				m_inout[1][0] *= sInv;				m_inout[2][0] *= sInv;
	m_inout[0][1] *= sInv;				m_inout[1][1] *= sInv;				m_inout[2][1] *= sInv;
	m_inout[0][2] *= sInv;				m_inout[1][2] *= sInv;				m_inout[2][2] *= sInv;
	// 
	return m_inout;
}


// matrix multiplication with compatible matrix or vector
///
A3_INLINE a3real3r a3real3Real3x3ProductL(a3real3p v_out, const a3real3p v, const a3real3x3p m)
{
	a3real3 tmp;
#ifndef A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
	tmp[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
	tmp[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
	// 
#else	// A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2];
	tmp[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2];
	tmp[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2];
	// 
#endif	// !A3_MAT_ROWMAJOR
	v_out[0] = tmp[0];
	v_out[1] = tmp[1];
	v_out[2] = tmp[2];
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Real3x3ProductR(a3real3p v_out, const a3real3x3p m, const a3real3p v)
{
	a3real3 tmp;
#ifndef A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2];
	tmp[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2];
	tmp[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2];
	// 
#else	// A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
	tmp[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2];
	tmp[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
	// 
#endif	// !A3_MAT_ROWMAJOR
	v_out[0] = tmp[0];
	v_out[1] = tmp[1];
	v_out[2] = tmp[2];
	// 
	return v_out;
}

A3_INLINE a3real3r a3real3Real3x3MulL(a3real3p v_inout, const a3real3x3p m)
{
	a3real3 tmp;
	a3real3Real3x3ProductL(tmp, v_inout, m);
	a3real3SetReal3(v_inout, tmp);
	return v_inout;
}

A3_INLINE a3real3r a3real3Real3x3MulR(const a3real3x3p m, a3real3p v_inout)
{
	a3real3 tmp;
	a3real3Real3x3ProductR(tmp, m, v_inout);
	a3real3SetReal3(v_inout, tmp);
	return v_inout;
}


A3_INLINE a3real3x3r a3real3x3Product(a3real3x3p m_out, const a3real3x3p mL, const a3real3x3p mR)
{
#ifndef A3_MAT_ROWMAJOR
	a3real3Real3x3Product(m_out[0], mL, mR[0]);
	a3real3Real3x3Product(m_out[1], mL, mR[1]);
	a3real3Real3x3Product(m_out[2], mL, mR[2]);
	// 
#else	// A3_MAT_ROWMAJOR
	a3real3Real3x3Product(m_out[0], mR, mL[0]);
	a3real3Real3x3Product(m_out[1], mR, mL[1]);
	a3real3Real3x3Product(m_out[2], mR, mL[2]);
	// 
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3ConcatL(a3real3x3p mL_inout, const a3real3x3p mR)
{
#ifndef A3_MAT_ROWMAJOR
	a3real3x3 tmp2;
	a3real3x3Product(tmp2, mL_inout, mR);
	a3real3x3SetReal3x3(mL_inout, tmp2);
#else	// A3_MAT_ROWMAJOR
	a3real3Real3x3Mul(mR, mL_inout[0]);
	a3real3Real3x3Mul(mR, mL_inout[1]);
	a3real3Real3x3Mul(mR, mL_inout[2]);
	// 
#endif	// !A3_MAT_ROWMAJOR
	return mL_inout;
}

A3_INLINE a3real3x3r a3real3x3ConcatR(const a3real3x3p mL, a3real3x3p mR_inout)
{
#ifndef A3_MAT_ROWMAJOR
	a3real3Real3x3Mul(mL, mR_inout[0]);
	a3real3Real3x3Mul(mL, mR_inout[1]);
	a3real3Real3x3Mul(mL, mR_inout[2]);
	// 
#else	// A3_MAT_ROWMAJOR
	a3real3x3 tmp2;
	a3real3x3Product(tmp2, mL, mR_inout);
	a3real3x3SetReal3x3(mR_inout, tmp2);
#endif	// !A3_MAT_ROWMAJOR
	return mR_inout;
}


// basic transformations
///
A3_INLINE a3real3x3r a3real3x3SetScale(a3real3x3p m_out, const a3real s)
{
	m_out[0][0] = m_out[1][1] = m_out[2][2] = s;
	m_out[0][1] = m_out[0][2] = m_out[1][0] = m_out[1][2] = m_out[2][0] = m_out[2][1] = a3real_zero;
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetNonUnif(a3real3x3p m_out, const a3real sx, const a3real sy, const a3real sz)
{
	m_out[0][0] = sx;				m_out[1][1] = sy;				m_out[2][2] = sz;
	m_out[0][1] = m_out[0][2] = m_out[1][0] = m_out[1][2] = m_out[2][0] = m_out[2][1] = a3real_zero;
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetRotateX(a3real3x3p m_out, const a3real degrees)
{
	const a3real c = a3cosd(degrees);
	const a3real s = a3sind(degrees);
	m_out[1][1] = m_out[2][2] = c;
	m_out[0][1] = m_out[0][2] = m_out[1][0] = m_out[2][0] = a3real_zero;
	m_out[0][0] = a3real_one;
#ifndef A3_MAT_ROWMAJOR
	m_out[2][1] = -s;
	m_out[1][2] = s;
#else	// A3_MAT_ROWMAJOR
	m_out[2][1] = s;
	m_out[1][2] = -s;
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetRotateY(a3real3x3p m_out, const a3real degrees)
{
	const a3real c = a3cosd(degrees);
	const a3real s = a3sind(degrees);
	m_out[0][0] = m_out[2][2] = c;
	m_out[0][1] = m_out[1][0] = m_out[1][2] = m_out[2][1] = a3real_zero;
	m_out[1][1] = a3real_one;
#ifndef A3_MAT_ROWMAJOR
	m_out[0][2] = -s;
	m_out[2][0] = s;
#else	// A3_MAT_ROWMAJOR
	m_out[0][2] = s;
	m_out[2][0] = -s;
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetRotateZ(a3real3x3p m_out, const a3real degrees)
{
	const a3real c = a3cosd(degrees);
	const a3real s = a3sind(degrees);
	m_out[0][0] = m_out[1][1] = c;
	m_out[0][2] = m_out[1][2] = m_out[2][0] = m_out[2][1] = a3real_zero;
	m_out[2][2] = a3real_one;
#ifndef A3_MAT_ROWMAJOR
	m_out[1][0] = -s;
	m_out[0][1] = s;
#else	// A3_MAT_ROWMAJOR
	m_out[1][0] = s;
	m_out[0][1] = -s;
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetRotateXYZ(a3real3x3p m_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z)
{
	const a3real
		cx = a3cosd(degrees_x),
		cy = a3cosd(degrees_y),
		cz = a3cosd(degrees_z),
		sx = a3sind(degrees_x),
		sy = a3sind(degrees_y),
		sz = a3sind(degrees_z);

#ifndef A3_MAT_ROWMAJOR
	m_out[0][0] = cy*cz;			m_out[1][0] = -cy*sz;			m_out[2][0] = sy;
	m_out[0][1] = cx*sz + cz*sx*sy;	m_out[1][1] = cx*cz - sx*sy*sz;	m_out[2][1] = -cy*sx;
	m_out[0][2] = sx*sz - cx*cz*sy;	m_out[1][2] = cz*sx + cx*sy*sz;	m_out[2][2] = cx*cy;
#else	// A3_MAT_ROWMAJOR
	m_out[0][0] = cy*cz;			m_out[0][1] = -cy*sz;			m_out[0][2] = sy;
	m_out[1][0] = cx*sz + cz*sx*sy;	m_out[1][1] = cx*cz - sx*sy*sz;	m_out[1][2] = -cy*sx;
	m_out[2][0] = sx*sz - cx*cz*sy;	m_out[2][1] = cz*sx + cx*sy*sz;	m_out[2][2] = cx*cy;
#endif	// !A3_MAT_ROWMAJOR
	// 
	// 
	return m_out;
}

A3_INLINE a3real3x3r a3real3x3SetRotateZYX(a3real3x3p m_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z)
{
	const a3real
		cx = a3cosd(degrees_x),
		cy = a3cosd(degrees_y),
		cz = a3cosd(degrees_z),
		sx = a3sind(degrees_x),
		sy = a3sind(degrees_y),
		sz = a3sind(degrees_z);

#ifndef A3_MAT_ROWMAJOR
	m_out[0][0] = cz*cy;	m_out[1][0] = cz*sy*sx - cx*sz;	m_out[2][0] = sz*sx + cz*cx*sy;
	m_out[0][1] = cy*sz;	m_out[1][1] = cz*cx + sz*sy*sx;	m_out[2][1] = cx*sz*sy - cz*sx;
	m_out[0][2] = -sy;		m_out[1][2] = cy*sx;			m_out[2][2] = cy*cx;
#else	// A3_MAT_ROWMAJOR
	m_out[0][0] = cz*cy;	m_out[0][1] = cz*sy*sx - cx*sz;	m_out[0][2] = sz*sx + cz*cx*sy;
	m_out[1][0] = cy*sz;	m_out[1][1] = cz*cx + sz*sy*sx;	m_out[1][2] = cx*sz*sy - cz*sx;
	m_out[2][0] = -sy;		m_out[2][1] = cy*sx;			m_out[2][2] = cy*cx;
#endif	// !A3_MAT_ROWMAJOR
	// 
	// 
	return m_out;
}

A3_INLINE a3real3x3rk a3real3x3GetEulerXYZIgnoreScale(const a3real3x3p m, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out)
{
#ifndef A3_MAT_ROWMAJOR
	*degrees_x_out = a3atan2d(-m[2][1], m[2][2]);
	*degrees_y_out = a3asind(m[2][0]);
	*degrees_z_out = a3atan2d(-m[1][0], m[0][0]);
#else	// A3_MAT_ROWMAJOR
	*degrees_x_out = a3atan2d(-m[1][2], m[2][2]);
	*degrees_y_out = a3asind(m[0][2]);
	*degrees_z_out = a3atan2d(-m[0][1], m[0][0]);
#endif	// !A3_MAT_ROWMAJOR
	return m;
}

A3_INLINE a3real3x3rk a3real3x3GetEulerZYXIgnoreScale(const a3real3x3p m, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out)
{
#ifndef A3_MAT_ROWMAJOR
	*degrees_x_out = a3atan2d(m[1][2], m[2][2]);
	*degrees_y_out = a3asind(-m[0][2]);
	*degrees_z_out = a3atan2d(m[0][1], m[0][0]);
#else	// A3_MAT_ROWMAJOR
	*degrees_x_out = a3atan2d(m[2][1], m[2][2]);
	*degrees_y_out = a3asind(-m[2][0]);
	*degrees_z_out = a3atan2d(m[1][0], m[0][0]);
#endif	// !A3_MAT_ROWMAJOR
	return m;
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
// 
// 
// 


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_MATRIX3_IMPL_INL
#endif	// __ANIMAL3D_A3DM_MATRIX_H