#include "utility.hpp"

namespace util
{

	int sign(int x)
	{
		return (x > 0) - (x < 0);
	}

}

bool Point::operator==(const Point p) const
{
	return (this->x == p.x && this->y == p.y);
}

bool Point::operator!=(const Point p) const
{
	return !(*this == p);
}

Point& Point::operator+=(const Point& p)
{
	this->x += p.x;
	this->y += p.y;
	return *this;
}

Point& Point::operator-=(const Point& p)
{
	this->x -= p.x;
	this->y -= p.y;
	return *this;
}

const Point Point::operator+(const Point& p) const
{
	return (Point(*this) += p);
}

const Point Point::operator-(const Point& p) const
{
	return (Point(*this) -= p);
}

std::string util::indefArticle(std::string str)
{
	std::string ret="a";
	if (str.size() <= 0) return ret;
	char f = str[0];
	if (f == 'a' || f == 'A' || f == 'e' || f == 'E' || f == 'i' || f == 'I' ||
	    f == 'u' || f == 'U' || f == 'o' || f == 'O')
	{
		ret = "an";
	}
	return ret;
}

std::string util::plural(std::string str)
{
	std::string ret = str;
	if (str.size() >= 1 && str[str.size()-1] == 's') ret.append("es");
	else if (str.size() >= 2 && str[str.size()-2] == 's' && str[str.size()-1] == 'h') ret.append("es");
	else if (str.size() >= 1 && str[str.size()-1] == 'y') ret.replace(str.size()-1, 1, "ies");
	else ret.append("s");
	return ret;
}

int util::letterToInt(char c)
{
	if (isupper(c)) return (c + 26 - 'A');
	else if (islower(c)) return (c - 'a');
	return 0;
}