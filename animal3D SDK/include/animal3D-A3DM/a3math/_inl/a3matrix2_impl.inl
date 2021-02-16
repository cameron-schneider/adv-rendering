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

	a3matrix2_impl.inl
	Implementations for 2D matrix functions.
*/

#ifdef __ANIMAL3D_A3DM_MATRIX_H
#ifndef __ANIMAL3D_A3DM_MATRIX2_IMPL_INL
#define __ANIMAL3D_A3DM_MATRIX2_IMPL_INL


#include "../a3trig.h"


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// identity matrix
A3_GLOBAL const a3mat2 a3mat2_identity = { 
	a3real_one, a3real_zero, 
	a3real_zero, a3real_one 
//
//
};


//-----------------------------------------------------------------------------

// create 2x2 matrix
///
A3_INLINE a3real2x2r a3real2x2SetIdentity(a3real2x2p m_out)
{
	m_out[0][0] = m_out[1][1] = a3real_one;
	m_out[0][1] = m_out[1][0] = a3real_zero;
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2Set(a3real2x2p m_out, const a3real x0, const a3real y0, const a3real x1, const a3real y1)
{
	m_out[0][0] = x0;				m_out[1][0] = x1;
	m_out[0][1] = y0;				m_out[1][1] = y1;
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2SetMajors(a3real2x2p m_out, const a3real2p v0, const a3real2p v1)
{
	m_out[0][0] = v0[0];			m_out[1][0] = v1[0];
	m_out[0][1] = v0[1];			m_out[1][1] = v1[1];
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2SetMinors(a3real2x2p m_out, const a3real2p v0, const a3real2p v1)
{
	m_out[0][0] = v0[0];			m_out[1][0] = v0[1];
	m_out[0][1] = v1[0];			m_out[1][1] = v1[1];
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2SetReal2x2(a3real2x2p m_out, const a3real2x2p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2SetReal3x3(a3real2x2p m_out, const a3real3x3p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2SetReal4x4(a3real2x2p m_out, const a3real4x4p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];
	// 
	// 
	return m_out;
}


// unary operations
///
A3_INLINE a3real a3real2x2Determinant(const a3real2x2p m)
{
	const a3real d = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	return d;
}

A3_INLINE a3real a3real2x2DeterminantInverse(const a3real2x2p m)
{
	const a3real d = a3real2x2Determinant(m);
	return a3recip(d);
}

A3_INLINE a3real2x2r a3real2x2GetNegative(a3real2x2p m_out, const a3real2x2p m)
{
	m_out[0][0] = -m[0][0];			m_out[1][0] = -m[1][0];
	m_out[0][1] = -m[0][1];			m_out[1][1] = -m[1][1];
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2GetTransposed(a3real2x2p m_out, const a3real2x2p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[0][1];
	m_out[0][1] = m[1][0];			m_out[1][1] = m[1][1];
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2GetInverse(a3real2x2p m_out, const a3real2x2p m)
{
	const a3real dInv = a3real2x2DeterminantInverse(m);
	m_out[0][0] = dInv * m[1][1];	m_out[1][0] = -dInv * m[1][0];
	m_out[0][1] = -dInv * m[0][1];	m_out[1][1] = dInv * m[0][0];
	return m_out;
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

A3_INLINE a3real2x2r a3real2x2Negate(a3real2x2p m_inout)
{
	m_inout[0][0] = -m_inout[0][0];	m_inout[1][0] = -m_inout[1][0];
	m_inout[0][1] = -m_inout[0][1];	m_inout[1][1] = -m_inout[1][1];
	// 
	// 
	return m_inout;
}

A3_INLINE a3real2x2r a3real2x2Transpose(a3real2x2p m_inout)
{
	a3real tmp;
	a3swap2(tmp, m_inout[0][1], m_inout[1][0]);
	// 
	// 
	// 
	// 
	// 
	return m_inout;
}

A3_INLINE a3real2x2r a3real2x2Invert(a3real2x2p m_inout)
{
	a3real dInv = a3real2x2DeterminantInverse(m_inout);
	m_inout[0][0] *= dInv;	m_inout[1][0] *= -dInv;
	m_inout[0][1] *= -dInv;	m_inout[1][1] *= dInv;
	a3swap2(dInv, m_inout[0][0], m_inout[1][1]);
	return m_inout;
}


// binary operations
///
A3_INLINE a3real2x2r a3real2x2Sum(a3real2x2p m_out, const a3real2x2p mL, const a3real2x2p mR)
{
	m_out[0][0] = mL[0][0] + mR[0][0];	m_out[1][0] = mL[1][0] + mR[1][0];
	m_out[0][1] = mL[0][1] + mR[0][1];	m_out[1][1] = mL[1][1] + mR[1][1];
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2Diff(a3real2x2p m_out, const a3real2x2p mL, const a3real2x2p mR)
{
	m_out[0][0] = mL[0][0] - mR[0][0];	m_out[1][0] = mL[1][0] - mR[1][0];
	m_out[0][1] = mL[0][1] - mR[0][1];	m_out[1][1] = mL[1][1] - mR[1][1];
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2ProductS(a3real2x2p m_out, const a3real2x2p m, const a3real s)
{
	m_out[0][0] = m[0][0] * s;			m_out[1][0] = m[1][0] * s;
	m_out[0][1] = m[0][1] * s;			m_out[1][1] = m[1][1] * s;
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2QuotientS(a3real2x2p m_out, const a3real2x2p m, const a3real s)
{
	const a3real sInv = a3recip(s);
	m_out[0][0] = m[0][0] * sInv;		m_out[1][0] = m[1][0] * sInv;
	m_out[0][1] = m[0][1] * sInv;		m_out[1][1] = m[1][1] * sInv;
	// 
	// 
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2Add(a3real2x2p mL_inout, const a3real2x2p mR)
{
	mL_inout[0][0] += mR[0][0];			mL_inout[1][0] += mR[1][0];
	mL_inout[0][1] += mR[0][1];			mL_inout[1][1] += mR[1][1];
	// 
	// 
	return mL_inout;
}

A3_INLINE a3real2x2r a3real2x2Sub(a3real2x2p mL_inout, const a3real2x2p mR)
{
	mL_inout[0][0] -= mR[0][0];			mL_inout[1][0] -= mR[1][0];
	mL_inout[0][1] -= mR[0][1];			mL_inout[1][1] -= mR[1][1];
	// 
	// 
	return mL_inout;
}

A3_INLINE a3real2x2r a3real2x2MulS(a3real2x2p m_inout, const a3real s)
{
	m_inout[0][0] *= s;					m_inout[1][0] *= s;
	m_inout[0][1] *= s;					m_inout[1][1] *= s;
	// 
	// 
	return m_inout;
}

A3_INLINE a3real2x2r a3real2x2DivS(a3real2x2p m_inout, const a3real s)
{
	const a3real sInv = a3recip(s);
	m_inout[0][0] *= sInv;				m_inout[1][0] *= sInv;
	m_inout[0][1] *= sInv;				m_inout[1][1] *= sInv;
	// 
	// 
	return m_inout;
}


// matrix multiplication with compatible matrix or vector
///
A3_INLINE a3real2r a3real2Real2x2ProductL(a3real2p v_out, const a3real2p v, const a3real2x2p m)
{
	a3real2 tmp;
#ifndef A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[0][1] * v[1];
	tmp[1] = m[1][0] * v[0] + m[1][1] * v[1];
	// 
	// 
#else	// A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[1][0] * v[1];
	tmp[1] = m[0][1] * v[0] + m[1][1] * v[1];
	// 
	// 
#endif	// !A3_MAT_ROWMAJOR
	v_out[0] = tmp[0];
	v_out[1] = tmp[1];
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Real2x2ProductR(a3real2p v_out, const a3real2x2p m, const a3real2p v)
{
	a3real2 tmp;
#ifndef A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[1][0] * v[1];
	tmp[1] = m[0][1] * v[0] + m[1][1] * v[1];
	// 
	// 
#else	// A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[0][1] * v[1];
	tmp[1] = m[1][0] * v[0] + m[1][1] * v[1];
	// 
	// 
#endif	// !A3_MAT_ROWMAJOR
	v_out[0] = tmp[0];
	v_out[1] = tmp[1];
	// 
	// 
	return v_out;
}

A3_INLINE a3real2r a3real2Real2x2MulL(a3real2p v_inout, const a3real2x2p m)
{
	a3real2 tmp;
	a3real2Real2x2ProductL(tmp, v_inout, m);
	a3real2SetReal2(v_inout, tmp);
	return v_inout;
}

A3_INLINE a3real2r a3real2Real2x2MulR(const a3real2x2p m, a3real2p v_inout)
{
	a3real2 tmp;
	a3real2Real2x2ProductR(tmp, m, v_inout);
	a3real2SetReal2(v_inout, tmp);
	return v_inout;
}


A3_INLINE a3real2x2r a3real2x2Product(a3real2x2p m_out, const a3real2x2p mL, const a3real2x2p mR)
{
#ifndef A3_MAT_ROWMAJOR
	a3real2Real2x2Product(m_out[0], mL, mR[0]);
	a3real2Real2x2Product(m_out[1], mL, mR[1]);
	// 
	// 
#else	// A3_MAT_ROWMAJOR
	a3real2Real2x2Product(m_out[0], mR, mL[0]);
	a3real2Real2x2Product(m_out[1], mR, mL[1]);
	// 
	// 
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2ConcatL(a3real2x2p mL_inout, const a3real2x2p mR)
{
#ifndef A3_MAT_ROWMAJOR
	a3real2x2 tmp2;
	a3real2x2Product(tmp2, mL_inout, mR);
	a3real2x2SetReal2x2(mL_inout, tmp2);
#else	// A3_MAT_ROWMAJOR
	a3real2Real2x2Mul(mR, mL_inout[0]);
	a3real2Real2x2Mul(mR, mL_inout[1]);
	// 
	// 
#endif	// !A3_MAT_ROWMAJOR
	return mL_inout;
}

A3_INLINE a3real2x2r a3real2x2ConcatR(const a3real2x2p mL, a3real2x2p mR_inout)
{
#ifndef A3_MAT_ROWMAJOR
	a3real2Real2x2Mul(mL, mR_inout[0]);
	a3real2Real2x2Mul(mL, mR_inout[1]);
	// 
	// 
#else	// A3_MAT_ROWMAJOR
	a3real2x2 tmp2;
	a3real2x2Product(tmp2, mL, mR_inout);
	a3real2x2SetReal2x2(mR_inout, tmp2);
#endif	// !A3_MAT_ROWMAJOR
	return mR_inout;
}


// basic transformations: uniform scale, non-uniform scale, rotate
///
A3_INLINE a3real2x2r a3real2x2SetScale(a3real2x2p m_out, const a3real s)
{
	m_out[0][0] = m_out[1][1] = s;
	m_out[0][1] = m_out[1][0] = a3real_zero;
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2SetNonUnif(a3real2x2p m_out, const a3real sx, const a3real sy)
{
	m_out[0][0] = sx;				m_out[1][1] = sy;
	m_out[0][1] = m_out[1][0] = a3real_zero;
	return m_out;
}

A3_INLINE a3real2x2r a3real2x2SetRotate(a3real2x2p m_out, const a3real degrees)
{
	const a3real c = a3cosd(degrees);
	const a3real s = a3sind(degrees);
	m_out[0][0] = m_out[1][1] = c;
#ifndef A3_MAT_ROWMAJOR
	m_out[1][0] = -s;
	m_out[0][1] = s;
#else	// A3_MAT_ROWMAJOR
	m_out[1][0] = s;
	m_out[0][1] = -s;
#endif	// !A3_MAT_ROWMAJOR
	// 
	// 
	return m_out;
}

// no more rotations here
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


#endif	// !__ANIMAL3D_A3DM_MATRIX2_IMPL_INL
#endif	// __ANIMAL3D_A3DM_MATRIX_H