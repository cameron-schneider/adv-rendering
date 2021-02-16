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

	a3quaternion_impl.inl
	Implementations for quaternion functions.
*/

#ifdef __ANIMAL3D_A3DM_QUATERNION_H
#ifndef __ANIMAL3D_A3DM_QUATERNION_IMPL_INL
#define __ANIMAL3D_A3DM_QUATERNION_IMPL_INL


#include "../a3sqrt.h"
#include "../a3trig.h"


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// identity quaternion
A3_GLOBAL const a3quat a3quat_identity = { a3real_zero, a3real_zero, a3real_zero, a3real_one };


//-----------------------------------------------------------------------------
// quaternion functions

// create quaternion as array
///
A3_INLINE a3real4r a3quatSetIdentity(a3real4p q_out)
{
	q_out[0] = q_out[1] = q_out[2] = a3real_zero;
	q_out[3] = a3real_one;
	return q_out;
}

A3_INLINE a3real4r a3quatSet(a3real4p q_out, const a3real x, const a3real y, const a3real z, const a3real w)
{
	q_out[0] = x;
	q_out[1] = y;
	q_out[2] = z;
	q_out[3] = w;
	return q_out;
}

A3_INLINE a3real4r a3quatSetReal4(a3real4p q_out, const a3real4p v)
{
	q_out[0] = v[0];
	q_out[1] = v[1];
	q_out[2] = v[2];
	q_out[3] = v[3];
	return q_out;
}


