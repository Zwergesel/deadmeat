#include "utility.hpp"

namespace util
{

int sign(int x)
{
	return (x > 0) - (x < 0);
}

}

bool operator==(Point a, Point b)
{
	return (a.x == b.x && a.y == b.y);
}

Point operator+=(Point a, Point b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}