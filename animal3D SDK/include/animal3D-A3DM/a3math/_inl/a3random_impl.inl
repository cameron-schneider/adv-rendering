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

	a3random_impl.inl
	Implementations for random number generator functions.
*/

#ifdef __ANIMAL3D_A3DM_RANDOM_H
#ifndef __ANIMAL3D_A3DM_RANDOM_IMPL_INL
#define __ANIMAL3D_A3DM_RANDOM_IMPL_INL


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------

// values for random number generation
A3_GLOBAL a3integer a3_randseed = 1235;

#define A3_RANDMAX		134456
#define A3_RANDMAXINV	a3recip(A3_RANDMAX)


//-----------------------------------------------------------------------------

// random getters and setters: get max and seed
// set seed returns old seed
A3_INLINE a3integer a3randomGetMax()
{
	return A3_RANDMAX;
}

A3_INLINE a3integer a3randomGetSeed()
{
	return a3_randseed;
}

A3_INLINE a3integer a3randomSetSeed(const a3integer seed)
{
	const a3integer seed_old = a3_randseed;
	a3_randseed = seed;
	return seed_old;
}


// quick n' dirty random number generator that's been around since the 70s
// need to set the seed for actual randomness
A3_INLINE a3integer a3randomInt()
{
	const a3integer randparam_a = 141, randparam_c = 28411;
	a3_randseed = (a3_randseed*randparam_a + randparam_c) % A3_RANDMAX;
	return a3_randseed;
}

// random: pick a random decimal number
A3_INLINE a3real a3random()
{
	return (a3real)a3randomInt();
}

// normalized random: pick a random number in [0, 1)
A3_INLINE a3real a3randomNormalized()
{
	const a3real n = a3random()*A3_RANDMAXINV;
	return n;
}

// symmetric random: pick a random number in [-1, 1)
A3_INLINE a3real a3randomSymmetric()
{
	const a3real n = a3randomNormalized();
	return a3deserialize(n);
}


// max random: pick a random number in [0, nMax)
A3_INLINE a3real a3randomMax(const a3real nMax)
{
	const a3real n = a3randomNormalized()*nMax;
	return n;
}

A3_INLINE a3integer a3randomMaxInt(const a3integer nMax)
{
	const a3integer n = a3randomInt() % nMax;
	return n;
}


// range random: pick a random number in [nMin, nMax)
A3_INLINE a3real a3randomRange(const a3real nMin, const a3real nMax)
{
	const a3real n = a3randomNormalized()*(nMax - nMin) + nMin;
	return n;
}

A3_INLINE a3integer a3randomRangeInt(const a3integer nMin, const a3integer nMax)
{
	const a3integer n = a3randomInt() % (nMax - nMin) + nMin;
	return n;
}


//-----------------------------------------------------------------------------

#undef A3_RANDMAX


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_RANDOM_IMPL_INL
#endif	// __ANIMAL3D_A3DM_RANDOM_H