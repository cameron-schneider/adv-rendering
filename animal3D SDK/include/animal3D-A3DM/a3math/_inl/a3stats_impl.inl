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

	a3stats_impl.inl
	Implementations for miscellaneous statistical functions.
*/

#ifdef __ANIMAL3D_A3DM_STATS_H
#ifndef __ANIMAL3D_A3DM_STATS_IMPL_INL
#define __ANIMAL3D_A3DM_STATS_IMPL_INL


#include "../a3sqrt.h"


A3_BEGIN_IMPL


//-----------------------------------------------------------------------------
// median, mean, variance, standard deviation (with options to return mean)

A3_INLINE a3real a3median(const a3real data[], const a3count n)
{
	const a3index odd = n % 2;
	const a3index half = n / 2;
	if (odd)
		return data[half];
	return (a3real_half * (data[half] + data[half - 1]));
}

A3_INLINE a3real a3medianInt(const a3integer data[], const a3count n)
{
	const a3index odd = n % 2;
	const a3index half = n / 2;
	if (odd)
		return (a3real)data[half];
	return (a3real_half * (a3real)(data[half] + data[half - 1]));
}


A3_INLINE a3real a3mean(const a3real data[], const a3count n)
{
	a3real sum = a3real_zero;
	if (n > 0)
	{
		a3index i;
		for (i = 0; i < n; ++i)
			sum += data[i];
		sum /= (a3real)n;
	}
	return sum;
}

A3_INLINE a3real a3meanInt(const a3integer data[], const a3count n)
{
	a3integer sum = 0;
	a3real sumf;
	if (n > 0)
	{
		a3index i;
		for (i = 0; i < n; ++i)
			sum += data[i];
		sumf = (a3real)sum / (a3real)n;
	}
	return sumf;
}


A3_INLINE a3real a3variance(const a3real data[], const a3count n, a3real *mean_out)
{
	const a3real mean = a3mean(data, n);
	a3real v = a3real_zero, dv;
	a3index i;
	for (i = 0; i < n; ++i)
	{
		dv = data[i] - mean;
		v += dv*dv;
	}
	if (mean_out)
		*mean_out = mean;
	return v;
}

A3_INLINE a3real a3varianceInt(const a3integer data[], const a3count n, a3real *mean_out)
{
	const a3real mean = a3meanInt(data, n);
	a3real v = a3real_zero, dv;
	a3index i;
	for (i = 0; i < n; ++i)
	{
		dv = (a3real)data[i] - mean;
		v += dv*dv;
	}
	if (mean_out)
		*mean_out = mean;
	return v;
}


A3_INLINE a3real a3standardDeviation(const a3real data[], const a3count n, a3real *mean_out)
{
	const a3real v = a3variance(data, n, mean_out);
	return a3sqrt(v);
}

A3_INLINE a3real a3standardDeviationInt(const a3integer data[], const a3count n, a3real *mean_out)
{
	const a3real v = a3varianceInt(data, n, mean_out);
	return a3sqrt(v);
}


//-----------------------------------------------------------------------------
// factorial, permutations and combinations

A3_INLINE a3biginteger a3factorial(a3bigcount n)
{
	if (n > 1)
	{
		a3biginteger f = n;
		for (; n > 2; f *= --n);
		return f;
	}
	return 1;
}

A3_INLINE a3biginteger a3permutations(const a3bigcount n, const a3bigindex k)
{
	if (k <= n)
		return (a3biginteger)(a3factorial(n) / a3factorial(n - k));
	return 0;
}

A3_INLINE a3biginteger a3combinations(const a3bigcount n, const a3bigindex k)
{
	if (k <= n)
		return (a3biginteger)(a3factorial(n) / (a3factorial(k)*a3factorial(n - k)));
	return 0;
}


//-----------------------------------------------------------------------------


A3_END_IMPL


#endif	// !__ANIMAL3D_A3DM_STATS_IMPL_INL
#endif	// __ANIMAL3D_A3DM_STATS_H