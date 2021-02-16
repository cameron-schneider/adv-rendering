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

	a3dualquaternion_impl.inl
	Implementations for dual quaternion functions.
*/

#ifdef __ANIMAL3D_A3DM_DUALQUATERNION_H
#ifndef __ANIMAL3D_A3DM_DUALQUATERNION_IMPL_INL
#define __ANIMAL3D_A3DM_DUALQUATERNION_IMPL_INL


//-----------------------------------------------------------------------------
// CUT FROM HEADER
/*
//////// setters
A3_INLINE a3real4x2r a3dualquatSetAxisAngleScale(a3real4x2p Q_out, const a3real3p unitAxis, const a3real degrees, const a3real s);
A3_INLINE a3real4x2r a3dualquatSetAxisAngleScaleTranslate(a3real4x2p Q_out, const a3real3p unitAxis, const a3real degrees, const a3real s, const a3real3p translate);
A3_INLINE a3real4x2r a3dualquatSetEulerScale(a3real4x2p Q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z, const a3boolean eulerIsXYZ, const a3real s);
A3_INLINE a3real4x2r a3dualquatSetEulerScaleTranslate(a3real4x2p Q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z, const a3boolean eulerIsXYZ, const a3real s, const a3real3p translate);
A3_INLINE a3real4x2r a3dualquatSetScale(a3real4x2p Q_out, const a3real s);
A3_INLINE a3real4x2r a3dualquatSetScaleTranslate(a3real4x2p Q_out, const a3real s, const a3real3p translate);

//////// unary
A3_INLINE a3real4x2r a3dualquatGetInverse(a3real4x2p Q_out, const a3real4x2p Q);
A3_INLINE a3real4x2r a3dualquatGetInverseGetScale(a3real4x2p Q_out, a3real *s_out, const a3real4x2p Q);
A3_INLINE a3real4x2r a3dualquatInvert(a3real4x2p Q_inout);
A3_INLINE a3real4x2r a3dualquatInvertGetScale(a3real4x2p Q_inout, a3real *s_out);
A3_INLINE a3real4x2rk a3dualquatGetAxisAngle(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out);
A3_INLINE a3real4x2rk a3dualquatGetAxisAngleScale(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out, a3real *s_out);
A3_INLINE a3real4x2rk a3dualquatGetAxisAngleTranslate(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out, a3real3p translate_out);
A3_INLINE a3real4x2rk a3dualquatGetAxisAngleScaleTranslate(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out, a3real *s_out, a3real3p translate_out);

//////// conversion
A3_INLINE a3real4x4r a3dualquatConvertToMat4(a3real4x4p m_out, const a3real4x2 Q);
A3_INLINE a3real4x3r a3dualquatConvertToTransform(a3real4x3p t_out, a3boolean flag_out[], const a3real4x2p Q);
A3_INLINE a3real4x3r a3dualquatConvertToTransformIgnoreScale(a3real4x3p t_out, a3boolean flag_out[], const a3real4x2p Q);

//////// binary
A3_INLINE a3real3r a3dualquatVec3GetTransformed(a3real3p v_out, const a3real3p v, const a3real4x2p Q);
A3_INLINE a3real3r a3dualquatVec3Transform(a3real3p v_inout, const a3real4x2p Q);

//////// interpolation
A3_INLINE a3real4x4r a3dualquatSclerp(a3real4x2p Q_out, const a3real4x2p Q0, const a3real4x2p Q1, const a3real param);
A3_INLINE a3real4x4r a3dualquatSclerpIdentityQ0(a3real4x2p Q_out, const a3real4x2p Q1, const a3real param);
A3_INLINE a3real4x4r a3dualquatSclerpIdentityQ1(a3real4x2p Q_out, const a3real4x2p Q0, const a3real param);

//////// utilities
A3_INLINE a3real4x2rk a3dualquatCalculateScrewParams(const a3real4x2p Q, a3real3p unitAxis_out, a3real3p moment_out, a3real3p translate_out, a3real *degrees_out, a3real *distance_out, a3real *scale_out);
*/

A3_INLINE a3real3r a3dualquatCalculateTranslate(a3real3p translate_out, const a3real4p r, const a3real4p d);


//-----------------------------------------------------------------------------


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// identity transform
const a3dualquat a3dualquat_identity = {
	a3real_zero, a3real_zero, a3real_zero, a3real_one,		// rotation (identity)
	a3real_zero, a3real_zero, a3real_zero, a3real_zero,		// translation (zero)
};


//-----------------------------------------------------------------------------
// dual quaternion functions