// create rotation quaternion
///
A3_INLINE a3real4r a3quatSetAxisAngle(a3real4p q_out, const a3real3p unitAxis, const a3real degrees)
{
	const a3real halfdeg = a3real_half * degrees;
	const a3real c = a3cosd(halfdeg);
	const a3real s = a3sind(halfdeg);
	q_out[0] = s * unitAxis[0];
	q_out[1] = s * unitAxis[1];
	q_out[2] = s * unitAxis[2];
	q_out[3] = c;
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerX(a3real4p q_out, const a3real degrees_x)
{
	const a3real halfdeg = a3real_half * degrees_x;
	q_out[0] = a3sind(halfdeg);
	q_out[1] = a3real_zero;
	q_out[2] = a3real_zero;
	q_out[3] = a3cosd(halfdeg);
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerY(a3real4p q_out, const a3real degrees_y)
{
	const a3real halfdeg = a3real_half * degrees_y;
	q_out[0] = a3real_zero;
	q_out[1] = a3sind(halfdeg);
	q_out[2] = a3real_zero;
	q_out[3] = a3cosd(halfdeg);
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerZ(a3real4p q_out, const a3real degrees_z)
{
	const a3real halfdeg = a3real_half * degrees_z;
	q_out[0] = a3real_zero;
	q_out[1] = a3real_zero;
	q_out[2] = a3sind(halfdeg);
	q_out[3] = a3cosd(halfdeg);
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerXYZ(a3real4p q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z)
{
	const a3real halfdegX = a3real_half * degrees_x;
	const a3real halfdegY = a3real_half * degrees_y;
	const a3real halfdegZ = a3real_half * degrees_z;
	const a3real cx = a3cosd(halfdegX);
	const a3real cy = a3cosd(halfdegY);
	const a3real cz = a3cosd(halfdegZ);
	const a3real sx = a3sind(halfdegX);
	const a3real sy = a3sind(halfdegY);
	const a3real sz = a3sind(halfdegZ);

	// qL = Y: qL.w = cy, qL.y = sy
	// qR = Z: qR.w = cz, qR.z = sz
	const a3real yz_x = sy*sz;
	const a3real yz_y = sy*cz;
	const a3real yz_z = cy*sz;
	const a3real yz_w = cy*cz;

	// qL = X: qL.w = cx, qL.x = sx
	// qR = yz: qR.w = yz_w, qR.x = yz_x, qR.y = yz_y, qR.z = yz_z
	q_out[0] = cx*yz_x + sx*yz_w;
	q_out[1] = cx*yz_y - sx*yz_z;
	q_out[2] = cx*yz_z + sx*yz_y;
	q_out[3] = cx*yz_w - sx*yz_x;
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerZYX(a3real4p q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z)
{
	const a3real halfdegX = a3real_half * degrees_x;
	const a3real halfdegY = a3real_half * degrees_y;
	const a3real halfdegZ = a3real_half * degrees_z;
	const a3real cx = a3cosd(halfdegX);
	const a3real cy = a3cosd(halfdegY);
	const a3real cz = a3cosd(halfdegZ);
	const a3real sx = a3sind(halfdegX);
	const a3real sy = a3sind(halfdegY);
	const a3real sz = a3sind(halfdegZ);

	// qL = Y: qL.w = cy, qL.y = sy
	// qR = X: qR.w = cx, qR.x = sx
	const a3real yx_x = cy*sx;
	const a3real yx_y = sy*cx;
	const a3real yx_z = sy*sx; //-sy*sx;
	const a3real yx_w = cy*cx;

	// qL = Z: qL.w = cz, qL.z = sz
	// qR = yx: qR.w = yx_w, qR.x = yx_x, qR.y = yx_y, qR.z = yx_z
	q_out[0] = cz*yx_x - sz*yx_y;
	q_out[1] = cz*yx_y + sz*yx_x;
	q_out[2] = sz*yx_w - cz*yx_z; //cz*yx_z + sz*yx_w;
	q_out[3] = cz*yx_w + sz*yx_z; //cz*yx_w - sz*yx_z;
	return q_out;
}

A3_INLINE a3real4r a3quatSetVectorDelta(a3real4p q_out, const a3real3p unitV0, const a3real3p unitV1)
{
	// general rules: 
	//	a  dot  b = cos(theta)
	//	a cross b = sin(theta) * n
	// quaternion: 
	//	q = cos(theta/2) + sin(theta/2) * n
	// therefore, to get half theta, 'a' is v0 and 'b' is halfway vector
	a3real3 h;
	a3real3Sum(h, unitV0, unitV1);
	a3real3Normalize(h);
	a3real3Cross(q_out, unitV0, h);
	q_out[3] = a3real3Dot(unitV0, h);
	return q_out;
}


// quaternions can also store uniform scale
///
A3_INLINE a3real4r a3quatSetAxisAngleScale(a3real4p q_out, const a3real3p unitAxis, const a3real degrees, const a3real s)
{
	const a3real len = a3sqrt(s);
	const a3real halfdeg = a3real_half * degrees;
	const a3real cs = a3cosd(halfdeg) * len;
	const a3real ss = a3sind(halfdeg) * len;
	q_out[0] = ss * unitAxis[0];
	q_out[1] = ss * unitAxis[1];
	q_out[2] = ss * unitAxis[2];
	q_out[3] = cs;
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerXScale(a3real4p q_out, const a3real degrees, const a3real s)
{
	const a3real len = a3sqrt(s);
	const a3real halfdeg = a3real_half * degrees;
	q_out[0] = a3sind(halfdeg) * len;
	q_out[1] = a3real_zero;
	q_out[2] = a3real_zero;
	q_out[3] = a3cosd(halfdeg) * len;
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerYScale(a3real4p q_out, const a3real degrees, const a3real s)
{
	const a3real len = a3sqrt(s);
	const a3real halfdeg = a3real_half * degrees;
	q_out[0] = a3real_zero;
	q_out[1] = a3sind(halfdeg) * len;
	q_out[2] = a3real_zero;
	q_out[3] = a3cosd(halfdeg) * len;
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerZScale(a3real4p q_out, const a3real degrees, const a3real s)
{
	const a3real len = a3sqrt(s);
	const a3real halfdeg = a3real_half * degrees;
	q_out[0] = a3real_zero;
	q_out[1] = a3real_zero;
	q_out[2] = a3sind(halfdeg) * len;
	q_out[3] = a3cosd(halfdeg) * len;
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerXYZScale(a3real4p q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z, const a3real s)
{
	const a3real len = a3sqrt(s);
	a3quatSetEulerXYZ(q_out, degrees_x, degrees_y, degrees_z);
	q_out[0] *= len;
	q_out[1] *= len;
	q_out[2] *= len;
	q_out[3] *= len;
	return q_out;
}

A3_INLINE a3real4r a3quatSetEulerZYXScale(a3real4p q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z, const a3real s)
{
	const a3real len = a3sqrt(s);
	a3quatSetEulerZYX(q_out, degrees_x, degrees_y, degrees_z);
	q_out[0] *= len;
	q_out[1] *= len;
	q_out[2] *= len;
	q_out[3] *= len;
	return q_out;
}

A3_INLINE a3real4r a3quatSetVectorDeltaScale(a3real4p q_out, const a3real3p unitV0, const a3real3p unitV1, const a3real s)
{
	// quaternion between two vectors with scale
	const a3real len = a3sqrt(s);
	a3real3 h;
	a3real3Sum(h, unitV0, unitV1);
	a3real3Normalize(h);
	a3real3Cross(q_out, unitV0, h);
	a3real3MulS(q_out, len);
	q_out[3] = len * a3real3Dot(unitV0, h);
	return q_out;
}

A3_INLINE a3real4r a3quatSetScale(a3real4p q_out, const a3real s)
{
	q_out[0] = q_out[1] = q_out[2] = a3real_zero;
	q_out[3] = a3sqrt(s);
	return q_out;
}


// unary operations
///
A3_INLINE a3real4r a3quatGetConjugated(a3real4p q_out, const a3real4p q)
{
	q_out[0] = -q[0];
	q_out[1] = -q[1];
	q_out[2] = -q[2];
	q_out[3] = q[3];
	return q_out;
}

A3_INLINE a3real4r a3quatGetInverse(a3real4p q_out, const a3real4p q)
{
	const a3real lenSqInv = a3real4LengthSquaredInverse(q);
	q_out[0] = -q[0] * lenSqInv;
	q_out[1] = -q[1] * lenSqInv;
	q_out[2] = -q[2] * lenSqInv;
	q_out[3] = q[3] * lenSqInv;
	return q_out;
}

A3_INLINE a3real4r a3quatGetInverseGetScale(a3real4p q_out, a3real *s_out, const a3real4p q)
{
	const a3real lenSq = a3real4LengthSquared(q);
	const a3real lenSqInv = a3recip(lenSq);
	q_out[0] = -q[0] * lenSqInv;
	q_out[1] = -q[1] * lenSqInv;
	q_out[2] = -q[2] * lenSqInv;
	q_out[3] = q[3] * lenSqInv;
	*s_out = lenSq;
	return q_out;
}

A3_INLINE a3real4r a3quatGetInverseIgnoreScale(a3real4p q_out, const a3real4p q)
{
	// the inverse without scale is just the conjugate
	q_out[0] = -q[0];
	q_out[1] = -q[1];
	q_out[2] = -q[2];
	q_out[3] = q[3];
	return q_out;
}

A3_INLINE a3real4r a3quatConjugate(a3real4p q_inout)
{
	q_inout[0] = -q_inout[0];
	q_inout[1] = -q_inout[1];
	q_inout[2] = -q_inout[2];
	return q_inout;
}

A3_INLINE a3real4r a3quatInvert(a3real4p q_inout)
{
	const a3real lenSqInv = a3real4LengthSquaredInverse(q_inout);
	q_inout[0] *= -lenSqInv;
	q_inout[1] *= -lenSqInv;
	q_inout[2] *= -lenSqInv;
	q_inout[3] *= lenSqInv;
	return q_inout;
}

A3_INLINE a3real4r a3quatInvertGetScale(a3real4p q_inout, a3real *s_out)
{
	const a3real lenSq = a3real4LengthSquared(q_inout);
	const a3real lenSqInv = a3recip(lenSq);
	q_inout[0] *= -lenSqInv;
	q_inout[1] *= -lenSqInv;
	q_inout[2] *= -lenSqInv;
	q_inout[3] *= lenSqInv;
	*s_out = lenSq;
	return q_inout;
}

A3_INLINE a3real4r a3quatInvertIgnoreScale(a3real4p q_inout)
{
	// conjugate
	q_inout[0] = -q_inout[0];
	q_inout[1] = -q_inout[1];
	q_inout[2] = -q_inout[2];
	return q_inout;
}

A3_INLINE a3real4rk a3quatGetAxisAngle(const a3real4p q, a3real3p unitAxis_out, a3real *degrees_out)
{
	const a3real lenInv = a3real4LengthInverse(q);
	const a3real ac = a3acosd(q[3] * lenInv);
	const a3real s = (ac != a3real_zero ? lenInv / a3sind(ac) : a3real_zero);
	unitAxis_out[0] = q[0] * s;
	unitAxis_out[1] = q[1] * s;
	unitAxis_out[2] = q[2] * s;
	*degrees_out = ac;
	return q;
}

A3_INLINE a3real4rk a3quatGetAxisAngleScale(const a3real4p q, a3real3p unitAxis_out, a3real *degrees_out, a3real *s_out)
{
	const a3real lenSq = a3real4LengthSquared(q);
	const a3real lenInv = (a3real)a3sqrtInverse(lenSq);
	const a3real ac = a3acosd(q[3] * lenInv);
	const a3real s = (ac != a3real_zero ? lenInv / a3sind(ac) : a3real_zero);
	unitAxis_out[0] = q[0] * s;
	unitAxis_out[1] = q[1] * s;
	unitAxis_out[2] = q[2] * s;
	*degrees_out = ac;
	*s_out = lenSq;
	return q;
}

A3_INLINE a3real4rk a3quatGetAxisAngleIgnoreScale(const a3real4p q, a3real3p unitAxis_out, a3real *degrees_out)
{
	const a3real ac = a3acosd(q[3]);
	const a3real s = (ac != a3real_zero ? a3real_one / a3sind(ac) : a3real_zero);
	unitAxis_out[0] = q[0] * s;
	unitAxis_out[1] = q[1] * s;
	unitAxis_out[2] = q[2] * s;
	*degrees_out = ac;
	return q;
}

A3_INLINE a3real4rk a3quatGetEulerXYZ(const a3real4p q, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out)
{
	const a3real lenSqInv = a3real4LengthSquaredInverse(q);
	const a3real x = q[0], y = q[1], z = q[2], w = q[3];
	const a3real xx = x * x, yy = y * y, zz = z * z, ww = w * w;
	*degrees_x_out = a3atan2d(a3real_two * (w*x - y*z), (ww - xx - yy + zz));
	*degrees_y_out = a3asind(a3real_two * lenSqInv * (w*y + x*z));
	*degrees_z_out = a3atan2d(a3real_two * (w*z - x*y), (ww + xx - yy - zz));
	return q;
}

A3_INLINE a3real4rk a3quatGetEulerZYX(const a3real4p q, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out)
{
	const a3real lenSqInv = a3real4LengthSquaredInverse(q);
	const a3real x = q[0], y = q[1], z = q[2], w = q[3];
	const a3real xx = x * x, yy = y * y, zz = z * z, ww = w * w;
	*degrees_x_out = a3atan2d(a3real_two * (w*x + y*z), (ww - xx - yy + zz));
	*degrees_y_out = a3asind(a3real_two * lenSqInv * (w*y - x*z));
	*degrees_z_out = a3atan2d(a3real_two * (w*z + x*y), (ww + xx - yy - zz));
	return q;
}

A3_INLINE a3real4rk a3quatGetEulerXYZScale(const a3real4p q, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out, a3real *s_out)
{
	const a3real lenSqInv = a3real4LengthSquaredInverse(q);
	const a3real x = q[0], y = q[1], z = q[2], w = q[3];
	const a3real xx = x * x, yy = y * y, zz = z * z, ww = w * w;
	*degrees_x_out = a3atan2d(a3real_two * (w*x - y*z), (ww - xx - yy + zz));
	*degrees_y_out = a3asind(a3real_two * lenSqInv * (w*y + x*z));
	*degrees_z_out = a3atan2d(a3real_two * (w*z - x*y), (ww + xx - yy - zz));
	*s_out = a3sqrtInverse(lenSqInv);
	return q;
}

A3_INLINE a3real4rk a3quatGetEulerZYXScale(const a3real4p q, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out, a3real *s_out)
{
	const a3real lenSqInv = a3real4LengthSquaredInverse(q);
	const a3real x = q[0], y = q[1], z = q[2], w = q[3];
	const a3real xx = x * x, yy = y * y, zz = z * z, ww = w * w;
	*degrees_x_out = a3atan2d(a3real_two * (w*x + y*z), (ww - xx - yy + zz));
	*degrees_y_out = a3asind(a3real_two * lenSqInv * (w*y - x*z));
	*degrees_z_out = a3atan2d(a3real_two * (w*z + x*y), (ww + xx - yy - zz));
	*s_out = a3sqrtInverse(lenSqInv);
	return q;
}

A3_INLINE a3real4rk a3quatGetEulerXYZIgnoreScale(const a3real4p q, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out)
{
	const a3real x = q[0], y = q[1], z = q[2], w = q[3];
	const a3real xx = x * x, yy = y * y, zz = z * z, ww = w * w;
	*degrees_x_out = a3atan2d(a3real_two * (w*x - y*z), (ww - xx - yy + zz));
	*degrees_y_out = a3asind(a3real_two * (w*y + x*z));
	*degrees_z_out = a3atan2d(a3real_two * (w*z - x*y), (ww + xx - yy - zz));
	return q;
}

A3_INLINE a3real4rk a3quatGetEulerZYXIgnoreScale(const a3real4p q, a3real *degrees_x_out, a3real *degrees_y_out, a3real *degrees_z_out)
{
	const a3real x = q[0], y = q[1], z = q[2], w = q[3];
	const a3real xx = x * x, yy = y * y, zz = z * z, ww = w * w;
	*degrees_x_out = a3atan2d(a3real_two * (w*x + y*z), (ww - xx - yy + zz));
	*degrees_y_out = a3asind(a3real_two * (w*y - x*z));
	*degrees_z_out = a3atan2d(a3real_two * (w*z + x*y), (ww + xx - yy - zz));
	return q;
}

A3_INLINE a3real4rk a3quatGetScale(const a3real4p q, a3real *s_out)
{
	*s_out = a3real4LengthSquared(q);
	return q;
}


// conversion to and from matrix
///
A3_INLINE a3real3x3r a3quatConvertToMat3(a3real3x3p m_out, const a3real4p q)
{
	const a3real X2 = q[0] + q[0], Y2 = q[1] + q[1], Z2 = q[2] + q[2],
		XX = q[0] * q[0], YY = q[1] * q[1], ZZ = q[2] * q[2], WW = q[3] * q[3],
		WX2 = q[3] * X2, WY2 = q[3] * Y2, WZ2 = q[3] * Z2,
		XY2 = X2*q[1], XZ2 = X2*q[2], YZ2 = Y2*q[2];
	return a3real3x3Set(m_out,
#ifndef A3_MAT_ROWMAJOR
		WW + XX - YY - ZZ, XY2 + WZ2, XZ2 - WY2,
		XY2 - WZ2, WW - XX + YY - ZZ, YZ2 + WX2,
		XZ2 + WY2, YZ2 - WX2, WW - XX - YY + ZZ
#else	// A3_MAT_ROWMAJOR
		WW + XX - YY - ZZ, XY2 - WZ2, XZ2 + WY2,
		XY2 + WZ2, WW - XX + YY - ZZ, YZ2 - WX2,
		XZ2 - WY2, YZ2 + WX2, WW - XX - YY + ZZ
#endif	// !A3_MAT_ROWMAJOR
	);
}

A3_INLINE a3real4x4r a3quatConvertToMat4(a3real4x4p m_out, const a3real4p q)
{
	const a3real X2 = q[0] + q[0], Y2 = q[1] + q[1], Z2 = q[2] + q[2],
		XX = q[0] * q[0], YY = q[1] * q[1], ZZ = q[2] * q[2], WW = q[3] * q[3],
		WX2 = q[3] * X2, WY2 = q[3] * Y2, WZ2 = q[3] * Z2,
		XY2 = X2*q[1], XZ2 = X2*q[2], YZ2 = Y2*q[2];
	return a3real4x4Set(m_out,
#ifndef A3_MAT_ROWMAJOR
		WW + XX - YY - ZZ, XY2 + WZ2, XZ2 - WY2, a3real_zero,
		XY2 - WZ2, WW - XX + YY - ZZ, YZ2 + WX2, a3real_zero,
		XZ2 + WY2, YZ2 - WX2, WW - XX - YY + ZZ, a3real_zero,
#else	// A3_MAT_ROWMAJOR
		WW + XX - YY - ZZ, XY2 - WZ2, XZ2 + WY2, a3real_zero,
		XY2 + WZ2, WW - XX + YY - ZZ, YZ2 - WX2, a3real_zero,
		XZ2 - WY2, YZ2 + WX2, WW - XX - YY + ZZ, a3real_zero,
#endif	// !A3_MAT_ROWMAJOR
		a3real_zero, a3real_zero, a3real_zero, a3real_one
	);
}

A3_INLINE a3real4x4r a3quatConvertToMat4Translate(a3real4x4p m_out, const a3real4p q, const a3real3p translate)
{
	const a3real X2 = q[0] + q[0], Y2 = q[1] + q[1], Z2 = q[2] + q[2],
		XX = q[0] * q[0], YY = q[1] * q[1], ZZ = q[2] * q[2], WW = q[3] * q[3],
		WX2 = q[3] * X2, WY2 = q[3] * Y2, WZ2 = q[3] * Z2,
		XY2 = X2*q[1], XZ2 = X2*q[2], YZ2 = Y2*q[2];
	return a3real4x4Set(m_out,
#ifndef A3_MAT_ROWMAJOR
		WW + XX - YY - ZZ, XY2 + WZ2, XZ2 - WY2, a3real_zero,
		XY2 - WZ2, WW - XX + YY - ZZ, YZ2 + WX2, a3real_zero,
		XZ2 + WY2, YZ2 - WX2, WW - XX - YY + ZZ, a3real_zero,
#else	// A3_MAT_ROWMAJOR
		WW + XX - YY - ZZ, XY2 - WZ2, XZ2 + WY2, a3real_zero,
		XY2 + WZ2, WW - XX + YY - ZZ, YZ2 - WX2, a3real_zero,
		XZ2 - WY2, YZ2 + WX2, WW - XX - YY + ZZ, a3real_zero,
#endif	// !A3_MAT_ROWMAJOR
		translate[0], translate[1], translate[2], a3real_one
	);
}

A3_INLINE a3real4r a3quatConvertFromMat3(a3real4p q_out, const a3real3x3p m)
{
	const a3real s = a3real_one + m[0][0] + m[1][1] + m[2][2];
	const a3real w = q_out[3] = a3sqrt(s), w2inv = a3recip(w + w);
	q_out[3] *= a3real_half;
#ifndef A3_MAT_ROWMAJOR
	q_out[0] = (m[1][2] - m[2][1]) * w2inv;
	q_out[1] = (m[2][0] - m[0][2]) * w2inv;
	q_out[2] = (m[0][1] - m[1][0]) * w2inv;
#else	// A3_MAT_ROWMAJOR
	q_out[0] = (m[2][1] - m[1][2]) * w2inv;
	q_out[1] = (m[0][2] - m[2][0]) * w2inv;
	q_out[2] = (m[1][0] - m[0][1]) * w2inv;
#endif	// !A3_MAT_ROWMAJOR
	return q_out;
}

A3_INLINE a3real4r a3quatConvertFromMat4(a3real4p q_out, const a3real4x4p m)
{
	const a3real s = a3real_one + m[0][0] + m[1][1] + m[2][2];
	const a3real w = q_out[3] = a3sqrt(s), w2inv = a3recip(w + w);
	q_out[3] *= a3real_half;
#ifndef A3_MAT_ROWMAJOR
	q_out[0] = (m[1][2] - m[2][1]) * w2inv;
	q_out[1] = (m[2][0] - m[0][2]) * w2inv;
	q_out[2] = (m[0][1] - m[1][0]) * w2inv;
#else	// A3_MAT_ROWMAJOR
	q_out[0] = (m[2][1] - m[1][2]) * w2inv;
	q_out[1] = (m[0][2] - m[2][0]) * w2inv;
	q_out[2] = (m[1][0] - m[0][1]) * w2inv;
#endif	// !A3_MAT_ROWMAJOR
	return q_out;
}

A3_INLINE a3real4r a3quatConvertFromMat4Translate(a3real4p q_out, a3real3p translate_out, const a3real4x4p m)
{
	const a3real s = a3real_one + m[0][0] + m[1][1] + m[2][2];
	const a3real w = q_out[3] = a3sqrt(s), w2inv = a3recip(w + w);
	q_out[3] *= a3real_half;
#ifndef A3_MAT_ROWMAJOR
	q_out[0] = (m[1][2] - m[2][1]) * w2inv;
	q_out[1] = (m[2][0] - m[0][2]) * w2inv;
	q_out[2] = (m[0][1] - m[1][0]) * w2inv;
	a3real3Set(translate_out, m[3][0], m[3][1], m[3][2]);
#else	// A3_MAT_ROWMAJOR
	q_out[0] = (m[2][1] - m[1][2]) * w2inv;
	q_out[1] = (m[0][2] - m[2][0]) * w2inv;
	q_out[2] = (m[1][0] - m[0][1]) * w2inv;
	a3real3Set(translate_out, m[0][3], m[1][3], m[2][3]);
#endif	// !A3_MAT_ROWMAJOR
	return q_out;
}

A3_INLINE a3real4r a3quatConvertFromMat3Safe(a3real4p q_out, const a3real3x3p m)
{
	// help from here: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	const a3real xx = m[0][0], yy = m[1][1], zz = m[2][2], trace = xx + yy + zz;
	a3real s, s2inv;
#ifndef A3_MAT_ROWMAJOR
	if (trace > a3real_zero)
	{
		s = a3real_one + trace;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[1][2] - m[2][1]) * s2inv;
		q_out[1] = (m[2][0] - m[0][2]) * s2inv;
		q_out[2] = (m[0][1] - m[1][0]) * s2inv;
		q_out[3] = s * a3real_half;
	}
	else if ((xx > yy) & (xx > zz))
	{
		s = a3real_one + xx - yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = s * a3real_half;
		q_out[1] = (m[0][1] + m[1][0]) * s2inv;
		q_out[2] = (m[2][0] + m[0][2]) * s2inv;
		q_out[3] = (m[1][2] - m[2][1]) * s2inv;
	}
	else if (yy > zz)
	{
		s = a3real_one - xx + yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[0][1] + m[1][0]) * s2inv;
		q_out[1] = s * a3real_half;
		q_out[2] = (m[1][2] + m[2][1]) * s2inv;
		q_out[3] = (m[2][0] - m[0][2]) * s2inv;
	}
	else
	{
		s = a3real_one - xx - yy + zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][0] + m[0][2]) * s2inv;
		q_out[1] = (m[1][2] + m[2][1]) * s2inv;
		q_out[2] = s * a3real_half;
		q_out[3] = (m[0][1] - m[1][0]) * s2inv;
	}
#else	// A3_MAT_ROWMAJOR
	if (trace > a3real_zero)
	{
		s = a3real_one + trace;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][1] - m[1][2]) * s2inv;
		q_out[1] = (m[0][2] - m[2][0]) * s2inv;
		q_out[2] = (m[1][0] - m[0][1]) * s2inv;
		q_out[3] = s * a3real_half;
	}
	else if ((xx > yy) & (xx > zz))
	{
		s = a3real_one + xx - yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = s * a3real_half;
		q_out[1] = (m[0][1] + m[1][0]) * s2inv;
		q_out[2] = (m[2][0] + m[0][2]) * s2inv;
		q_out[3] = (m[2][1] - m[1][2]) * s2inv;
	}
	else if (yy > zz)
	{
		s = a3real_one - xx + yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[0][1] + m[1][0]) * s2inv;
		q_out[1] = s * a3real_half;
		q_out[2] = (m[1][2] + m[2][1]) * s2inv;
		q_out[3] = (m[0][2] - m[2][0]) * s2inv;
	}
	else
	{
		s = a3real_one - xx - yy + zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][0] + m[0][2]) * s2inv;
		q_out[1] = (m[1][2] + m[2][1]) * s2inv;
		q_out[2] = s * a3real_half;
		q_out[3] = (m[1][0] - m[0][1]) * s2inv;
	}
#endif	// !A3_MAT_ROWMAJOR
	return q_out;
}

