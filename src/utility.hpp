#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <algorithm>

namespace util
{

	int sign(int x);

	template <class T> T clamp(T val, T min, T max)
	{
		return std::max(min, std::min(val, max));
	}

};

#endif