// create dual quaternion using different transformation combinations
///
A3_INLINE a3real4x2r a3dualquatSetIdentity(a3real4x2p Q_out)
{
	a3real4SetReal4(Q_out[a3dualquatComp_real], a3vec4_w.v);
	a3real4SetReal4(Q_out[a3dualquatComp_dual], a3vec4_zero.v);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSet(a3real4x2p Q_out, const a3real4 rotateScaleQuat, const a3real4 dualPartQuat)
{
	a3real4SetReal4(Q_out[a3dualquatComp_real], rotateScaleQuat);
	a3real4SetReal4(Q_out[a3dualquatComp_dual], dualPartQuat);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetAxisAngle(a3real4x2p Q_out, const a3real3p unitAxis, const a3real degrees)
{
	a3quatSetAxisAngle(Q_out[a3dualquatComp_real], unitAxis, degrees);
	a3real4SetReal4(Q_out[a3dualquatComp_dual], a3vec4_zero.v);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetAxisAngleScale(a3real4x2p Q_out, const a3real3p unitAxis, const a3real degrees, const a3real s)
{
	a3quatSetAxisAngleScale(Q_out[a3dualquatComp_real], unitAxis, degrees, s);
	a3real4SetReal4(Q_out[a3dualquatComp_dual], a3vec4_zero.v);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetAxisAngleTranslate(a3real4x2p Q_out, const a3real3p unitAxis, const a3real degrees, const a3real3p translate)
{
	a3quatSetAxisAngle(Q_out[a3dualquatComp_real], unitAxis, degrees);
	a3dualquatCalculateDualPart(Q_out[a3dualquatComp_dual], Q_out[a3dualquatComp_real], translate);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetAxisAngleScaleTranslate(a3real4x2p Q_out, const a3real3p unitAxis, const a3real degrees, const a3real s, const a3real3p translate)
{
	a3quatSetAxisAngleScale(Q_out[a3dualquatComp_real], unitAxis, degrees, s);
	a3dualquatCalculateDualPart(Q_out[a3dualquatComp_dual], Q_out[a3dualquatComp_real], translate);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetEuler(a3real4x2p Q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z, const a3boolean eulerIsXYZ)
{
	(eulerIsXYZ ? a3quatSetEulerXYZ : a3quatSetEulerZYX)(Q_out[a3dualquatComp_real], degrees_x, degrees_y, degrees_z);
	a3real4SetReal4(Q_out[a3dualquatComp_dual], a3vec4_zero.v);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetEulerScale(a3real4x2p Q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z, const a3boolean eulerIsXYZ, const a3real s)
{
	(eulerIsXYZ ? a3quatSetEulerXYZScale : a3quatSetEulerZYXScale)(Q_out[a3dualquatComp_real], degrees_x, degrees_y, degrees_z, s);
	a3real4SetReal4(Q_out[a3dualquatComp_dual], a3vec4_zero.v);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetEulerTranslate(a3real4x2p Q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z, const a3boolean eulerIsXYZ, const a3real3p translate)
{
	(eulerIsXYZ ? a3quatSetEulerXYZ : a3quatSetEulerZYX)(Q_out[a3dualquatComp_real], degrees_x, degrees_y, degrees_z);
	a3dualquatCalculateDualPart(Q_out[a3dualquatComp_dual], Q_out[a3dualquatComp_real], translate);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetEulerScaleTranslate(a3real4x2p Q_out, const a3real degrees_x, const a3real degrees_y, const a3real degrees_z, const a3boolean eulerIsXYZ, const a3real s, const a3real3p translate)
{
	(eulerIsXYZ ? a3quatSetEulerXYZScale : a3quatSetEulerZYXScale)(Q_out[a3dualquatComp_real], degrees_x, degrees_y, degrees_z, s);
	a3dualquatCalculateDualPart(Q_out[a3dualquatComp_dual], Q_out[a3dualquatComp_real], translate);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetScale(a3real4x2p Q_out, const a3real s)
{
	a3quatSetScale(Q_out[a3dualquatComp_real], s);
	a3real4SetReal4(Q_out[a3dualquatComp_dual], a3vec4_zero.v);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetTranslate(a3real4x2p Q_out, const a3real3p translate)
{
	a3real4SetReal4(Q_out[a3dualquatComp_real], a3vec4_w.v);
	a3real3ProductS(Q_out[a3dualquatComp_dual], translate, a3real_half);
	Q_out[a3dualquatComp_dual][3] = a3real_zero;
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSetScaleTranslate(a3real4x2p Q_out, const a3real s, const a3real3p translate)
{
	a3quatSetScale(Q_out[a3dualquatComp_real], s);
	a3real3ProductS(Q_out[a3dualquatComp_dual], translate, a3real_half*Q_out[a3dualquatComp_real][0]);
	Q_out[a3dualquatComp_dual][3] = a3real_zero;
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatCopy(a3real4x2p Q_out, const a3real4x2p Q)
{
	a3real4SetReal4(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real]);
	a3real4SetReal4(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual]);
	return Q_out;
}


// unary operations
///
A3_INLINE a3real4x2r a3dualquatGetConjugated(a3real4x2p Q_out, const a3real4x2p Q)
{
	// conjugate both parts
	a3quatGetConjugated(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real]);
	a3quatGetConjugated(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual]);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatGetInverse(a3real4x2p Q_out, const a3real4x2p Q)
{
	// divide both parts by a3real part's length squared
	const a3real lenSqInv = a3real4LengthSquaredInverse(Q_out[a3dualquatComp_real]);
	a3real4ProductS(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real], lenSqInv);
	a3real4ProductS(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual], lenSqInv);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatGetInverseGetScale(a3real4x2p Q_out, a3real *s_out, const a3real4x2p Q)
{
	const a3real lenSq = *s_out = a3real4LengthSquared(Q_out[a3dualquatComp_real]);
	const a3real lenSqInv = a3recip(lenSq);
	a3real4ProductS(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real], lenSqInv);
	a3real4ProductS(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual], lenSqInv);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatGetInverseIgnoreScale(a3real4x2p Q_out, const a3real4x2p Q)
{
	// just the conjugate
	a3quatGetConjugated(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real]);
	a3quatGetConjugated(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual]);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatConjugate(a3real4x2p Q_inout)
{
	a3quatConjugate(Q_inout[a3dualquatComp_real]);
	a3quatConjugate(Q_inout[a3dualquatComp_dual]);
	return Q_inout;
}

A3_INLINE a3real4x2r a3dualquatInvert(a3real4x2p Q_inout)
{
	const a3real lenSqInv = a3real4LengthSquaredInverse(Q_inout[a3dualquatComp_real]);
	a3real4MulS(Q_inout[a3dualquatComp_real], lenSqInv);
	a3real4MulS(Q_inout[a3dualquatComp_dual], lenSqInv);
	return Q_inout;
}

A3_INLINE a3real4x2r a3dualquatInvertGetScale(a3real4x2p Q_inout, a3real *s_out)
{
	const a3real lenSq = *s_out = a3real4LengthSquared(Q_inout[a3dualquatComp_real]);
	const a3real lenSqInv = a3recip(lenSq);
	a3real4MulS(Q_inout[a3dualquatComp_real], lenSqInv);
	a3real4MulS(Q_inout[a3dualquatComp_dual], lenSqInv);
	return Q_inout;
}

A3_INLINE a3real4x2r a3dualquatInvertIgnoreScale(a3real4x2p Q_inout)
{
	a3quatConjugate(Q_inout[a3dualquatComp_real]);
	a3quatConjugate(Q_inout[a3dualquatComp_dual]);
	return Q_inout;
}

A3_INLINE a3real4x2rk a3dualquatGetAxisAngle(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out)
{
	a3quatGetAxisAngle(Q[a3dualquatComp_real], unitAxis_out, degrees_out);
	return Q;
}

A3_INLINE a3real4x2rk a3dualquatGetAxisAngleScale(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out, a3real *s_out)
{
	a3quatGetAxisAngleScale(Q[a3dualquatComp_real], unitAxis_out, degrees_out, s_out);
	return Q;
}

A3_INLINE a3real4x2rk a3dualquatGetAxisAngleTranslate(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out, a3real3p translate_out)
{
	a3quatGetAxisAngle(Q[a3dualquatComp_real], unitAxis_out, degrees_out);
	a3dualquatCalculateTranslate(translate_out, Q[a3dualquatComp_real], Q[a3dualquatComp_dual]);
	return Q;
}

A3_INLINE a3real4x2rk a3dualquatGetAxisAngleScaleTranslate(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out, a3real *s_out, a3real3p translate_out)
{
	a3quatGetAxisAngleScale(Q[a3dualquatComp_real], unitAxis_out, degrees_out, s_out);
	a3dualquatCalculateTranslate(translate_out, Q[a3dualquatComp_real], Q[a3dualquatComp_dual]);
	return Q;
}

A3_INLINE a3real4x2rk a3dualquatGetAxisAngleIgnoreScale(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out)
{
	a3quatGetAxisAngleIgnoreScale(Q[a3dualquatComp_real], unitAxis_out, degrees_out);
	return Q;
}

A3_INLINE a3real4x2rk a3dualquatGetAxisAngleTranslateIgnoreScale(const a3real4x2p Q, a3real3p unitAxis_out, a3real *degrees_out, a3real3p translate_out)
{
	a3quatGetAxisAngleIgnoreScale(Q[a3dualquatComp_real], unitAxis_out, degrees_out);
	a3dualquatCalculateTranslateIgnoreScale(translate_out, Q[a3dualquatComp_real], Q[a3dualquatComp_dual]);
	return Q;
}


// other unary functions
// note: the magnitude of a dual quaternion is represented by the magnitude 
//	of the a3real part alone (proven mathematically)
///
A3_INLINE a3real a3dualquatLengthSquared(const a3real4x2p Q)
{
	return a3real4LengthSquared(Q[a3dualquatComp_real]);
}

A3_INLINE a3real a3dualquatLengthSquaredInverse(const a3real4x2p Q)
{
	return a3real4LengthSquaredInverse(Q[a3dualquatComp_real]);
}

A3_INLINE a3real a3dualquatLength(const a3real4x2p Q)
{
	return a3real4Length(Q[a3dualquatComp_real]);
}

A3_INLINE a3real a3dualquatLengthInverse(const a3real4x2p Q)
{
	return a3real4LengthInverse(Q[a3dualquatComp_real]);
}

A3_INLINE a3real4x2r a3dualquatGetUnit(a3real4x2p Q_out, const a3real4x2p Q)
{
	const a3real lenInv = a3real4LengthInverse(Q[a3dualquatComp_real]);
	a3real4ProductS(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real], lenInv);
	a3real4ProductS(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual], lenInv);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatGetUnitInvLength(a3real4x2p Q_out, const a3real4x2p Q, a3real *invLength_out)
{
	const a3real lenInv = *invLength_out = a3real4LengthInverse(Q[a3dualquatComp_real]);
	a3real4ProductS(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real], lenInv);
	a3real4ProductS(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual], lenInv);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatNormalize(a3real4x2p Q_inout)
{
	const a3real lenInv = a3real4LengthInverse(Q_inout[a3dualquatComp_real]);
	a3real4MulS(Q_inout[a3dualquatComp_real], lenInv);
	a3real4MulS(Q_inout[a3dualquatComp_dual], lenInv);
	return Q_inout;
}

A3_INLINE a3real4x2r a3dualquatNormalizeGetInvLength(a3real4x2p Q_inout, a3real *invLength_out)
{
	const a3real lenInv = *invLength_out = a3real4LengthInverse(Q_inout[a3dualquatComp_real]);
	a3real4MulS(Q_inout[a3dualquatComp_real], lenInv);
	a3real4MulS(Q_inout[a3dualquatComp_dual], lenInv);
	return Q_inout;
}


// conversion to matrix or transform
///
A3_INLINE a3real4x4r a3dualquatConvertToMat4(a3real4x4p m_out, const a3real4x2 Q)
{
	a3quatConvertToMat4(m_out, Q[a3dualquatComp_real]);
	a3dualquatCalculateTranslate(m_out[3], Q[a3dualquatComp_real], Q[a3dualquatComp_dual]);
	return m_out;
}

A3_INLINE a3real4x4r a3dualquatConvertToMat4IgnoreScale(a3real4x4p m_out, const a3real4x2 Q)
{
	a3quatConvertToMat4(m_out, Q[a3dualquatComp_real]);
	a3dualquatCalculateTranslateIgnoreScale(m_out[3], Q[a3dualquatComp_real], Q[a3dualquatComp_dual]);
	return m_out;
}

A3_INLINE a3real4x3r a3dualquatConvertToTransform(a3real4x3p t_out, a3boolean flag_out[], const a3real4x2p Q)
{
	// get the squared length of the a3real part because that is the uniform scale
	// use its sqrt to normalize the a3real part for the rotation
	// calculate the translation without accounting for the scale, divide by squared length
//	const a3real lenSq = a3real4LengthSquared(Q[a3dualquatComp_real]);
//	const a3real lenInv = a3sqrtInverse(lenSq);
//	a3real4ProductS(t_out[a3transformComp_rotate], Q[a3dualquatComp_real], lenInv);
//	a3real4Set(t_out[a3transformComp_scale], lenSq, lenSq, lenSq, a3real_one);
//	a3dualquatCalculateTranslateIgnoreScale(t_out[a3transformComp_translate], Q[a3dualquatComp_real], Q[a3dualquatComp_dual]);
//	a3real3DivS(t_out[a3transformComp_translate], lenSq);
//	t_out[a3transformComp_translate][3] = a3real_one;
//	flag_out[a3transformFlag_identity] = flag_out[a3transformFlag_identityRotation] = flag_out[a3transformFlag_unitScale] = 0;
//	flag_out[a3transformFlag_uniformScale] = 1;
	return t_out;
}

A3_INLINE a3real4x3r a3dualquatConvertToTransformIgnoreScale(a3real4x3p t_out, a3boolean flag_out[], const a3real4x2p Q)
{
	// copy a3real part for rotation and all ones for scale, calculate translate
//	a3real4SetReal4(t_out[a3transformComp_rotate], Q[a3dualquatComp_real]);
//	a3real4SetReal4(t_out[a3transformComp_scale], a3vec4_one.v);
//	a3dualquatCalculateTranslateIgnoreScale(t_out[a3transformComp_translate], Q[a3dualquatComp_real], Q[a3dualquatComp_dual]);
//	t_out[a3transformComp_translate][3] = a3real_one;
//	flag_out[a3transformFlag_identity] = flag_out[a3transformFlag_identityRotation] = 0;
//	flag_out[a3transformFlag_unitScale] = flag_out[a3transformFlag_uniformScale] = 1;
	return t_out;
}


// binary operations
///
A3_INLINE a3real4x2r a3dualquatSum(a3real4x2p Q_out, const a3real4x2p QL, const a3real4x2p QR)
{
	a3real4Sum(Q_out[a3dualquatComp_real], QL[a3dualquatComp_real], QR[a3dualquatComp_real]);
	a3real4Sum(Q_out[a3dualquatComp_dual], QL[a3dualquatComp_dual], QR[a3dualquatComp_dual]);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatDiff(a3real4x2p Q_out, const a3real4x2p QL, const a3real4x2p QR)
{
	a3real4Diff(Q_out[a3dualquatComp_real], QL[a3dualquatComp_real], QR[a3dualquatComp_real]);
	a3real4Diff(Q_out[a3dualquatComp_dual], QL[a3dualquatComp_dual], QR[a3dualquatComp_dual]);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatProductS(a3real4x2p Q_out, const a3real4x2p Q, const a3real s)
{
	a3real4ProductS(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real], s);
	a3real4ProductS(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual], s);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatQuotientS(a3real4x2p Q_out, const a3real4x2p Q, const a3real s)
{
	a3real4QuotientS(Q_out[a3dualquatComp_real], Q[a3dualquatComp_real], s);
	a3real4QuotientS(Q_out[a3dualquatComp_dual], Q[a3dualquatComp_dual], s);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatAdd(a3real4x2p QL_inout, const a3real4x2p QR)
{
	a3real4Add(QL_inout[a3dualquatComp_real], QR[a3dualquatComp_real]);
	a3real4Add(QL_inout[a3dualquatComp_dual], QR[a3dualquatComp_dual]);
	return QL_inout;
}

A3_INLINE a3real4x2r a3dualquatSub(a3real4x2p QL_inout, const a3real4x2p QR)
{
	a3real4Sub(QL_inout[a3dualquatComp_real], QR[a3dualquatComp_real]);
	a3real4Sub(QL_inout[a3dualquatComp_dual], QR[a3dualquatComp_dual]);
	return QL_inout;
}

A3_INLINE a3real4x2r a3dualquatMulS(a3real4x2p Q_inout, const a3real s)
{
	a3real4MulS(Q_inout[a3dualquatComp_real], s);
	a3real4MulS(Q_inout[a3dualquatComp_dual], s);
	return Q_inout;
}

A3_INLINE a3real4x2r a3dualquatDivS(a3real4x2p Q_inout, const a3real s)
{
	a3real4DivS(Q_inout[a3dualquatComp_real], s);
	a3real4DivS(Q_inout[a3dualquatComp_dual], s);
	return Q_inout;
}


// other binary operations: dual quaternion concatenation
// can also transform vector using dual quaternion
///
A3_INLINE a3real4x2r a3dualquatProduct(a3real4x2p Q_out, const a3real4x2p QL, const a3real4x2 QR)
{
	// QL = rL + dL(e)
	// QR = rR + dR(e)
	// (QL)(QR) = (rL + dL(e))(rR + dR(e))
	//			= rLrR + rLdR(e) + dLrR(e) + dLdR(e^2)
	//			= rLrR + (rLdR + dLrR)(e)
	//			  ____	  ___________
	//			  a3real		  dual
	// instead of creating a temp variable, use both parts of Q_out to store the summands
	// then overwrite the a3real part with the first product
	a3quatProduct(Q_out[0], QL[a3dualquatComp_real], QR[a3dualquatComp_dual]);
	a3quatProduct(Q_out[1], QL[a3dualquatComp_dual], QR[a3dualquatComp_real]);
	a3real4Add(Q_out[1], Q_out[0]);
	a3quatProduct(Q_out[0], QL[a3dualquatComp_real], QR[a3dualquatComp_real]);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatConcatL(a3real4x2p QL_inout, const a3real4x2 QR)
{
	a3real4 tmp1, tmp2;
	a3quatProduct(tmp1, QL_inout[a3dualquatComp_real], QR[a3dualquatComp_dual]);
	a3quatProduct(tmp2, QL_inout[a3dualquatComp_dual], QR[a3dualquatComp_real]);
	a3real4Sum(QL_inout[a3dualquatComp_dual], tmp1, tmp2);
	a3quatConcatL(QL_inout[a3dualquatComp_real], QR[a3dualquatComp_real]);
	return QL_inout;
}

A3_INLINE a3real4x2r a3dualquatConcatR(const a3real4x2p QL, a3real4x2 QR_inout)
{
	a3real4 tmp1, tmp2;
	a3quatProduct(tmp1, QL[a3dualquatComp_real], QR_inout[a3dualquatComp_dual]);
	a3quatProduct(tmp2, QL[a3dualquatComp_dual], QR_inout[a3dualquatComp_real]);
	a3real4Sum(QR_inout[a3dualquatComp_dual], tmp1, tmp2);
	a3quatConcatR(QL[a3dualquatComp_real], QR_inout[a3dualquatComp_real]);
	return QR_inout;
}

A3_INLINE a3real3r a3dualquatVec3GetTransformed(a3real3p v_out, const a3real3p v, const a3real4x2p Q)
{
	const a3real *r = Q[a3dualquatComp_real], *d = Q[a3dualquatComp_dual];
	const a3real lenSq = a3real4LengthSquared(r);
	const a3real lenSqInv2 = a3real_two / lenSq;
	const a3real q2x = r[0] + r[0];
	const a3real q2y = r[1] + r[1];
	const a3real q2z = r[2] + r[2];
	const a3real cx = r[3] * v[0] + r[1] * v[2] - r[2] * v[1];
	const a3real cy = r[3] * v[1] + r[2] * v[0] - r[0] * v[2];
	const a3real cz = r[3] * v[2] + r[0] * v[1] - r[1] * v[0];
	const a3real txHalf = d[0] * r[3] + d[2] * r[1] - d[1] * r[2] - d[3] * r[0];
	const a3real tyHalf = d[1] * r[3] + d[0] * r[2] - d[2] * r[0] - d[3] * r[1];
	const a3real tzHalf = d[2] * r[3] + d[1] * r[0] - d[0] * r[1] - d[3] * r[2];
	v_out[0] = v[0] * lenSq + txHalf*lenSqInv2 + q2y*cz - q2z*cy;
	v_out[1] = v[1] * lenSq + tyHalf*lenSqInv2 + q2z*cx - q2x*cz;
	v_out[2] = v[2] * lenSq + tzHalf*lenSqInv2 + q2x*cy - q2y*cx;
	return v_out;
}

A3_INLINE a3real3r a3dualquatVec3GetTransformedIgnoreScale(a3real3p v_out, const a3real3p v, const a3real4x2p Q)
{
	const a3real *r = Q[a3dualquatComp_real], *d = Q[a3dualquatComp_dual];
	const a3real q2x = r[0] + r[0];
	const a3real q2y = r[1] + r[1];
	const a3real q2z = r[2] + r[2];
	const a3real cx = r[3] * v[0] + r[1] * v[2] - r[2] * v[1];
	const a3real cy = r[3] * v[1] + r[2] * v[0] - r[0] * v[2];
	const a3real cz = r[3] * v[2] + r[0] * v[1] - r[1] * v[0];
	const a3real txHalf = d[0] * r[3] + d[2] * r[1] - d[1] * r[2] - d[3] * r[0];
	const a3real tyHalf = d[1] * r[3] + d[0] * r[2] - d[2] * r[0] - d[3] * r[1];
	const a3real tzHalf = d[2] * r[3] + d[1] * r[0] - d[0] * r[1] - d[3] * r[2];
	v_out[0] = v[0] + txHalf + txHalf + q2y*cz - q2z*cy;
	v_out[1] = v[1] + tyHalf + tyHalf + q2z*cx - q2x*cz;
	v_out[2] = v[2] + tzHalf + tzHalf + q2x*cy - q2y*cx;
	return v_out;
}

A3_INLINE a3real3r a3dualquatVec3Transform(a3real3p v_inout, const a3real4x2p Q)
{
	const a3real *r = Q[a3dualquatComp_real], *d = Q[a3dualquatComp_dual];
	const a3real lenSq = a3real4LengthSquared(r);
	const a3real lenSqInv2 = a3real_two / lenSq;
	const a3real q2x = r[0] + r[0];
	const a3real q2y = r[1] + r[1];
	const a3real q2z = r[2] + r[2];
	const a3real cx = r[3] * v_inout[0] + r[1] * v_inout[2] - r[2] * v_inout[1];
	const a3real cy = r[3] * v_inout[1] + r[2] * v_inout[0] - r[0] * v_inout[2];
	const a3real cz = r[3] * v_inout[2] + r[0] * v_inout[1] - r[1] * v_inout[0];
	const a3real txHalf = d[0] * r[3] + d[2] * r[1] - d[1] * r[2] - d[3] * r[0];
	const a3real tyHalf = d[1] * r[3] + d[0] * r[2] - d[2] * r[0] - d[3] * r[1];
	const a3real tzHalf = d[2] * r[3] + d[1] * r[0] - d[0] * r[1] - d[3] * r[2];
	v_inout[0] = v_inout[0] * lenSq + txHalf*lenSqInv2 + q2y*cz - q2z*cy;
	v_inout[1] = v_inout[1] * lenSq + tyHalf*lenSqInv2 + q2z*cx - q2x*cz;
	v_inout[2] = v_inout[2] * lenSq + tzHalf*lenSqInv2 + q2x*cy - q2y*cx;
	return v_inout;
}

A3_INLINE a3real3r a3dualquatVec3TransformIgnoreScale(a3real3p v_inout, const a3real4x2p Q)
{
	const a3real *r = Q[a3dualquatComp_real], *d = Q[a3dualquatComp_dual];
	const a3real q2x = r[0] + r[0];
	const a3real q2y = r[1] + r[1];
	const a3real q2z = r[2] + r[2];
	const a3real cx = r[3] * v_inout[0] + r[1] * v_inout[2] - r[2] * v_inout[1];
	const a3real cy = r[3] * v_inout[1] + r[2] * v_inout[0] - r[0] * v_inout[2];
	const a3real cz = r[3] * v_inout[2] + r[0] * v_inout[1] - r[1] * v_inout[0];
	const a3real txHalf = d[0] * r[3] + d[2] * r[1] - d[1] * r[2] - d[3] * r[0];
	const a3real tyHalf = d[1] * r[3] + d[0] * r[2] - d[2] * r[0] - d[3] * r[1];
	const a3real tzHalf = d[2] * r[3] + d[1] * r[0] - d[0] * r[1] - d[3] * r[2];
	v_inout[0] += txHalf + txHalf + q2y*cz - q2z*cy;
	v_inout[1] += tyHalf + tyHalf + q2z*cx - q2x*cz;
	v_inout[2] += tzHalf + tzHalf + q2x*cy - q2y*cx;
	return v_inout;
}


// interpolation: dual quaternion ScLERP (screw linear interpolation)
// essentially SLERP for dual quaternions, accounting for the translation part
// DLB (dual linear blend) is also available, note the result will be normalized
// all versions assume non-zero scale
///
A3_INLINE a3real4x2r a3dualquatSclerp(a3real4x2p Q_out, const a3real4x2p Q0, const a3real4x2p Q1, const a3real param)
{
	const a3real *q0 = Q0[a3dualquatComp_real], *q1 = Q1[a3dualquatComp_real], *d0 = Q0[a3dualquatComp_dual], *d1 = Q1[a3dualquatComp_dual];
	const a3real lenSq = a3real4LengthSquared(q0) * a3real4LengthSquared(q1);
	a3real d = a3real4Dot(q0, q1) * a3sqrtInverse(lenSq);
	a3real4 q1_b, d1_b;
	const a3real *q1_ptr = q1, *d1_ptr = d1;

	if (d < a3real_zero)
	{
		d = -d;
		q1_ptr = q1_b;
		d1_ptr = d1_b;
		a3real4GetNegative(q1_b, q1);
		a3real4GetNegative(d1_b, d1);
	}

	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		a3quatSet(Q_out[a3dualquatComp_real], s0*q0[0] + s1*q1_ptr[0], s0*q0[1] + s1*q1_ptr[1], s0*q0[2] + s1*q1_ptr[2], s0*q0[3] + s1*q1_ptr[3]);
		a3quatSet(Q_out[a3dualquatComp_dual], s0*d0[0] + s1*d1_ptr[0], s0*d0[1] + s1*d1_ptr[1], s0*d0[2] + s1*d1_ptr[2], s0*d0[3] + s1*d1_ptr[3]);
		return Q_out;
	}
	return a3dualquatCopy(Q_out, Q0);
}

A3_INLINE a3real4x2r a3dualquatSclerpUnit(a3real4x2p Q_out, const a3real4x2p Q0, const a3real4x2p Q1, const a3real param)
{
	const a3real *q0 = Q0[a3dualquatComp_real], *q1 = Q1[a3dualquatComp_real], *d0 = Q0[a3dualquatComp_dual], *d1 = Q1[a3dualquatComp_dual];
	a3real d = a3real4Dot(q0, q1);
	a3real4 q1_b, d1_b;
	const a3real *q1_ptr = q1, *d1_ptr = d1;

	if (d < a3real_zero)
	{
		d = -d;
		q1_ptr = q1_b;
		d1_ptr = d1_b;
		a3real4GetNegative(q1_b, q1);
		a3real4GetNegative(d1_b, d1);
	}

	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		a3quatSet(Q_out[a3dualquatComp_real], s0*q0[0] + s1*q1_ptr[0], s0*q0[1] + s1*q1_ptr[1], s0*q0[2] + s1*q1_ptr[2], s0*q0[3] + s1*q1_ptr[3]);
		a3quatSet(Q_out[a3dualquatComp_dual], s0*d0[0] + s1*d1_ptr[0], s0*d0[1] + s1*d1_ptr[1], s0*d0[2] + s1*d1_ptr[2], s0*d0[3] + s1*d1_ptr[3]);
		return Q_out;
	}
	return a3dualquatCopy(Q_out, Q0);
}

A3_INLINE a3real4x2r a3dualquatSclerpIdentityQ0(a3real4x2p Q_out, const a3real4x2p Q1, const a3real param)
{
	const a3real *q1 = Q1[a3dualquatComp_real], *d1 = Q1[a3dualquatComp_dual];
	const a3real lenSq = a3real4LengthSquared(q1);
	a3real d = q1[3] * a3sqrtInverse(lenSq);
	a3real4 q1_b, d1_b;
	const a3real *q1_ptr = q1, *d1_ptr = d1;

	if (d < a3real_zero)
	{
		d = -d;
		q1_ptr = q1_b;
		d1_ptr = d1_b;
		a3real4GetNegative(q1_b, q1);
		a3real4GetNegative(d1_b, d1);
	}

	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		a3quatSet(Q_out[a3dualquatComp_real], s1*q1_b[0], s1*q1_b[1], s1*q1_b[2], s0 + s1*q1_b[3]);
		a3quatSet(Q_out[a3dualquatComp_dual], s1*d1_b[0], s1*d1_b[1], s1*d1_b[2], s1*d1_b[3]);	// no w component for s0 because identity w is zero for translate
		return Q_out;
	}
	return a3dualquatSetIdentity(Q_out);
}

A3_INLINE a3real4x2r a3dualquatSclerpIdentityQ1(a3real4x2p Q_out, const a3real4x2p Q0, const a3real param)
{
	const a3real *q0 = Q0[a3dualquatComp_real], *d0 = Q0[a3dualquatComp_dual];
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
		a3quatSet(Q_out[a3dualquatComp_real], s0*q0[0], s0*q0[1], s0*q0[2], s0*q0[3] + s1*w1);
		a3quatSet(Q_out[a3dualquatComp_dual], s0*d0[0], s0*d0[1], s0*d0[2], s0*d0[3]);	// ditto
	}
	else
		a3dualquatSetIdentity(Q_out);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatSclerpUnitIdentityQ0(a3real4x2p Q_out, const a3real4x2p Q1, const a3real param)
{
	const a3real *q1 = Q1[a3dualquatComp_real], *d1 = Q1[a3dualquatComp_dual];
	a3real d = q1[3];
	a3real4 q1_b, d1_b;
	const a3real *q1_ptr = q1, *d1_ptr = d1;

	if (d < a3real_zero)
	{
		d = -d;
		q1_ptr = q1_b;
		d1_ptr = d1_b;
		a3real4GetNegative(q1_b, q1);
		a3real4GetNegative(d1_b, d1);
	}

	if (d < a3real_one)
	{
		const a3real ac = a3acosd(d);
		const a3real s = a3cscd(ac);
		const a3real s1 = s * a3sind(param*ac);
		const a3real s0 = s * a3sind(ac - param*ac);
		a3quatSet(Q_out[a3dualquatComp_real], s1*q1_b[0], s1*q1_b[1], s1*q1_b[2], s0 + s1*q1_b[3]);
		a3quatSet(Q_out[a3dualquatComp_dual], s1*d1_b[0], s1*d1_b[1], s1*d1_b[2], s1*d1_b[3]);
		return Q_out;
	}
	return a3dualquatSetIdentity(Q_out);
}

A3_INLINE a3real4x2r a3dualquatSclerpUnitIdentityQ1(a3real4x2p Q_out, const a3real4x2p Q0, const a3real param)
{
	const a3real *q0 = Q0[a3dualquatComp_real], *d0 = Q0[a3dualquatComp_dual];
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
		a3quatSet(Q_out[a3dualquatComp_real], s0*q0[0], s0*q0[1], s0*q0[2], s0*q0[3] + s1*w1);
		a3quatSet(Q_out[a3dualquatComp_dual], s0*d0[0], s0*d0[1], s0*d0[2], s0*d0[3]);
	}
	else
		a3dualquatSetIdentity(Q_out);
	return Q_out;
}

A3_INLINE a3real4x2r a3dualquatDLB2(a3real4x2p Q_out, const a3real4x2p Q0, const a3real4x2p Q1)
{
	a3dualquatSum(Q_out, Q0, Q1);
	return a3dualquatNormalize(Q_out);
}

A3_INLINE a3real4x2r a3dualquatDLB4(a3real4x2p Q_out, const a3real4x2p Q0, const a3real4x2p Q1, const a3real4x2p Q2, const a3real4x2p Q3)
{
	a3dualquatSum(Q_out, Q0, Q1);
	a3dualquatAdd(Q_out, Q2);
	a3dualquatAdd(Q_out, Q3);
	return a3dualquatNormalize(Q_out);
}

A3_INLINE a3real4x2r a3dualquatDLB(a3real4x2p Q_out, const a3real4x2 Q[], const a3count count)
{
	const a3real4x2 *end = Q + count;
	a3dualquatSum(Q_out, Q[0], Q[1]);
	++Q;
	while (++Q < end)
		a3dualquatAdd(Q_out, *Q);
	return a3dualquatNormalize(Q_out);
}


// utilities to calculate the dual part given a a3real part and translation vector, 
//	and calculate translation given a3real and dual parts
///
A3_INLINE a3real4r a3dualquatCalculateDualPart(a3real4p d_out, const a3real4p r, const a3real3p translate)
{
	// s = 0 + 1/2 t (pure quaternion)
	// d = sr
	a3real3ProductS(d_out, translate, a3real_half);
	a3quatConcatLPureL(d_out, r);
	return d_out;
}

A3_INLINE a3real3r a3dualquatCalculateTranslate(a3real3p translate_out, const a3real4p r, const a3real4p d)
{
	// t = 2s
	// s = dr^-1 = dr* / |r|^2
	// t = 2dr^-1 = dr* (2 / |r|^2)
	const a3real lenSqInv2 = a3real_two / a3real4LengthSquared(r);

	// ignore w because it will be zero; do conjugate and multiplication at the same time
	translate_out[0] = lenSqInv2 * (d[0] * r[3] + d[2] * r[1] - d[1] * r[2] - d[3] * r[0]);
	translate_out[1] = lenSqInv2 * (d[1] * r[3] + d[0] * r[2] - d[2] * r[0] - d[3] * r[1]);
	translate_out[2] = lenSqInv2 * (d[2] * r[3] + d[1] * r[0] - d[0] * r[1] - d[3] * r[2]);
	return translate_out;
}

A3_INLINE a3real3r a3dualquatCalculateTranslateIgnoreScale(a3real3p translate_out, const a3real4p r, const a3real4p d)
{
	// t = 2s
	// s = dr*
	// t = 2dr*
	translate_out[0] = a3real_two * (d[0] * r[3] + d[2] * r[1] - d[1] * r[2] - d[3] * r[0]);
	translate_out[1] = a3real_two * (d[1] * r[3] + d[0] * r[2] - d[2] * r[0] - d[3] * r[1]);
	translate_out[2] = a3real_two * (d[2] * r[3] + d[1] * r[0] - d[0] * r[1] - d[3] * r[2]);
	return translate_out;
}


// get screw parameters (kinematics)
///
A3_INLINE a3real4x2rk a3dualquatCalculateScrewParams(const a3real4x2p Q, a3real3p unitAxis_out, a3real3p moment_out, a3real3p translate_out, a3real *degrees_out, a3real *distance_out, a3real *scale_out)
{
	const a3real *r = Q[a3dualquatComp_real], *d = Q[a3dualquatComp_dual];
	const a3real lenSq = a3real4LengthSquared(Q[a3dualquatComp_real]);
	const a3real lenSqInv2 = a3real_two / lenSq;
	const a3real lenInv = a3sqrtInverse(lenSq);
	const a3real ac = a3acosd(r[3] * lenInv), cot = a3atand(ac), s = (ac != a3real_zero ? lenInv / a3sind(ac) : a3real_zero);
	a3real distance;


	translate_out[0] = lenSqInv2 * (d[0] * r[3] + d[2] * r[1] - d[1] * r[2] - d[3] * r[0]);
	translate_out[1] = lenSqInv2 * (d[1] * r[3] + d[0] * r[2] - d[2] * r[0] - d[3] * r[1]);
	translate_out[2] = lenSqInv2 * (d[2] * r[3] + d[1] * r[0] - d[0] * r[1] - d[3] * r[2]);

	unitAxis_out[0] = r[0] * s;
	unitAxis_out[1] = r[1] * s;
	unitAxis_out[2] = r[2] * s;

	*degrees_out = ac + ac;
	*distance_out = distance = a3real3Dot(translate_out, unitAxis_out);
	*scale_out = lenSq;

	// moment = 1/2 * ( cross(translate, axis) + (translate - distance*axis)*cot(halfAngle) )
	moment_out[0] = a3real_half*(translate_out[1]*unitAxis_out[2] - translate_out[2]*unitAxis_out[1] + cot*(translate_out[0] - distance*unitAxis_out[0]));
	moment_out[1] = a3real_half*(translate_out[2]*unitAxis_out[0] - translate_out[0]*unitAxis_out[2] + cot*(translate_out[1] - distance*unitAxis_out[1]));
	moment_out[2] = a3real_half*(translate_out[0]*unitAxis_out[1] - translate_out[1]*unitAxis_out[0] + cot*(translate_out[2] - distance*unitAxis_out[2]));
	return Q;
}

A3_INLINE a3real4x2rk a3dualquatCalculateScrewParamsIgnoreScale(const a3real4x2p Q, a3real3p unitAxis_out, a3real3p moment_out, a3real3p translate_out, a3real *degrees_out, a3real *distance_out)
{
	const a3real *r = Q[a3dualquatComp_real], *d = Q[a3dualquatComp_dual];
	const a3real ac = a3acosd(r[3]), cot = a3atand(ac), s = (ac != a3real_zero ? a3real_one / a3sind(ac) : a3real_zero);
	a3real distance;

	translate_out[0] = d[0] * r[3] + d[2] * r[1] - d[1] * r[2] - d[3] * r[0];
	translate_out[1] = d[1] * r[3] + d[0] * r[2] - d[2] * r[0] - d[3] * r[1];
	translate_out[2] = d[2] * r[3] + d[1] * r[0] - d[0] * r[1] - d[3] * r[2];

	unitAxis_out[0] = r[0] * s;
	unitAxis_out[1] = r[1] * s;
	unitAxis_out[2] = r[2] * s;

	*degrees_out = ac + ac;
	*distance_out = distance = a3real3Dot(translate_out, unitAxis_out);

	// moment = 1/2 * ( cross(translate, axis) + (translate - distance*axis)*cot(halfAngle) )
	moment_out[0] = a3real_half*(translate_out[1] * unitAxis_out[2] - translate_out[2] * unitAxis_out[1] + cot*(translate_out[0] - distance*unitAxis_out[0]));
	moment_out[1] = a3real_half*(translate_out[2] * unitAxis_out[0] - translate_out[0] * unitAxis_out[2] + cot*(translate_out[1] - distance*unitAxis_out[1]));
	moment_out[2] = a3real_half*(translate_out[0] * unitAxis_out[1] - translate_out[1] * unitAxis_out[0] + cot*(translate_out[2] - distance*unitAxis_out[2]));
	return Q;
}


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_DUALQUATERNION_IMPL_INL
#endif	// __ANIMAL3D_A3DM_DUALQUATERNION_H