A3_INLINE a3real4r a3quatConvertFromMat4Safe(a3real4p q_out, const a3real4x4p m)
{
	const a3real xx = m[0][0], yy = m[1][1], zz = m[2][2], trace = xx + yy + zz;
	a3real s, s2inv;
#ifndef A3_MAT_ROWMAJOR
	if (trace > a3real_zero)
	{
		s = a3real_one + trace;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[1][2] - m[2][1]) * s2inv;
		q_out[1] = (m[2][0] - m[0][2]) * s2inv;
		q_out[2] = (m[0][1] - m[1][0]) * s2inv;
		q_out[3] = s * a3real_half;
	}
	else if ((xx > yy) & (xx > zz))
	{
		s = a3real_one + xx - yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = s * a3real_half;
		q_out[1] = (m[0][1] + m[1][0]) * s2inv;
		q_out[2] = (m[2][0] + m[0][2]) * s2inv;
		q_out[3] = (m[1][2] - m[2][1]) * s2inv;
	}
	else if (yy > zz)
	{
		s = a3real_one - xx + yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[0][1] + m[1][0]) * s2inv;
		q_out[1] = s * a3real_half;
		q_out[2] = (m[1][2] + m[2][1]) * s2inv;
		q_out[3] = (m[2][0] - m[0][2]) * s2inv;
	}
	else
	{
		s = a3real_one - xx - yy + zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][0] + m[0][2]) * s2inv;
		q_out[1] = (m[1][2] + m[2][1]) * s2inv;
		q_out[2] = s * a3real_half;
		q_out[3] = (m[0][1] - m[1][0]) * s2inv;
	}
