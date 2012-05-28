#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <algorithm>

struct Point
{
	int x, y;
	Point():x(0),y(0){};
	Point(int x, int y):x(x),y(y){};
};

namespace util
{

	int sign(int x);

	template <class T> T clamp(T val, T min, T max)
	{
		return std::max(min, std::min(val, max));
	}

};

#endif