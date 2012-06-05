#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <algorithm>
#include <string>

typedef unsigned char symbol;
typedef unsigned int uint;

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
	std::string plural(std::string str);

	const static unsigned char letters[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	                                       'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
	                                       };
  const static int numLetters = 52;
	int letterToInt(char c);
};

#endif