#else	// A3_MAT_ROWMAJOR
	if (trace > a3real_zero)
	{
		s = a3real_one + trace;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][1] - m[1][2]) * s2inv;
		q_out[1] = (m[0][2] - m[2][0]) * s2inv;
		q_out[2] = (m[1][0] - m[0][1]) * s2inv;
		q_out[3] = s * a3real_half;
	}
	else if ((xx > yy) & (xx > zz))
	{
		s = a3real_one + xx - yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = s * a3real_half;
		q_out[1] = (m[0][1] + m[1][0]) * s2inv;
		q_out[2] = (m[2][0] + m[0][2]) * s2inv;
		q_out[3] = (m[2][1] - m[1][2]) * s2inv;
	}
	else if (yy > zz)
	{
		s = a3real_one - xx + yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[0][1] + m[1][0]) * s2inv;
		q_out[1] = s * a3real_half;
		q_out[2] = (m[1][2] + m[2][1]) * s2inv;
		q_out[3] = (m[0][2] - m[2][0]) * s2inv;
	}
	else
	{
		s = a3real_one - xx - yy + zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][0] + m[0][2]) * s2inv;
		q_out[1] = (m[1][2] + m[2][1]) * s2inv;
		q_out[2] = s * a3real_half;
		q_out[3] = (m[1][0] - m[0][1]) * s2inv;
	}
