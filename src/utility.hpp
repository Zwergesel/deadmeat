#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <algorithm>
#include <string>

struct Point
{
	int x, y;
	Point():x(0),y(0) {};
	Point(int x, int y):x(x),y(y) {};
	bool operator==(const Point p) const;
	bool operator!=(const Point p) const;
	Point& operator+=(const Point& p);
	Point& operator-=(const Point& p);
	const Point operator+(const Point& p) const;
	const Point operator-(const Point& p) const;
};

namespace util
{

	int sign(int x);

	template <class T> T clamp(T val, T min, T max)
	{
		return std::max(min, std::min(val, max));
	}

	std::string indefArticle(std::string str);

};

#endif