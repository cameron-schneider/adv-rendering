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

	a3matrix4_impl.inl
	Implementations for 4D matrix functions.
*/

#ifdef __ANIMAL3D_A3DM_MATRIX_H
#ifndef __ANIMAL3D_A3DM_MATRIX4_IMPL_INL
#define __ANIMAL3D_A3DM_MATRIX4_IMPL_INL


#include "../a3trig.h"


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// identity matrix
A3_GLOBAL const a3mat4 a3mat4_identity = { 
	a3real_one, a3real_zero, a3real_zero, a3real_zero, 
	a3real_zero, a3real_one, a3real_zero, a3real_zero, 
	a3real_zero, a3real_zero, a3real_one, a3real_zero, 
	a3real_zero, a3real_zero, a3real_zero, a3real_one 
};


//-----------------------------------------------------------------------------

// create 4x4 matrix
///
A3_INLINE a3real4x4r a3real4x4SetIdentity(a3real4x4p m_out)
{
	m_out[0][0] = m_out[1][1] = m_out[2][2] = m_out[3][3] = a3real_one;
	m_out[0][1] = m_out[0][2] = m_out[0][3] = m_out[1][0] = m_out[1][2] = m_out[1][3] = m_out[2][0] = m_out[2][1] = m_out[2][3] = m_out[3][0] = m_out[3][1] = m_out[3][2] = a3real_zero;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4Set(a3real4x4p m_out, const a3real x0, const a3real y0, const a3real z0, const a3real w0, const a3real x1, const a3real y1, const a3real z1, const a3real w1, const a3real x2, const a3real y2, const a3real z2, const a3real w2, const a3real x3, const a3real y3, const a3real z3, const a3real w3)
{
	m_out[0][0] = x0;				m_out[1][0] = x1;				m_out[2][0] = x2;				m_out[3][0] = x3;
	m_out[0][1] = y0;				m_out[1][1] = y1;				m_out[2][1] = y2;				m_out[3][1] = y3;
	m_out[0][2] = z0;				m_out[1][2] = z1;				m_out[2][2] = z2;				m_out[3][2] = z3;
	m_out[0][3] = w0;				m_out[1][3] = w1;				m_out[2][3] = w2;				m_out[3][3] = w3;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetMajors(a3real4x4p m_out, const a3real4p v0, const a3real4p v1, const a3real4p v2, const a3real4p v3)
{
	m_out[0][0] = v0[0];			m_out[1][0] = v1[0];			m_out[2][0] = v2[0];			m_out[3][0] = v3[0];
	m_out[0][1] = v0[1];			m_out[1][1] = v1[1];			m_out[2][1] = v2[1];			m_out[3][1] = v3[1];
	m_out[0][2] = v0[2];			m_out[1][2] = v1[2];			m_out[2][2] = v2[2];			m_out[3][2] = v3[2];
	m_out[0][3] = v0[3];			m_out[1][3] = v1[3];			m_out[2][3] = v2[3];			m_out[3][3] = v3[3];
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetMinors(a3real4x4p m_out, const a3real4p v0, const a3real4p v1, const a3real4p v2, const a3real4p v3)
{
	m_out[0][0] = v0[0];			m_out[1][0] = v0[1];			m_out[2][0] = v0[2];			m_out[3][0] = v0[3];
	m_out[0][1] = v1[0];			m_out[1][1] = v1[1];			m_out[2][1] = v1[2];			m_out[3][1] = v1[3];
	m_out[0][2] = v2[0];			m_out[1][2] = v2[1];			m_out[2][2] = v2[2];			m_out[3][2] = v2[3];
	m_out[0][3] = v3[0];			m_out[1][3] = v3[1];			m_out[2][3] = v3[2];			m_out[3][3] = v3[3];
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetReal2x2(a3real4x4p m_out, const a3real2x2p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];			m_out[2][0] = a3real_zero;			m_out[3][0] = a3real_zero;
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];			m_out[2][1] = a3real_zero;			m_out[3][1] = a3real_zero;
	m_out[0][2] = a3real_zero;		m_out[1][2] = a3real_zero;		m_out[2][2] = a3real_one; 			m_out[3][2] = a3real_zero;
	m_out[0][3] = a3real_zero;		m_out[1][3] = a3real_zero;		m_out[2][3] = a3real_zero;			m_out[3][3] = a3real_one;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetReal3x3(a3real4x4p m_out, const a3real3x3p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];			m_out[2][0] = m[2][0];	 		m_out[3][0] = a3real_zero;
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];			m_out[2][1] = m[2][1];	 		m_out[3][1] = a3real_zero;
	m_out[0][2] = m[0][2];			m_out[1][2] = m[1][2];			m_out[2][2] = m[2][2];	 		m_out[3][2] = a3real_zero;
	m_out[0][3] = a3real_zero;		m_out[1][3] = a3real_zero;		m_out[2][3] = a3real_zero;		m_out[3][3] = a3real_one;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetReal4x4(a3real4x4p m_out, const a3real4x4p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[1][0];			m_out[2][0] = m[2][0];			m_out[3][0] = m[3][0];
	m_out[0][1] = m[0][1];			m_out[1][1] = m[1][1];			m_out[2][1] = m[2][1];			m_out[3][1] = m[3][1];
	m_out[0][2] = m[0][2];			m_out[1][2] = m[1][2];			m_out[2][2] = m[2][2];			m_out[3][2] = m[3][2];
	m_out[0][3] = m[0][3];			m_out[1][3] = m[1][3];			m_out[2][3] = m[2][3];			m_out[3][3] = m[3][3];
	return m_out;
}


// unary operations
///
A3_INLINE a3real a3real4x4Determinant(const a3real4x4p m)
{
	const a3real d = (m[0][0] * m[1][1] * m[2][2] * m[3][3] + m[1][0] * m[2][1] * m[3][2] * m[0][3] + m[2][0] * m[3][1] * m[0][2] * m[1][3] + m[3][0] * m[0][1] * m[1][2] * m[2][3]) - (m[3][0] * m[2][1] * m[1][2] * m[0][3] + m[2][0] * m[1][1] * m[0][2] * m[3][3] + m[1][0] * m[0][1] * m[3][2] * m[2][3] + m[0][0] * m[3][1] * m[2][2] * m[1][3]);
	return d;
}

A3_INLINE a3real a3real4x4DeterminantInverse(const a3real4x4p m)
{
	const a3real d = a3real4x4Determinant(m);
	return a3recip(d);
}

A3_INLINE a3real4x4r a3real4x4GetNegative(a3real4x4p m_out, const a3real4x4p m)
{
	m_out[0][0] = -m[0][0];			m_out[1][0] = -m[1][0];			m_out[2][0] = -m[2][0];			m_out[3][0] = -m[3][0];
	m_out[0][1] = -m[0][1];			m_out[1][1] = -m[1][1];			m_out[2][1] = -m[2][1];			m_out[3][1] = -m[3][1];
	m_out[0][2] = -m[0][2];			m_out[1][2] = -m[1][2];			m_out[2][2] = -m[2][2];			m_out[3][2] = -m[3][2];
	m_out[0][3] = -m[0][3];			m_out[1][3] = -m[1][3];			m_out[2][3] = -m[2][3];			m_out[3][3] = -m[3][3];
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4GetTransposed(a3real4x4p m_out, const a3real4x4p m)
{
	m_out[0][0] = m[0][0];			m_out[1][0] = m[0][1];			m_out[2][0] = m[0][2];			m_out[3][0] = m[0][3];
	m_out[0][1] = m[1][0];			m_out[1][1] = m[1][1];			m_out[2][1] = m[1][2];			m_out[3][1] = m[1][3];
	m_out[0][2] = m[2][0];			m_out[1][2] = m[2][1];			m_out[2][2] = m[2][2];			m_out[3][2] = m[2][3];
	m_out[0][3] = m[3][0];			m_out[1][3] = m[3][1];			m_out[2][3] = m[3][2];			m_out[3][3] = m[3][3];
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4GetInverse(a3real4x4p m_out, const a3real4x4p m)
{
	const a3real dInv = a3real4x4DeterminantInverse(m);
	a3real4x4Set(m_out,
		dInv*((m[1][1] * m[2][2] * m[3][3] + m[2][1] * m[3][2] * m[1][3] + m[3][1] * m[1][2] * m[2][3]) - (m[1][1] * m[3][2] * m[2][3] + m[2][1] * m[1][2] * m[3][3] + m[3][1] * m[2][2] * m[1][3])),
		dInv*((m[0][1] * m[3][2] * m[2][3] + m[2][1] * m[0][2] * m[3][3] + m[3][1] * m[2][2] * m[0][3]) - (m[0][1] * m[2][2] * m[3][3] + m[2][1] * m[3][2] * m[0][3] + m[3][1] * m[0][2] * m[2][3])),
		dInv*((m[0][1] * m[1][2] * m[3][3] + m[1][1] * m[3][2] * m[0][3] + m[3][1] * m[0][2] * m[1][3]) - (m[0][1] * m[3][2] * m[1][3] + m[1][1] * m[0][2] * m[3][3] + m[3][1] * m[1][2] * m[0][3])),
		dInv*((m[0][1] * m[2][2] * m[1][3] + m[1][1] * m[0][2] * m[2][3] + m[2][1] * m[1][2] * m[0][3]) - (m[0][1] * m[1][2] * m[2][3] + m[1][1] * m[2][2] * m[0][3] + m[2][1] * m[0][2] * m[1][3])),
		dInv*((m[1][0] * m[3][2] * m[2][3] + m[2][0] * m[1][2] * m[3][3] + m[3][0] * m[2][2] * m[1][3]) - (m[1][0] * m[2][2] * m[3][3] + m[2][0] * m[3][2] * m[1][3] + m[3][0] * m[1][2] * m[2][3])),
		dInv*((m[0][0] * m[2][2] * m[3][3] + m[2][0] * m[3][2] * m[0][3] + m[3][0] * m[0][2] * m[2][3]) - (m[0][0] * m[3][2] * m[2][3] + m[2][0] * m[0][2] * m[3][3] + m[3][0] * m[2][2] * m[0][3])),
		dInv*((m[0][0] * m[3][2] * m[1][3] + m[1][0] * m[0][2] * m[3][3] + m[3][0] * m[1][2] * m[0][3]) - (m[0][0] * m[1][2] * m[3][3] + m[1][0] * m[3][2] * m[0][3] + m[3][0] * m[0][2] * m[1][3])),
		dInv*((m[0][0] * m[1][2] * m[2][3] + m[1][0] * m[2][2] * m[0][3] + m[2][0] * m[0][2] * m[1][3]) - (m[0][0] * m[2][2] * m[1][3] + m[1][0] * m[0][2] * m[2][3] + m[2][0] * m[1][2] * m[0][3])),
		dInv*((m[1][0] * m[2][1] * m[3][3] + m[2][0] * m[3][1] * m[1][3] + m[3][0] * m[1][1] * m[2][3]) - (m[1][0] * m[3][1] * m[2][3] + m[2][0] * m[1][1] * m[3][3] + m[3][0] * m[2][1] * m[1][3])),
		dInv*((m[0][0] * m[3][1] * m[2][3] + m[2][0] * m[0][1] * m[3][3] + m[3][0] * m[2][1] * m[0][3]) - (m[0][0] * m[2][1] * m[3][3] + m[2][0] * m[3][1] * m[0][3] + m[3][0] * m[0][1] * m[2][3])),
		dInv*((m[0][0] * m[1][1] * m[3][3] + m[1][0] * m[3][1] * m[0][3] + m[3][0] * m[0][1] * m[1][3]) - (m[0][0] * m[3][1] * m[1][3] + m[1][0] * m[0][1] * m[3][3] + m[3][0] * m[1][1] * m[0][3])),
		dInv*((m[0][0] * m[2][1] * m[1][3] + m[1][0] * m[0][1] * m[2][3] + m[2][0] * m[1][1] * m[0][3]) - (m[0][0] * m[1][1] * m[2][3] + m[1][0] * m[2][1] * m[0][3] + m[2][0] * m[0][1] * m[1][3])),
		dInv*((m[1][0] * m[3][1] * m[2][2] + m[2][0] * m[1][1] * m[3][2] + m[3][0] * m[2][1] * m[1][2]) - (m[1][0] * m[2][1] * m[3][2] + m[2][0] * m[3][1] * m[1][2] + m[3][0] * m[1][1] * m[2][2])),
		dInv*((m[0][0] * m[2][1] * m[3][2] + m[2][0] * m[3][1] * m[0][2] + m[3][0] * m[0][1] * m[2][2]) - (m[0][0] * m[3][1] * m[2][2] + m[2][0] * m[0][1] * m[3][2] + m[3][0] * m[2][1] * m[0][2])),
		dInv*((m[0][0] * m[3][1] * m[1][2] + m[1][0] * m[0][1] * m[3][2] + m[3][0] * m[1][1] * m[0][2]) - (m[0][0] * m[1][1] * m[3][2] + m[1][0] * m[3][1] * m[0][2] + m[3][0] * m[0][1] * m[1][2])),
		dInv*((m[0][0] * m[1][1] * m[2][2] + m[1][0] * m[2][1] * m[0][2] + m[2][0] * m[0][1] * m[1][2]) - (m[0][0] * m[2][1] * m[1][2] + m[1][0] * m[0][1] * m[2][2] + m[2][0] * m[1][1] * m[0][2])));
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4Negate(a3real4x4p m_inout)
{
	m_inout[0][0] = -m_inout[0][0];	m_inout[1][0] = -m_inout[1][0];	m_inout[2][0] = -m_inout[2][0];	m_inout[3][0] = -m_inout[3][0];
	m_inout[0][1] = -m_inout[0][1];	m_inout[1][1] = -m_inout[1][1];	m_inout[2][1] = -m_inout[2][1];	m_inout[3][1] = -m_inout[3][1];
	m_inout[0][2] = -m_inout[0][2];	m_inout[1][2] = -m_inout[1][2];	m_inout[2][2] = -m_inout[2][2];	m_inout[3][2] = -m_inout[3][2];
	m_inout[0][3] = -m_inout[0][3];	m_inout[1][3] = -m_inout[1][3];	m_inout[2][3] = -m_inout[2][3];	m_inout[3][3] = -m_inout[3][3];
	return m_inout;
}

A3_INLINE a3real4x4r a3real4x4Transpose(a3real4x4p m_inout)
{
	a3real tmp;
	a3swap2(tmp, m_inout[0][1], m_inout[1][0]);
	a3swap2(tmp, m_inout[0][2], m_inout[2][0]);
	a3swap2(tmp, m_inout[0][3], m_inout[3][0]);
	a3swap2(tmp, m_inout[1][2], m_inout[2][1]);
	a3swap2(tmp, m_inout[1][3], m_inout[3][1]);
	a3swap2(tmp, m_inout[2][3], m_inout[3][2]);
	return m_inout;
}

A3_INLINE a3real4x4r a3real4x4Invert(a3real4x4p m_inout)
{
	a3real4x4 tmp;
	a3real4x4GetInverse(tmp, m_inout);
	a3real4x4SetReal4x4(m_inout, tmp);
	return m_inout;
}
//


// binary operations
///
A3_INLINE a3real4x4r a3real4x4Sum(a3real4x4p m_out, const a3real4x4p mL, const a3real4x4p mR)
{
	m_out[0][0] = mL[0][0] + mR[0][0];	m_out[1][0] = mL[1][0] + mR[1][0];	m_out[2][0] = mL[2][0] + mR[2][0];	m_out[3][0] = mL[3][0] + mR[3][0];
	m_out[0][1] = mL[0][1] + mR[0][1];	m_out[1][1] = mL[1][1] + mR[1][1];	m_out[2][1] = mL[2][1] + mR[2][1];	m_out[3][1] = mL[3][1] + mR[3][1];
	m_out[0][2] = mL[0][2] + mR[0][2];	m_out[1][2] = mL[1][2] + mR[1][2];	m_out[2][2] = mL[2][2] + mR[2][2];	m_out[3][2] = mL[3][2] + mR[3][2];
	m_out[0][3] = mL[0][3] + mR[0][3];	m_out[1][3] = mL[1][3] + mR[1][3];	m_out[2][3] = mL[2][3] + mR[2][3];	m_out[3][3] = mL[3][3] + mR[3][3];
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4Diff(a3real4x4p m_out, const a3real4x4p mL, const a3real4x4p mR)
{
	m_out[0][0] = mL[0][0] - mR[0][0];	m_out[1][0] = mL[1][0] - mR[1][0];	m_out[2][0] = mL[2][0] - mR[2][0];	m_out[3][0] = mL[3][0] - mR[3][0];
	m_out[0][1] = mL[0][1] - mR[0][1];	m_out[1][1] = mL[1][1] - mR[1][1];	m_out[2][1] = mL[2][1] - mR[2][1];	m_out[3][1] = mL[3][1] - mR[3][1];
	m_out[0][2] = mL[0][2] - mR[0][2];	m_out[1][2] = mL[1][2] - mR[1][2];	m_out[2][2] = mL[2][2] - mR[2][2];	m_out[3][2] = mL[3][2] - mR[3][2];
	m_out[0][3] = mL[0][3] - mR[0][3];	m_out[1][3] = mL[1][3] - mR[1][3];	m_out[2][3] = mL[2][3] - mR[2][3];	m_out[3][3] = mL[3][3] - mR[3][3];
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4ProductS(a3real4x4p m_out, const a3real4x4p m, const a3real s)
{
	m_out[0][0] = m[0][0] * s;			m_out[1][0] = m[1][0] * s;			m_out[2][0] = m[2][0] * s;			m_out[3][0] = m[3][0] * s;
	m_out[0][1] = m[0][1] * s;			m_out[1][1] = m[1][1] * s;			m_out[2][1] = m[2][1] * s;			m_out[3][1] = m[3][1] * s;
	m_out[0][2] = m[0][2] * s;			m_out[1][2] = m[1][2] * s;			m_out[2][2] = m[2][2] * s;			m_out[3][2] = m[3][2] * s;
	m_out[0][3] = m[0][3] * s;			m_out[1][3] = m[1][3] * s;			m_out[2][3] = m[2][3] * s;			m_out[3][3] = m[3][3] * s;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4QuotientS(a3real4x4p m_out, const a3real4x4p m, const a3real s)
{
	const a3real sInv = a3recip(s);
	m_out[0][0] = m[0][0] * sInv;		m_out[1][0] = m[1][0] * sInv;		m_out[2][0] = m[2][0] * sInv;		m_out[3][0] = m[3][0] * sInv;
	m_out[0][1] = m[0][1] * sInv;		m_out[1][1] = m[1][1] * sInv;		m_out[2][1] = m[2][1] * sInv;		m_out[3][1] = m[3][1] * sInv;
	m_out[0][2] = m[0][2] * sInv;		m_out[1][2] = m[1][2] * sInv;		m_out[2][2] = m[2][2] * sInv;		m_out[3][2] = m[3][2] * sInv;
	m_out[0][3] = m[0][3] * sInv;		m_out[1][3] = m[1][3] * sInv;		m_out[2][3] = m[2][3] * sInv;		m_out[3][3] = m[3][3] * sInv;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4Add(a3real4x4p mL_inout, const a3real4x4p mR)
{
	mL_inout[0][0] += mR[0][0];			mL_inout[1][0] += mR[1][0];			mL_inout[2][0] += mR[2][0];			mL_inout[3][0] += mR[3][0];
	mL_inout[0][1] += mR[0][1];			mL_inout[1][1] += mR[1][1];			mL_inout[2][1] += mR[2][1];			mL_inout[3][1] += mR[3][1];
	mL_inout[0][2] += mR[0][2];			mL_inout[1][2] += mR[1][2];			mL_inout[2][2] += mR[2][2];			mL_inout[3][2] += mR[3][2];
	mL_inout[0][3] += mR[0][3];			mL_inout[1][3] += mR[1][3];			mL_inout[2][3] += mR[2][3];			mL_inout[3][3] += mR[3][3];
	return mL_inout;
}

A3_INLINE a3real4x4r a3real4x4Sub(a3real4x4p mL_inout, const a3real4x4p mR)
{
	mL_inout[0][0] -= mR[0][0];			mL_inout[1][0] -= mR[1][0];			mL_inout[2][0] -= mR[2][0];			mL_inout[3][0] -= mR[3][0];
	mL_inout[0][1] -= mR[0][1];			mL_inout[1][1] -= mR[1][1];			mL_inout[2][1] -= mR[2][1];			mL_inout[3][1] -= mR[3][1];
	mL_inout[0][2] -= mR[0][2];			mL_inout[1][2] -= mR[1][2];			mL_inout[2][2] -= mR[2][2];			mL_inout[3][2] -= mR[3][2];
	mL_inout[0][3] -= mR[0][3];			mL_inout[1][3] -= mR[1][3];			mL_inout[2][3] -= mR[2][3];			mL_inout[3][3] -= mR[3][3];
	return mL_inout;
}

A3_INLINE a3real4x4r a3real4x4MulS(a3real4x4p m_inout, const a3real s)
{
	m_inout[0][0] *= s;					m_inout[1][0] *= s;					m_inout[2][0] *= s;					m_inout[3][0] *= s;
	m_inout[0][1] *= s;					m_inout[1][1] *= s;					m_inout[2][1] *= s;					m_inout[3][1] *= s;
	m_inout[0][2] *= s;					m_inout[1][2] *= s;					m_inout[2][2] *= s;					m_inout[3][2] *= s;
	m_inout[0][3] *= s;					m_inout[1][3] *= s;					m_inout[2][3] *= s;					m_inout[3][3] *= s;
	return m_inout;
}

A3_INLINE a3real4x4r a3real4x4DivS(a3real4x4p m_inout, const a3real s)
{
	const a3real sInv = a3recip(s);
	m_inout[0][0] *= sInv;				m_inout[1][0] *= sInv;				m_inout[2][0] *= sInv;				m_inout[3][0] *= sInv;
	m_inout[0][1] *= sInv;				m_inout[1][1] *= sInv;				m_inout[2][1] *= sInv;				m_inout[3][1] *= sInv;
	m_inout[0][2] *= sInv;				m_inout[1][2] *= sInv;				m_inout[2][2] *= sInv;				m_inout[3][2] *= sInv;
	m_inout[0][3] *= sInv;				m_inout[1][3] *= sInv;				m_inout[2][3] *= sInv;				m_inout[3][3] *= sInv;
	return m_inout;
}


// matrix multiplication with compatible matrix or vector
///
A3_INLINE a3real4r a3real4Real4x4ProductL(a3real4p v_out, const a3real4p v, const a3real4x4p m)
{
	a3real4 tmp;
#ifndef A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3];
	tmp[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3];
	tmp[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3];
	tmp[3] = m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3];
#else	// A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	tmp[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	tmp[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	tmp[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
#endif	// !A3_MAT_ROWMAJOR
	v_out[0] = tmp[0];
	v_out[1] = tmp[1];
	v_out[2] = tmp[2];
	v_out[3] = tmp[3];
	return v_out;
}

A3_INLINE a3real4r a3real4Real4x4ProductR(a3real4p v_out, const a3real4x4p m, const a3real4p v)
{
	a3real4 tmp;
#ifndef A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	tmp[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	tmp[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	tmp[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
#else	// A3_MAT_ROWMAJOR
	tmp[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3];
	tmp[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3];
	tmp[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3];
	tmp[3] = m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3];
#endif	// !A3_MAT_ROWMAJOR
	v_out[0] = tmp[0];
	v_out[1] = tmp[1];
	v_out[2] = tmp[2];
	v_out[3] = tmp[3];
	return v_out;
}

A3_INLINE a3real4r a3real4Real4x4MulL(a3real4p v_inout, const a3real4x4p m)
{
	a3real4 tmp;
	a3real4Real4x4ProductL(tmp, v_inout, m);
	a3real4SetReal4(v_inout, tmp);
	return v_inout;
}

A3_INLINE a3real4r a3real4Real4x4MulR(const a3real4x4p m, a3real4p v_inout)
{
	a3real4 tmp;
	a3real4Real4x4ProductR(tmp, m, v_inout);
	a3real4SetReal4(v_inout, tmp);
	return v_inout;
}


A3_INLINE a3real4x4r a3real4x4Product(a3real4x4p m_out, const a3real4x4p mL, const a3real4x4p mR)
{
#ifndef A3_MAT_ROWMAJOR
	a3real4Real4x4Product(m_out[0], mL, mR[0]);
	a3real4Real4x4Product(m_out[1], mL, mR[1]);
	a3real4Real4x4Product(m_out[2], mL, mR[2]);
	a3real4Real4x4Product(m_out[3], mL, mR[3]);
#else	// A3_MAT_ROWMAJOR
	a3real4Real4x4Product(m_out[0], mR, mL[0]);
	a3real4Real4x4Product(m_out[1], mR, mL[1]);
	a3real4Real4x4Product(m_out[2], mR, mL[2]);
	a3real4Real4x4Product(m_out[3], mR, mL[3]);
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4ConcatL(a3real4x4p mL_inout, const a3real4x4p mR)
{
#ifndef A3_MAT_ROWMAJOR
	a3real4x4 tmp2;
	a3real4x4Product(tmp2, mL_inout, mR);
	a3real4x4SetReal4x4(mL_inout, tmp2);
#else	// A3_MAT_ROWMAJOR
	a3real4Real4x4Mul(mR, mL_inout[0]);
	a3real4Real4x4Mul(mR, mL_inout[1]);
	a3real4Real4x4Mul(mR, mL_inout[2]);
	a3real4Real4x4Mul(mR, mL_inout[3]);
#endif	// !A3_MAT_ROWMAJOR
	return mL_inout;
}

A3_INLINE a3real4x4r a3real4x4ConcatR(const a3real4x4p mL, a3real4x4p mR_inout)
{
#ifndef A3_MAT_ROWMAJOR
	a3real4Real4x4Mul(mL, mR_inout[0]);
	a3real4Real4x4Mul(mL, mR_inout[1]);
	a3real4Real4x4Mul(mL, mR_inout[2]);
	a3real4Real4x4Mul(mL, mR_inout[3]);
#else	// A3_MAT_ROWMAJOR
	a3real4x4 tmp2;
	a3real4x4Product(tmp2, mL, mR_inout);
	a3real4x4SetReal4x4(mR_inout, tmp2);
#endif	// !A3_MAT_ROWMAJOR
	return mR_inout;
}


// basic transformations
///
A3_INLINE a3real4x4r a3real4x4SetScale(a3real4x4p m_out, const a3real s)
{
	m_out[0][0] = m_out[1][1] = m_out[2][2] = s;	m_out[3][3] = a3real_one;
	m_out[0][1] = m_out[0][2] = m_out[0][3] = m_out[1][0] = m_out[1][2] = m_out[1][3] = m_out[2][0] = m_out[2][1] = m_out[2][3] = m_out[3][0] = m_out[3][1] = m_out[3][2] = a3real_zero;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetNonUnif(a3real4x4p m_out, const a3real sx, const a3real sy, const a3real sz)
{
	m_out[0][0] = sx;				m_out[1][1] = sy;				m_out[2][2] = sz;				m_out[3][3] = a3real_one;
	m_out[0][1] = m_out[0][2] = m_out[0][3] = m_out[1][0] = m_out[1][2] = m_out[1][3] = m_out[2][0] = m_out[2][1] = m_out[2][3] = m_out[3][0] = m_out[3][1] = m_out[3][2] = a3real_zero;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetRotateX(a3real4x4p m_out, const a3real degrees)
{
	const a3real c = a3cosd(degrees);
	const a3real s = a3sind(degrees);
	m_out[1][1] = m_out[2][2] = c;
	m_out[0][1] = m_out[0][2] = m_out[0][3] = m_out[1][0] = m_out[1][3] = m_out[2][0] = m_out[2][3] = m_out[3][0] = m_out[3][1] = m_out[3][2] = a3real_zero;
	m_out[0][0] = m_out[3][3] = a3real_one;
#ifndef A3_MAT_ROWMAJOR
	m_out[2][1] = -s;
	m_out[1][2] = s;
#else	// A3_MAT_ROWMAJOR
	m_out[2][1] = s;
	m_out[1][2] = -s;
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetRotateY(a3real4x4p m_out, const a3real degrees)
{
	const a3real c = a3cosd(degrees);
	const a3real s = a3sind(degrees);
	m_out[0][0] = m_out[2][2] = c;
	m_out[0][1] = m_out[0][3] = m_out[1][0] = m_out[1][2] = m_out[1][3] = m_out[2][1] = m_out[2][3] = m_out[3][0] = m_out[3][1] = m_out[3][2] = a3real_zero;
	m_out[1][1] = m_out[3][3] = a3real_one;
#ifndef A3_MAT_ROWMAJOR
	m_out[0][2] = -s;
	m_out[2][0] = s;
#else	// A3_MAT_ROWMAJOR
	m_out[0][2] = s;
	m_out[2][0] = -s;
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetRotateZ(a3real4x4p m_out, const a3real degrees)
{
	const a3real c = a3cosd(degrees);
	const a3real s = a3sind(degrees);
	m_out[0][0] = m_out[1][1] = c;
	m_out[0][2] = m_out[0][3] = m_out[1][2] = m_out[1][3] = m_out[2][0] = m_out[2][1] = m_out[2][3] = m_out[3][0] = m_out[3][1] = m_out[3][2] = a3real_zero;
	m_out[2][2] = m_out[3][3] = a3real_one;
#ifndef A3_MAT_ROWMAJOR
	m_out[1][0] = -s;
	m_out[0][1] = s;
#else	// A3_MAT_ROWMAJOR
	m_out[1][0] = s;
	m_out[0][1] = -s;
#endif	// !A3_MAT_ROWMAJOR
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetRotateXYZ(a3real4x4p m_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z)
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
	m_out[0][3] = m_out[1][3] = m_out[2][3] = m_out[3][2] = m_out[3][1] = m_out[3][0] = a3real_zero;
	m_out[3][3] = a3real_one;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4SetRotateZYX(a3real4x4p m_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z)
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
	m_out[0][3] = m_out[1][3] = m_out[2][3] = m_out[3][2] = m_out[3][1] = m_out[3][0] = a3real_zero;
	m_out[3][3] = a3real_one;
	return m_out;
}

A3_INLINE a3real4x4rk a3real4x4GetEulerXYZIgnoreScale(const a3real4x4p m, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out)
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

A3_INLINE a3real4x4rk a3real4x4GetEulerZYXIgnoreScale(const a3real4x4p m, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out)
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

A3_INLINE a3real4x4rk a3real4x4GetEulerXYZTranslateIgnoreScale(const a3real4x4p m, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out, a3real3p translate_out)
{
#ifndef A3_MAT_ROWMAJOR
	*degrees_x_out = a3atan2d(-m[2][1], m[2][2]);
	*degrees_y_out = a3asind(m[2][0]);
	*degrees_z_out = a3atan2d(-m[1][0], m[0][0]);
	a3real3Set(translate_out, m[3][0], m[3][1], m[3][2]);
#else	// A3_MAT_ROWMAJOR
	*degrees_x_out = a3atan2d(-m[1][2], m[2][2]);
	*degrees_y_out = a3asind(m[0][2]);
	*degrees_z_out = a3atan2d(-m[0][1], m[0][0]);
	a3real3Set(translate_out, m[0][3], m[1][3], m[2][3]);
#endif	// !A3_MAT_ROWMAJOR
	return m;
}

A3_INLINE a3real4x4rk a3real4x4GetEulerZYXTranslateIgnoreScale(const a3real4x4p m, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out, a3real3p translate_out)
{
#ifndef A3_MAT_ROWMAJOR
	*degrees_x_out = a3atan2d(m[1][2], m[2][2]);
	*degrees_y_out = a3asind(-m[0][2]);
	*degrees_z_out = a3atan2d(m[0][1], m[0][0]);
	a3real3Set(translate_out, m[3][0], m[3][1], m[3][2]);
#else	// A3_MAT_ROWMAJOR
	*degrees_x_out = a3atan2d(m[2][1], m[2][2]);
	*degrees_y_out = a3asind(-m[2][0]);
	*degrees_z_out = a3atan2d(m[1][0], m[0][0]);
	a3real3Set(translate_out, m[0][3], m[1][3], m[2][3]);
#endif	// !A3_MAT_ROWMAJOR
	return m;
}


//-----------------------------------------------------------------------------
// utilities for matrices; extends matrix functionality

// concatenate/multiply/transform vectors as 4x3 (faster than full 4x4)
///
A3_INLINE a3real4r a3real4ProductTransform(a3real4p v_out, const a3real4p v, const a3real4x4p m)
{
#ifndef A3_MAT_ROWMAJOR
	const a3real x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3];
	const a3real y = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3];
	const a3real z = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3];
#else	// A3_MAT_ROWMAJOR
	const a3real x = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	const a3real y = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	const a3real z = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
#endif	// !A3_MAT_ROWMAJOR
	v_out[0] = x;
	v_out[1] = y;
	v_out[2] = z;
	v_out[3] = v[3];
	return v_out;
}

A3_INLINE a3real4r a3real4TransformProduct(a3real4p v_out, const a3real4x4p m, const a3real4p v)
{
#ifndef A3_MAT_ROWMAJOR
	const a3real x = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	const a3real y = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	const a3real z = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
#else	// A3_MAT_ROWMAJOR
	const a3real x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3];
	const a3real y = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3];
	const a3real z = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3];
#endif	// !A3_MAT_ROWMAJOR
	v_out[0] = x;
	v_out[1] = y;
	v_out[2] = z;
	v_out[3] = v[3];
	return v_out;
}

A3_INLINE a3real4r a3real4MulTransform(a3real4p v_inout, const a3real4x4p m)
{
	a3real4 tmp;
	a3real4ProductTransform(tmp, v_inout, m);
	a3real4SetReal4(v_inout, tmp);
	return v_inout;
}

A3_INLINE a3real4r a3real4TransformMul(const a3real4x4p m, a3real4p v_inout)
{
	a3real4 tmp;
	a3real4TransformProduct(tmp, m, v_inout);
	a3real4SetReal4(v_inout, tmp);
	return v_inout;
}

A3_INLINE a3real4x4r a3real4x4ProductTransform(a3real4x4p m_out, const a3real4x4p mL, const a3real4x4p mR)
{
	a3real4TransformProduct(m_out[0], mL, mR[0]);
	a3real4TransformProduct(m_out[1], mL, mR[1]);
	a3real4TransformProduct(m_out[2], mL, mR[2]);
	a3real4TransformProduct(m_out[3], mL, mR[3]);
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4MulTransform(a3real4x4p mL_inout, const a3real4x4p mR)
{
#ifndef A3_MAT_ROWMAJOR
	a3real4x4 tmp2;
	a3real4x4ProductTransform(tmp2, mL_inout, mR);
	a3real4x4SetReal4x4(mL_inout, tmp2);
#else	// A3_MAT_ROWMAJOR
	a3real4x4 tmp2;
	a3real4x4ProductTransform(tmp2, mL_inout, mR);
	a3real4x4SetReal4x4(mL_inout, tmp2);
#endif	// !A3_MAT_ROWMAJOR
	return mL_inout;
}

A3_INLINE a3real4x4r a3real4x4TransformMul(const a3real4x4p mL, a3real4x4p mR_inout)
{
#ifndef A3_MAT_ROWMAJOR
	a3real4x4 tmp2;
	a3real4x4ProductTransform(tmp2, mL, mR_inout);
	a3real4x4SetReal4x4(mR_inout, tmp2);
#else	// A3_MAT_ROWMAJOR
	a3real4x4 tmp2;
	a3real4x4ProductTransform(tmp2, mL, mR_inout);
	a3real4x4SetReal4x4(mR_inout, tmp2);
#endif	// !A3_MAT_ROWMAJOR
	return mR_inout;
}


// get inverse as 4x3 (faster than full 4x4)
///
A3_INLINE a3real4x4r a3real4x4TransformInverse(a3real4x4p m_out, const a3real4x4p m)
{
	// divide each part by its own squared length, then transpose
	// translation part is negated and multiplied by result
	const a3real sx2 = a3real3LengthSquaredInverse(m[0]);
	const a3real sy2 = a3real3LengthSquaredInverse(m[1]);
	const a3real sz2 = a3real3LengthSquaredInverse(m[2]);

	m_out[0][0] = m[0][0] * sx2;	m_out[1][0] = m[0][1] * sx2;	m_out[2][0] = m[0][2] * sx2;
	m_out[0][1] = m[1][0] * sy2;	m_out[1][1] = m[1][1] * sy2;	m_out[2][1] = m[1][2] * sy2;
	m_out[0][2] = m[2][0] * sz2;	m_out[1][2] = m[2][1] * sz2;	m_out[2][2] = m[2][2] * sz2;

	m_out[3][0] = -(m_out[0][0] * m[3][0] + m_out[1][0] * m[3][1] + m_out[2][0] * m[3][2]);
	m_out[3][1] = -(m_out[0][1] * m[3][0] + m_out[1][1] * m[3][1] + m_out[2][1] * m[3][2]);
	m_out[3][2] = -(m_out[0][2] * m[3][0] + m_out[1][2] * m[3][1] + m_out[2][2] * m[3][2]);

	m_out[0][3] = m_out[1][3] = m_out[2][3] = a3real_zero;
	m_out[3][3] = a3real_one;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4TransformInverseIgnoreScale(a3real4x4p m_out, const a3real4x4p m)
{
	// similar to above but no scaling
	m_out[0][0] = m[0][0];			m_out[1][0] = m[0][1];			m_out[2][0] = m[0][2];
	m_out[0][1] = m[1][0];			m_out[1][1] = m[1][1];			m_out[2][1] = m[1][2];
	m_out[0][2] = m[2][0];			m_out[1][2] = m[2][1];			m_out[2][2] = m[2][2];

	m_out[3][0] = -(m_out[0][0] * m[3][0] + m_out[1][0] * m[3][1] + m_out[2][0] * m[3][2]);
	m_out[3][1] = -(m_out[0][1] * m[3][0] + m_out[1][1] * m[3][1] + m_out[2][1] * m[3][2]);
	m_out[3][2] = -(m_out[0][2] * m[3][0] + m_out[1][2] * m[3][1] + m_out[2][2] * m[3][2]);

	m_out[0][3] = m_out[1][3] = m_out[2][3] = a3real_zero;
	m_out[3][3] = a3real_one;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4TransformInverseUniformScale(a3real4x4p m_out, const a3real4x4p m)
{
	// similar but only one scalar
	const a3real s2 = a3real3LengthSquaredInverse(m[0]);

	m_out[0][0] = m[0][0] * s2;		m_out[1][0] = m[0][1] * s2;		m_out[2][0] = m[0][2] * s2;
	m_out[0][1] = m[1][0] * s2;		m_out[1][1] = m[1][1] * s2;		m_out[2][1] = m[1][2] * s2;
	m_out[0][2] = m[2][0] * s2;		m_out[1][2] = m[2][1] * s2;		m_out[2][2] = m[2][2] * s2;

	m_out[3][0] = -(m_out[0][0] * m[3][0] + m_out[1][0] * m[3][1] + m_out[2][0] * m[3][2]);
	m_out[3][1] = -(m_out[0][1] * m[3][0] + m_out[1][1] * m[3][1] + m_out[2][1] * m[3][2]);
	m_out[3][2] = -(m_out[0][2] * m[3][0] + m_out[1][2] * m[3][1] + m_out[2][2] * m[3][2]);

	m_out[0][3] = m_out[1][3] = m_out[2][3] = a3real_zero;
	m_out[3][3] = a3real_one;
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4TransformInvert(a3real4x4p m_inout)
{
	const a3real sx2 = a3real3LengthSquaredInverse(m_inout[0]);
	const a3real sy2 = a3real3LengthSquaredInverse(m_inout[1]);
	const a3real sz2 = a3real3LengthSquaredInverse(m_inout[2]);
	a3real tmp;
	a3real3 tmp2;

	m_inout[0][0] *= sx2;	m_inout[1][0] *= sy2;	m_inout[2][0] *= sz2;
	m_inout[0][1] *= sx2;	m_inout[1][1] *= sy2;	m_inout[2][1] *= sz2;
	m_inout[0][2] *= sx2;	m_inout[1][2] *= sy2;	m_inout[2][2] *= sz2;
	a3swap2(tmp, m_inout[0][1], m_inout[1][0]);
	a3swap2(tmp, m_inout[0][2], m_inout[2][0]);
	a3swap2(tmp, m_inout[1][2], m_inout[2][1]);

	tmp2[0] = m_inout[0][0] * m_inout[3][0] + m_inout[1][0] * m_inout[3][1] + m_inout[2][0] * m_inout[3][2];
	tmp2[1] = m_inout[0][1] * m_inout[3][0] + m_inout[1][1] * m_inout[3][1] + m_inout[2][1] * m_inout[3][2];
	tmp2[2] = m_inout[0][2] * m_inout[3][0] + m_inout[1][2] * m_inout[3][1] + m_inout[2][2] * m_inout[3][2];
	a3real3GetNegative(m_inout[3], tmp2);
	return m_inout;
}

A3_INLINE a3real4x4r a3real4x4TransformInvertIgnoreScale(a3real4x4p m_inout)
{
	a3real tmp;
	a3real3 tmp2;

	a3swap2(tmp, m_inout[0][1], m_inout[1][0]);
	a3swap2(tmp, m_inout[0][2], m_inout[2][0]);
	a3swap2(tmp, m_inout[1][2], m_inout[2][1]);

	tmp2[0] = m_inout[0][0] * m_inout[3][0] + m_inout[1][0] * m_inout[3][1] + m_inout[2][0] * m_inout[3][2];
	tmp2[1] = m_inout[0][1] * m_inout[3][0] + m_inout[1][1] * m_inout[3][1] + m_inout[2][1] * m_inout[3][2];
	tmp2[2] = m_inout[0][2] * m_inout[3][0] + m_inout[1][2] * m_inout[3][1] + m_inout[2][2] * m_inout[3][2];
	a3real3GetNegative(m_inout[3], tmp2);
	return m_inout;
}

A3_INLINE a3real4x4r a3real4x4TransformInvertUniformScale(a3real4x4p m_inout)
{
	const a3real s2 = a3real3LengthSquaredInverse(m_inout[0]);
	a3real tmp;
	a3real3 tmp2;

	m_inout[0][0] *= s2;	m_inout[1][0] *= s2;	m_inout[2][0] *= s2;
	m_inout[0][1] *= s2;	m_inout[1][1] *= s2;	m_inout[2][1] *= s2;
	m_inout[0][2] *= s2;	m_inout[1][2] *= s2;	m_inout[2][2] *= s2;
	a3swap2(tmp, m_inout[0][1], m_inout[1][0]);
	a3swap2(tmp, m_inout[0][2], m_inout[2][0]);
	a3swap2(tmp, m_inout[1][2], m_inout[2][1]);

	tmp2[0] = m_inout[0][0] * m_inout[3][0] + m_inout[1][0] * m_inout[3][1] + m_inout[2][0] * m_inout[3][2];
	tmp2[1] = m_inout[0][1] * m_inout[3][0] + m_inout[1][1] * m_inout[3][1] + m_inout[2][1] * m_inout[3][2];
	tmp2[2] = m_inout[0][2] * m_inout[3][0] + m_inout[1][2] * m_inout[3][1] + m_inout[2][2] * m_inout[3][2];
	a3real3GetNegative(m_inout[3], tmp2);
	return m_inout;
}


// prepare a "look-at" matrix given "center" and "target"
// eyePos and targetPos cannot be the same, and worldUpVec should not be zero
// optional parameter for outputting inverse
///
A3_INLINE a3real3x3r a3real3x3MakeLookAt(a3real3x3p m_out, a3real3x3p mInv_out_opt, const a3real3p eyePos, const a3real3p targetPos, const a3real3p worldUpVec)
{
	a3real3Diff(m_out[2], eyePos, targetPos);
	a3real3Normalize(m_out[2]);
	a3real3CrossUnit(m_out[0], worldUpVec, m_out[2]);
	a3real3Cross(m_out[1], m_out[2], m_out[0]);

	if (mInv_out_opt)
		a3real3x3GetTransposed(mInv_out_opt, m_out);
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4MakeLookAt(a3real4x4p m_out, a3real4x4p mInv_out_opt, const a3real3p eyePos, const a3real3p targetPos, const a3real3p worldUpVec)
{
	a3real3Diff(m_out[2], eyePos, targetPos);
	a3real3Normalize(m_out[2]);
	a3real3CrossUnit(m_out[0], worldUpVec, m_out[2]);
	a3real3Cross(m_out[1], m_out[2], m_out[0]);

	a3real4SetReal3W(m_out[3], eyePos, a3real_one);
	m_out[0][3] = m_out[1][3] = m_out[2][3] = a3real_zero;

	if (mInv_out_opt)
		a3real4x4TransformInverseIgnoreScale(mInv_out_opt, m_out);
	return m_out;
}


// perspective projection matrix using vertical field of view (fovyDegrees) or planes
// fovyDegrees (vertical field of view) must be between zero and 180 degrees, exclusive
// aspect represents the aspect ratio of the viewport, cannot be zero
// near must be greater than zero, and far must be greater than near
// for planes, right must be greater than left and top must be greater than bottom
// optional parameter for outputting inverse
///
A3_INLINE a3real4x4r a3real4x4MakePerspectiveProjection(a3real4x4p m_out, a3real4x4p mInv_out_opt, const a3real fovyDegrees, const a3real aspect, const a3real nearDist, const a3real farDist)
{
	if (fovyDegrees > a3real_zero && fovyDegrees < a3real_oneeighty && nearDist > a3real_zero && farDist > nearDist)
	{
		// top of near plane is calculated using tan(theta/2)
		// store half angle then use it to calculate top
		// t = n*tan(a/2)
		// r = t*aspect
		// y1 = n/t = n/(n*tan(a/2)) = 1/tan(a/2) = cot(a/2)
		// x0 = n/r = n/(t*aspect) = (n/t)/aspect = y1/aspect
		const a3real halfAngle = a3real_half*fovyDegrees;
		const a3real farToNearInv = a3real_one / (nearDist - farDist);
		m_out[1][1] = a3cotd(halfAngle);
		m_out[0][0] = m_out[1][1] / aspect;
		m_out[2][2] = (nearDist + farDist)*farToNearInv;
		m_out[2][3] = -a3real_one;
		m_out[3][2] = a3real_two*nearDist*farDist*farToNearInv;

		// the rest are zero
		m_out[0][1] = m_out[0][2] = m_out[0][3]
			= m_out[1][0] = m_out[1][2] = m_out[1][3]
			= m_out[3][0] = m_out[3][1] = m_out[3][3]
			= m_out[2][0] = m_out[2][1] = a3real_zero;

		// inverse
		if (mInv_out_opt)
		{
			// t = n*tan(a/2) = n*sin(a/2)/cos(a/2)
			// r = t*aspect
			// y1 = t/n = (n*tan(a/2))/n = tan(a/2)
			// x0 = r/n = (t*aspect)/n = aspect*(t/n) = y1*aspect
			const a3real doubleNearFarInv = a3real_half / (nearDist*farDist);
			mInv_out_opt[1][1] = a3real_one / m_out[1][1];
			mInv_out_opt[0][0] = mInv_out_opt[1][1] * aspect;
			mInv_out_opt[2][3] = (nearDist - farDist)*doubleNearFarInv;
			mInv_out_opt[3][2] = -a3real_one;
			mInv_out_opt[3][3] = (nearDist + farDist)*doubleNearFarInv;

			// zeros
			mInv_out_opt[0][1] = mInv_out_opt[0][2] = mInv_out_opt[0][3]
				= mInv_out_opt[1][0] = mInv_out_opt[1][2] = mInv_out_opt[1][3]
				= mInv_out_opt[2][0] = mInv_out_opt[2][1] = mInv_out_opt[2][2]
				= mInv_out_opt[3][0] = mInv_out_opt[3][1] = a3real_zero;
		}
	}
	else
		a3real4x4SetIdentity(m_out);
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4MakePerspectiveProjectionPlanes(a3real4x4p m_out, a3real4x4p mInv_out_opt, const a3real rightDist, const a3real leftDist, const a3real topDist, const a3real bottomDist, const a3real nearDist, const a3real farDist)
{
	if (rightDist > leftDist && topDist > bottomDist && nearDist > a3real_zero && farDist > nearDist)
	{
		const a3real leftToRightInv = a3real_one / (rightDist - leftDist);
		const a3real bottomToTopInv = a3real_one / (topDist - bottomDist);
		const a3real farToNearInv = a3real_one / (nearDist - farDist);
		const a3real doubleNear = nearDist + nearDist;
		m_out[0][0] = doubleNear*leftToRightInv;
		m_out[1][1] = doubleNear*bottomToTopInv;
		m_out[2][0] = (rightDist + leftDist)*leftToRightInv;
		m_out[2][1] = (topDist + bottomDist)*bottomToTopInv;
		m_out[2][2] = (nearDist + farDist)*farToNearInv;
		m_out[2][3] = -a3real_one;
		m_out[3][2] = doubleNear*farDist*farToNearInv;

		m_out[0][1] = m_out[0][2] = m_out[0][3]
			= m_out[1][0] = m_out[1][2] = m_out[1][3]
			= m_out[3][0] = m_out[3][1] = m_out[3][3] = a3real_zero;

		if (mInv_out_opt)
		{
			const a3real doubleNearInv = a3real_half / nearDist;
			const a3real doubleNearFarInv = a3real_half / (nearDist*farDist);
			mInv_out_opt[3][0] = (rightDist - leftDist)*doubleNearInv;
			mInv_out_opt[3][1] = (topDist - bottomDist)*doubleNearInv;
			mInv_out_opt[2][3] = (nearDist - farDist)*doubleNearFarInv;
			mInv_out_opt[3][0] = (rightDist + leftDist)*doubleNearInv;
			mInv_out_opt[3][1] = (topDist + bottomDist)*doubleNearInv;
			mInv_out_opt[3][2] = -a3real_one;
			mInv_out_opt[3][3] = (nearDist + farDist)*doubleNearFarInv;

			mInv_out_opt[0][1] = mInv_out_opt[0][2] = mInv_out_opt[0][3]
				= mInv_out_opt[1][0] = mInv_out_opt[1][2] = mInv_out_opt[1][3]
				= mInv_out_opt[2][0] = mInv_out_opt[2][1] = mInv_out_opt[2][2] = a3real_zero;
		}
	}
	else
		a3real4x4SetIdentity(m_out);
	return m_out;
}


// orthographic projection matrix using viewport width and height
// right and left cannot be the same (width cannot be zero)
// top and bottom cannot be the same (height cannot be zero)
// near and far cannot be the same
// optional parameter for outputting inverse
///
A3_INLINE a3real4x4r a3real4x4MakeOrthographicProjection(a3real4x4p m_out, a3real4x4p mInv_out_opt, const a3real width, const a3real height, const a3real nearDist, const a3real farDist)
{
	if (width != a3real_zero && height != a3real_zero && nearDist != farDist)
	{
		const a3real farToNearInv = a3real_one / (nearDist - farDist);
		m_out[0][0] = a3real_two / width;
		m_out[1][1] = a3real_two / height;
		m_out[2][2] = farToNearInv + farToNearInv;
		m_out[3][2] = (nearDist + farDist)*farToNearInv;
		m_out[3][3] = a3real_one;

		m_out[0][1] = m_out[0][2] = m_out[0][3]
			= m_out[1][0] = m_out[1][2] = m_out[1][3]
			= m_out[2][0] = m_out[2][1] = m_out[2][3]
			= m_out[3][0] = m_out[3][1] = a3real_zero;

		if (mInv_out_opt)
		{
			mInv_out_opt[0][0] = a3real_half * width;
			mInv_out_opt[1][1] = a3real_half * height;
			mInv_out_opt[2][2] = a3real_half * (nearDist - farDist);
			mInv_out_opt[3][2] = -a3real_half * (nearDist + farDist);
			mInv_out_opt[3][3] = a3real_one;

			mInv_out_opt[0][1] = mInv_out_opt[0][2] = mInv_out_opt[0][3]
				= mInv_out_opt[1][0] = mInv_out_opt[1][2] = mInv_out_opt[1][3]
				= mInv_out_opt[2][0] = mInv_out_opt[2][1] = mInv_out_opt[2][3]
				= mInv_out_opt[3][0] = mInv_out_opt[3][1] = a3real_zero;
		}
	}
	else
		a3real4x4SetIdentity(m_out);
	return m_out;
}

A3_INLINE a3real4x4r a3real4x4MakeOrthographicProjectionPlanes(a3real4x4p m_out, a3real4x4p mInv_out_opt, const a3real rightDist, const a3real leftDist, const a3real topDist, const a3real bottomDist, const a3real nearDist, const a3real farDist)
{
	if (rightDist != leftDist && topDist != bottomDist && nearDist != farDist)
	{
		const a3real leftToRightInv = a3real_one / (rightDist - leftDist);
		const a3real bottomToTopInv = a3real_one / (topDist - bottomDist);
		const a3real farToNearInv = a3real_one / (nearDist - farDist);
		m_out[0][0] = leftToRightInv + leftToRightInv;
		m_out[1][1] = bottomToTopInv + bottomToTopInv;
		m_out[2][2] = farToNearInv + farToNearInv;
		m_out[3][0] = -(rightDist + leftDist)*leftToRightInv;
		m_out[3][1] = -(topDist + bottomDist)*bottomToTopInv;
		m_out[3][2] = (nearDist + farDist)*farToNearInv;
		m_out[3][3] = a3real_one;

		m_out[0][1] = m_out[0][2] = m_out[0][3]
			= m_out[1][0] = m_out[1][2] = m_out[1][3]
			= m_out[2][0] = m_out[2][1] = m_out[2][3] = a3real_zero;

		if (mInv_out_opt)
		{
			mInv_out_opt[0][0] = a3real_half*(rightDist - leftDist);
			mInv_out_opt[1][1] = a3real_half*(topDist - bottomDist);
			mInv_out_opt[2][2] = a3real_half * (nearDist - farDist);
			mInv_out_opt[3][0] = a3real_half*(rightDist + leftDist);
			mInv_out_opt[3][1] = a3real_half*(topDist + bottomDist);
			mInv_out_opt[3][2] = -a3real_half * (nearDist + farDist);
			mInv_out_opt[3][3] = a3real_one;

			mInv_out_opt[0][1] = mInv_out_opt[0][2] = mInv_out_opt[0][3]
				= mInv_out_opt[1][0] = mInv_out_opt[1][2] = mInv_out_opt[1][3]
				= mInv_out_opt[2][0] = mInv_out_opt[2][1] = mInv_out_opt[2][3] = a3real_zero;
		}
	}
	else
		a3real4x4SetIdentity(m_out);
	return m_out;
}


// convert to stereo projections
// interocular distance (distance between eyes) must be non-negative
// convergence distance (distance to convergence/focus) must be greater than zero (perspective)
// copies the stereo matrices to pointers passed as arguments
// optional parameter for outputting inverses
// returns pointer to right matrix
///
A3_INLINE a3real4x4r a3real4x4ConvertProjectionToStereo(a3real4x4p mL_out, a3real4x4p mR_out, a3real4x4p mInvL_out_opt, a3real4x4p mInvR_out_opt, const a3real4x4p mono, const a3real4x4p monoInv_opt, const a3real interocularDist, const a3real convergenceDist)
{
	if (interocularDist >= a3real_zero && convergenceDist > a3real_zero)
	{
		const a3real dx3 = a3real_half*interocularDist*mono[0][0];
		const a3real dx2 = dx3 / convergenceDist;
		if (mL_out)
		{
			a3real4x4SetReal4x4(mL_out, mono);
			mL_out[2][0] += dx2;
			mL_out[3][0] += dx3;
		}
		if (mR_out)
		{
			a3real4x4SetReal4x4(mR_out, mono);
			mR_out[2][0] -= dx2;
			mR_out[3][0] -= dx3;
		}

		if (monoInv_opt)
		{
			const a3real iShift = a3real_half*interocularDist;
			const a3real cShift = iShift / convergenceDist;
			const a3real dx2inv = cShift*monoInv_opt[2][2] + iShift*monoInv_opt[2][3];
			const a3real dx3inv = cShift*monoInv_opt[3][2] + iShift*monoInv_opt[3][3];

			if (mInvL_out_opt)
			{
				a3real4x4SetReal4x4(mInvL_out_opt, monoInv_opt);
				mInvL_out_opt[2][0] -= dx2inv;
				mInvL_out_opt[3][0] -= dx3inv;
			}
			if (mInvR_out_opt)
			{
				a3real4x4SetReal4x4(mInvR_out_opt, monoInv_opt);
				mInvR_out_opt[2][0] += dx2inv;
				mInvR_out_opt[3][0] += dx3inv;
			}
		}
	}
	else
	{
		if (mL_out)
			a3real4x4SetIdentity(mL_out);
		if (mR_out)
			a3real4x4SetIdentity(mR_out);

		if (monoInv_opt)
		{
			if (mInvL_out_opt)
				a3real4x4SetIdentity(mInvL_out_opt);
			if (mInvR_out_opt)
				a3real4x4SetIdentity(mInvR_out_opt);
		}
	}
	return mR_out;
}


// check if a matrix could be perspective, orthographic, or stereo
// returns 1 if yes, 0 if no
///
A3_INLINE a3boolean a3real4x4CheckPerspective(const a3real4x4p m)
{
	const a3real check = (m[0][2] + m[0][3] + m[1][2] + m[1][3] + m[3][3]);
	return (check == a3real_zero);
}

A3_INLINE a3boolean a3real4x4CheckOrthographic(const a3real4x4p m)
{
	const a3real check = (m[0][2] + m[0][3] + m[1][2] + m[1][3] + m[2][3]);
	return (check == a3real_zero);
}

A3_INLINE a3boolean a3real4x4CheckStereo(const a3real4x4p m)
{
	const a3real check = (m[2][0] * m[3][0]);
	return (check != a3real_zero);
}


// check if a matrix is perspective-stereo, or orthographic-stereo
// returns 1 if yes, 0 if not persp/ortho, -1 if persp/ortho but not stereo
///
A3_INLINE a3integer a3real4x4CheckPerspectiveStereo(const a3real4x4p m)
{
	const a3real check = (m[0][2] + m[0][3] + m[1][2] + m[1][3] + m[3][3]);
	return ((check == a3real_zero) ? (((m[2][0] * m[3][0]) != a3real_zero) ? 1 : -1) : 0);
}

A3_INLINE a3integer a3real4x4CheckOrthographicStereo(const a3real4x4p m)
{
	const a3real check = (m[0][2] + m[0][3] + m[1][2] + m[1][3] + m[2][3]);
	return ((check == a3real_zero) ? (((m[2][0] * m[3][0]) != a3real_zero) ? 1 : -1) : 0);
}


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_MATRIX4_IMPL_INL
#endif	// __ANIMAL3D_A3DM_MATRIX_H