#endif	// !A3_MAT_ROWMAJOR
	return q_out;
}

A3_INLINE a3real4r a3quatConvertFromMat4SafeTranslate(a3real4p q_out, a3real3p translate_out, const a3real4x4p m)
{
	const a3real xx = m[0][0], yy = m[1][1], zz = m[2][2], trace = xx + yy + zz;
	a3real s, s2inv;
#ifndef A3_MAT_ROWMAJOR
	if (trace > a3real_zero)
	{
		s = a3real_one + trace;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[1][2] - m[2][1]) * s2inv;
		q_out[1] = (m[2][0] - m[0][2]) * s2inv;
		q_out[2] = (m[0][1] - m[1][0]) * s2inv;
		q_out[3] = s * a3real_half;
	}
	else if ((xx > yy) & (xx > zz))
	{
		s = a3real_one + xx - yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = s * a3real_half;
		q_out[1] = (m[0][1] + m[1][0]) * s2inv;
		q_out[2] = (m[2][0] + m[0][2]) * s2inv;
		q_out[3] = (m[1][2] - m[2][1]) * s2inv;
	}
	else if (yy > zz)
	{
		s = a3real_one - xx + yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[0][1] + m[1][0]) * s2inv;
		q_out[1] = s * a3real_half;
		q_out[2] = (m[1][2] + m[2][1]) * s2inv;
		q_out[3] = (m[2][0] - m[0][2]) * s2inv;
	}
	else
	{
		s = a3real_one - xx - yy + zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][0] + m[0][2]) * s2inv;
		q_out[1] = (m[1][2] + m[2][1]) * s2inv;
		q_out[2] = s * a3real_half;
		q_out[3] = (m[0][1] - m[1][0]) * s2inv;
	}
	a3real3Set(translate_out, m[3][0], m[3][1], m[3][2]);
