#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <algorithm>
#include <string>

class Creature;
class Item;
class Object;

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
	static int sqlen(Point p);
};

struct Viewport
{
	int x,y,width,height;
	Viewport():x(0),y(0),width(1),height(1) {};
	Viewport(int x, int y, int w, int h):x(x),y(y),width(w),height(h) {};
};

enum FormatFlags
{
  F_DEFAULT = 0,
  F_PROPER = 1,
  F_DEFINITE = 2,
  F_PLURAL = 4,
  F_BOOTS = 8,
  F_AN = 16,

  F_MALE = 16384,
  F_FEMALE = 32768,
  F_NEUTER = 65536,
};

enum FormatRequest
{
  FORMAT_INDEF,
  FORMAT_DEF,
  FORMAT_YOUR,
  FORMAT_HIS
};

namespace util
{
	int sign(int x);

	template <class T> T clamp(T val, T min, T max)
	{
		return std::max(min, std::min(val, max));
	}

	std::string format(FormatRequest req, const std::string& name, uint flags, bool capitalize = false);
	std::string format(FormatRequest req, Creature* creature, bool capitalize = false);
	std::string format(FormatRequest req, Item* item, bool capitalize = false);
	std::string format(FormatRequest req, Object* obj, bool capitalize = false);
	std::string capitalize(std::string str);

	std::string plural(std::string str);

	const static unsigned char letters[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	                                       'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
	                                       };
	const static int numLetters = 52;
	int letterToInt(char c);
};

#endif