#else	// A3_MAT_ROWMAJOR
	if (trace > a3real_zero)
	{
		s = a3real_one + trace;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][1] - m[1][2]) * s2inv;
		q_out[1] = (m[0][2] - m[2][0]) * s2inv;
		q_out[2] = (m[1][0] - m[0][1]) * s2inv;
		q_out[3] = s * a3real_half;
	}
	else if ((xx > yy) & (xx > zz))
	{
		s = a3real_one + xx - yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = s * a3real_half;
		q_out[1] = (m[0][1] + m[1][0]) * s2inv;
		q_out[2] = (m[2][0] + m[0][2]) * s2inv;
		q_out[3] = (m[2][1] - m[1][2]) * s2inv;
	}
	else if (yy > zz)
	{
		s = a3real_one - xx + yy - zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[0][1] + m[1][0]) * s2inv;
		q_out[1] = s * a3real_half;
		q_out[2] = (m[1][2] + m[2][1]) * s2inv;
		q_out[3] = (m[0][2] - m[2][0]) * s2inv;
	}
	else
	{
		s = a3real_one - xx - yy + zz;
		s = a3sqrt(s);
		s2inv = a3recip(s + s);
		q_out[0] = (m[2][0] + m[0][2]) * s2inv;
		q_out[1] = (m[1][2] + m[2][1]) * s2inv;
		q_out[2] = s * a3real_half;
		q_out[3] = (m[1][0] - m[0][1]) * s2inv;
	}
	a3real3Set(translate_out, m[0][3], m[1][3], m[2][3]);
#endif	// !A3_MAT_ROWMAJOR
	return q_out;
}


// binary operations: quaternion multiply/concatenate/rotate vector
///
A3_INLINE a3real4r a3quatProduct(a3real4p q_out, const a3real4p qL, const a3real4p qR)
{
	// (w0w1 - dot(v0, v1)) + (w0*v1 + w1*v0 + cross(v0, v1))
	a3real4 tmp;
	tmp[3] = qL[3] * qR[3] - qL[0] * qR[0] - qL[1] * qR[1] - qL[2] * qR[2];
	tmp[0] = qL[3] * qR[0] + qL[0] * qR[3] + qL[1] * qR[2] - qL[2] * qR[1];
	tmp[1] = qL[3] * qR[1] - qL[0] * qR[2] + qL[1] * qR[3] + qL[2] * qR[0];
	tmp[2] = qL[3] * qR[2] + qL[0] * qR[1] - qL[1] * qR[0] + qL[2] * qR[3];
	q_out[0] = tmp[0];
	q_out[1] = tmp[1];
	q_out[2] = tmp[2];
	q_out[3] = tmp[3];
	return q_out;
}

A3_INLINE a3real4r a3quatConcatL(a3real4p qL_inout, const a3real4p qR)
{
	a3real4 q;
	a3quatProduct(q, qL_inout, qR);
	return a3quatSetReal4(qL_inout, q);
}

A3_INLINE a3real4r a3quatConcatR(const a3real4p qL, a3real4p qR_inout)
{
	a3real4 q;
	a3quatProduct(q, qL, qR_inout);
	return a3quatSetReal4(qR_inout, q);
}


A3_INLINE a3real3r a3quatVec3GetRotatedIgnoreScale(a3real3p v_out, const a3real3p v, const a3real4p q)
{
	// qvq* without accounting for scale
	const a3real q2x = q[0] + q[0];
	const a3real q2y = q[1] + q[1];
	const a3real q2z = q[2] + q[2];
	const a3real cx = q[3]*v[0] + q[1]*v[2] - q[2]*v[1];
	const a3real cy = q[3]*v[1] + q[2]*v[0] - q[0]*v[2];
	const a3real cz = q[3]*v[2] + q[0]*v[1] - q[1]*v[0];
	v_out[0] = v[0] + q2y*cz - q2z*cy;
	v_out[1] = v[1] + q2z*cx - q2x*cz;
	v_out[2] = v[2] + q2x*cy - q2y*cx;
	return v_out;
}

A3_INLINE a3real3r a3quatVec3GetRotatedScaled(a3real3p v_out, const a3real3p v, const a3real4p q)
{
	// qvq*
	const a3real lenSq = a3real4LengthSquared(q);
	const a3real q2x = q[0] + q[0];
	const a3real q2y = q[1] + q[1];
	const a3real q2z = q[2] + q[2];
	const a3real cx = q[3] * v[0] + q[1] * v[2] - q[2] * v[1];
	const a3real cy = q[3] * v[1] + q[2] * v[0] - q[0] * v[2];
	const a3real cz = q[3] * v[2] + q[0] * v[1] - q[1] * v[0];
	v_out[0] = v[0]*lenSq + q2y*cz - q2z*cy;
	v_out[1] = v[1]*lenSq + q2z*cx - q2x*cz;
	v_out[2] = v[2]*lenSq + q2x*cy - q2y*cx;
	return v_out;
}

A3_INLINE a3real3r a3quatVec3GetRotated(a3real3p v_out, const a3real3p v, const a3real4p q)
{
	// qvq^-1
	const a3real lenSqInv = a3real4LengthSquaredInverse(q);
	const a3real q2x = (q[0] + q[0])*lenSqInv;
	const a3real q2y = (q[1] + q[1])*lenSqInv;
	const a3real q2z = (q[2] + q[2])*lenSqInv;
	const a3real cx = q[3] * v[0] + q[1] * v[2] - q[2] * v[1];
	const a3real cy = q[3] * v[1] + q[2] * v[0] - q[0] * v[2];
	const a3real cz = q[3] * v[2] + q[0] * v[1] - q[1] * v[0];
	v_out[0] = v[0] + q2y*cz - q2z*cy;
	v_out[1] = v[1] + q2z*cx - q2x*cz;
	v_out[2] = v[2] + q2x*cy - q2y*cx;
	return v_out;
}

A3_INLINE a3real3r a3quatVec3RotateIgnoreScale(a3real3p v_inout, const a3real4p q)
{
	// qvq* no scale
	const a3real q2x = q[0] + q[0];
	const a3real q2y = q[1] + q[1];
	const a3real q2z = q[2] + q[2];
	const a3real cx = q[3] * v_inout[0] + q[1] * v_inout[2] - q[2] * v_inout[1];
	const a3real cy = q[3] * v_inout[1] + q[2] * v_inout[0] - q[0] * v_inout[2];
	const a3real cz = q[3] * v_inout[2] + q[0] * v_inout[1] - q[1] * v_inout[0];
	v_inout[0] += q2y*cz - q2z*cy;
	v_inout[1] += q2z*cx - q2x*cz;
	v_inout[2] += q2x*cy - q2y*cx;
	return v_inout;
}

A3_INLINE a3real3r a3quatVec3RotateScale(a3real3p v_inout, const a3real4p q)
{
	// qvq*
	const a3real lenSq = a3real4LengthSquared(q);
	const a3real q2x = q[0] + q[0];
	const a3real q2y = q[1] + q[1];
	const a3real q2z = q[2] + q[2];
	const a3real cx = q[3] * v_inout[0] + q[1] * v_inout[2] - q[2] * v_inout[1];
	const a3real cy = q[3] * v_inout[1] + q[2] * v_inout[0] - q[0] * v_inout[2];
	const a3real cz = q[3] * v_inout[2] + q[0] * v_inout[1] - q[1] * v_inout[0];
	v_inout[0] = v_inout[0] * lenSq + q2y*cz - q2z*cy;
	v_inout[1] = v_inout[1] * lenSq + q2z*cx - q2x*cz;
	v_inout[2] = v_inout[2] * lenSq + q2x*cy - q2y*cx;
	return v_inout;
}

A3_INLINE a3real3r a3quatVec3Rotate(a3real3p v_inout, const a3real4p q)
{
	// qvq^-1
	const a3real lenSqInv = a3real4LengthSquaredInverse(q);
	const a3real q2x = (q[0] + q[0])*lenSqInv;
	const a3real q2y = (q[1] + q[1])*lenSqInv;
	const a3real q2z = (q[2] + q[2])*lenSqInv;
	const a3real cx = q[3] * v_inout[0] + q[1] * v_inout[2] - q[2] * v_inout[1];
	const a3real cy = q[3] * v_inout[1] + q[2] * v_inout[0] - q[0] * v_inout[2];
	const a3real cz = q[3] * v_inout[2] + q[0] * v_inout[1] - q[1] * v_inout[0];
	v_inout[0] += q2y*cz - q2z*cy;
	v_inout[1] += q2z*cx - q2x*cz;
	v_inout[2] += q2x*cy - q2y*cx;
	return v_inout;
}


// quaternion multiplication with pure quaternions (slight optimization)
// a pure quaternion has a a3real part of zero, so it is essentially a 3D vector
///
A3_INLINE a3real4r a3quatProductPureL(a3real4p q_out, const a3real4p qPureL, const a3real4p qR)
{
	// base formula: 
	// (w0w1 - dot(v0, v1)) + (w0*v1 + w1*v0 + cross(v0, v1))
	// left w component is zero, so the formula is simplified: 
	// (-dot(v0, v1)) + (w1*v0 + cross(v0, v1))
	a3real4 tmp;
	tmp[3] = -qPureL[0] * qR[0] - qPureL[1] * qR[1] - qPureL[2] * qR[2];
	tmp[0] = qPureL[0] * qR[3] + qPureL[1] * qR[2] - qPureL[2] * qR[1];
	tmp[1] = qPureL[1] * qR[3] + qPureL[2] * qR[0] - qPureL[0] * qR[2];
	tmp[2] = qPureL[0] * qR[1] - qPureL[1] * qR[0] + qPureL[2] * qR[3];
	q_out[0] = tmp[0];
	q_out[1] = tmp[1];
	q_out[2] = tmp[2];
	q_out[3] = tmp[3];
	return q_out;
}

A3_INLINE a3real4r a3quatProductPureR(a3real4p q_out, const a3real4p qL, const a3real4p qPureR)
{
	// base formula: 
	// (w0w1 - dot(v0, v1)) + (w0*v1 + w1*v0 + cross(v0, v1))
	// right w component is zero, so the formula is simplified: 
	// (-dot(v0, v1)) + (w0*v1 + cross(v0, v1))
	a3real4 tmp;
	tmp[3] = -qL[0] * qPureR[0] - qL[1] * qPureR[1] - qL[2] * qPureR[2];
	tmp[0] = qL[3] * qPureR[0] + qL[1] * qPureR[2] - qL[2] * qPureR[1];
	tmp[1] = qL[3] * qPureR[1] - qL[0] * qPureR[2] + qL[2] * qPureR[0];
	tmp[2] = qL[3] * qPureR[2] + qL[0] * qPureR[1] - qL[1] * qPureR[0];
	q_out[0] = tmp[0];
	q_out[1] = tmp[1];
	q_out[2] = tmp[2];
	q_out[3] = tmp[3];
	return q_out;
}

A3_INLINE a3real4r a3quatConcatLPureL(a3real4p qPureL_inout, const a3real4p qR)
{
	// store cross product separately, calculate w (since it's overriding zero), 
	//	then scale output by existing w component and add the cross product
	a3real3 tmp;
	qPureL_inout[3] = -qPureL_inout[0] * qR[0] - qPureL_inout[1] * qR[1] - qPureL_inout[2] * qR[2];
	a3real3Cross(tmp, qPureL_inout, qR);
	a3real3MulS(qPureL_inout, qR[3]);
	return a3real3Add(qPureL_inout, tmp);
}

A3_INLINE a3real4r a3quatConcatRPureL(const a3real4p qPureL, a3real4p qR_inout)
{
	a3real4 tmp;
	a3quatProductPureL(tmp, qPureL, qR_inout);
	return a3real4SetReal4(qR_inout, tmp);
}

A3_INLINE a3real4r a3quatConcatLPureR(a3real4p qL_inout, const a3real4p qPureR)
{
	a3real4 tmp;
	a3quatProductPureL(tmp, qL_inout, qPureR);
	return a3real4SetReal4(qL_inout, tmp);
}

A3_INLINE a3real4r a3quatConcatRPureR(const a3real4p qL, a3real4p qPureR_inout)
{
	a3real3 tmp;
	qPureR_inout[3] = -qL[0] * qPureR_inout[0] - qL[1] * qPureR_inout[1] - qL[2] * qPureR_inout[2];
	a3real3Cross(tmp, qL, qPureR_inout);
	a3real3MulS(qPureR_inout, qL[3]);
	return a3real3Add(qPureR_inout, tmp);
}


// interpolation: quaternion SLERP
// use unit versions when the inputs are definitely unit quaternions
// use identity versions to blend between some quaternion and identity
// all versions assume non-zero scale
///
A3_INLINE a3real4r a3quatSlerp(a3real4p q_out, const a3real4p q0, const a3real4p q1, const a3real param)
{
	// slerp(p0, p1, t) = [sin([1 - t]a)p0 + sin(ta)p1] / sin(a)
	const a3real lenSq = a3real4LengthSquared(q0) * a3real4LengthSquared(q1);
	a3real d = a3real4Dot(q0, q1) * a3sqrtInverse(lenSq);
	a3real4 q1_b;
	const a3real *q1_ptr = q1;
	
	// quaternion feature: if dot product is negative, negate it and q1
	if (d < a3real_zero)
	{
		d = -d;
		q1_ptr = q1_b;
		a3real4GetNegative(q1_b, q1);
	}

	// if the quaternions are not parallel, perform SLERP
	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		q_out[0] = s0*q0[0] + s1*q1_ptr[0];
		q_out[1] = s0*q0[1] + s1*q1_ptr[1];
		q_out[2] = s0*q0[2] + s1*q1_ptr[2];
		q_out[3] = s0*q0[3] + s1*q1_ptr[3];
		return q_out;
	}
	
	// if the quaternions are beyond parallel, do NLERP
	if (d > a3real_one)
		return a3real4NLerp(q_out, q0, q1_ptr, param);

	// if the quaternions are parallel, set result to either input
	return a3quatSetReal4(q_out, q0);
}

A3_INLINE a3real4r a3quatSlerpUnit(a3real4p q_out, const a3real4p q0, const a3real4p q1, const a3real param)
{
	a3real d = a3real4Dot(q0, q1);
	a3real4 q1_b;
	const a3real *q1_ptr = q1;

	// dot check
	if (d < a3real_zero)
	{
		d = -d;
		q1_ptr = q1_b;
		a3real4GetNegative(q1_b, q1);
	}

	// SLERP
	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		q_out[0] = s0*q0[0] + s1*q1_ptr[0];
		q_out[1] = s0*q0[1] + s1*q1_ptr[1];
		q_out[2] = s0*q0[2] + s1*q1_ptr[2];
		q_out[3] = s0*q0[3] + s1*q1_ptr[3];
		return q_out;
	}
	if (d > a3real_one)
		return a3real4NLerp(q_out, q0, q1_ptr, param);
	return a3quatSetReal4(q_out, q0);
}

A3_INLINE a3real4r a3quatSlerpIdentityQ0(a3real4p q_out, const a3real4p q1, const a3real param)
{
	const a3real lenSq = a3real4LengthSquared(q1);
	a3real d = q1[3] * a3sqrtInverse(lenSq);
	a3real4 q1_b;
	const a3real *q1_ptr = q1;
	
	// dot
	if (d < a3real_zero)
	{
		d = -d;
		q1_ptr = q1_b;
		a3real4GetNegative(q1_b, q1);
	}

	// slerp
	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		q_out[0] = s1*q1_ptr[0];
		q_out[1] = s1*q1_ptr[1];
		q_out[2] = s1*q1_ptr[2];
		q_out[3] = s0 + s1*q1_ptr[3];
		return q_out;
	}
	if (d > a3real_one)
		return a3quatSetReal4(q_out, q1_ptr);
	return a3quatSetIdentity(q_out);
}

A3_INLINE a3real4r a3quatSlerpIdentityQ1(a3real4p q_out, const a3real4p q0, const a3real param)
{
	const a3real lenSq = a3real4LengthSquared(q0);
	a3real d = q0[3] * a3sqrtInverse(lenSq);
	a3real w1 = a3real_one;
	if (d < a3real_zero)
	{
		d = -d;
		w1 = -a3real_one;
	}
	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		q_out[0] = s0*q0[0];
		q_out[1] = s0*q0[1];
		q_out[2] = s0*q0[2];
		q_out[3] = s0*q0[3] + s1*w1;
		return q_out;
	}
	if (d > a3real_one)
		return a3quatSetReal4(q_out, q0);
	return a3quatSetIdentity(q_out);
}

A3_INLINE a3real4r a3quatSlerpUnitIdentityQ0(a3real4p q_out, const a3real4p q1, const a3real param)
{
	a3real d = q1[3];
	a3real4 q1_b;
	const a3real *q1_ptr = q1;

	// dot
	if (d < a3real_zero)
	{
		d = -d;
		q1_ptr = q1_b;
		a3real4GetNegative(q1_b, q1);
	}

	// slerp
	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		q_out[0] = s1*q1_ptr[0];
		q_out[1] = s1*q1_ptr[1];
		q_out[2] = s1*q1_ptr[2];
		q_out[3] = s0 + s1*q1_ptr[3];
		return q_out;
	}
	if (d > a3real_one)
		return a3quatSetReal4(q_out, q1_ptr);
	return a3quatSetIdentity(q_out);
}

A3_INLINE a3real4r a3quatSlerpUnitIdentityQ1(a3real4p q_out, const a3real4p q0, const a3real param)
{
	a3real d = q0[3];
	a3real w1 = a3real_one;
	if (d < a3real_zero)
	{
		d = -d;
		w1 = -a3real_one;
	}
	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		q_out[0] = s0*q0[0];
		q_out[1] = s0*q0[1];
		q_out[2] = s0*q0[2];
		q_out[3] = s0*q0[3] + s1*w1;
		return q_out;
	}
	if (d > a3real_one)
		return a3quatSetReal4(q_out, q0);
	return a3quatSetIdentity(q_out);
}


// Gram-Schmidt orthonormalization for quaternions
///
A3_INLINE a3real4r a3quatGramSchmidtOrtho4(a3real4p q0_out, a3real4p q1_out, a3real4p q2_out, a3real4p q3_out, const a3real4p q0, const a3real4p q1, const a3real4p q2, const a3real4p q3, const a3real4p qBase)
{
	a3real4 proj0, proj1, proj2, proj3;
	a3real4Projected(proj0, q0, qBase);
	a3real4Diff(q0_out, q0, proj0);
	a3real4Projected(proj0, q1, qBase);
	a3real4Projected(proj1, q1, q0_out);
	a3real3Add(proj1, proj0);
	a3real4Diff(q1_out, q1, proj1);
	a3real4Projected(proj0, q2, qBase);
	a3real4Projected(proj1, q2, q0_out);
	a3real4Projected(proj2, q2, q1_out);
	a3real3Add(proj2, proj0);
	a3real3Add(proj2, proj1);
	a3real4Diff(q2_out, q2, proj2);
	a3real4Projected(proj0, q3, qBase);
	a3real4Projected(proj1, q3, q0_out);
	a3real4Projected(proj2, q3, q1_out);
	a3real4Projected(proj3, q3, q2_out);
	a3real3Add(proj3, proj0);
	a3real3Add(proj3, proj1);
	a3real3Add(proj3, proj2);
	return a3real4Diff(q3_out, q3, proj3);
}

A3_INLINE a3real4r a3quatGramSchmidt4(a3real4p q0_inout, a3real4p q1_inout, a3real4p q2_inout, a3real4p q3_inout, const a3real4p qBase)
{
	a3real4 proj0, proj1, proj2, proj3;
	a3real4Projected(proj0, q0_inout, qBase);
	a3real4Sub(q0_inout, proj0);
	a3real4Projected(proj0, q1_inout, qBase);
	a3real4Projected(proj1, q1_inout, q0_inout);
	a3real3Add(proj1, proj0);
	a3real4Sub(q1_inout, proj1);
	a3real4Projected(proj0, q2_inout, qBase);
	a3real4Projected(proj1, q2_inout, q0_inout);
	a3real4Projected(proj2, q2_inout, q1_inout);
	a3real3Add(proj2, proj0);
	a3real3Add(proj2, proj1);
	a3real4Sub(q2_inout, proj2);
	a3real4Projected(proj0, q3_inout, qBase);
	a3real4Projected(proj1, q3_inout, q0_inout);
	a3real4Projected(proj2, q3_inout, q1_inout);
	a3real4Projected(proj3, q3_inout, q2_inout);
	a3real3Add(proj3, proj0);
	a3real3Add(proj3, proj1);
	a3real3Add(proj3, proj2);
	return a3real4Sub(q3_inout, proj3);
}


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_QUATERNION_IMPL_INL
#endif	// __ANIMAL3D_A3DM_QUATERNION_H