#include "utility.hpp"
#include <cctype>

int util::sign(int x)
{
	return (x > 0) - (x < 0);
}

std::string util::format(FormatRequest req, const std::string& name, uint flags, bool cap)
{
	std::string result;
	switch (req)
	{
	case FORMAT_INDEF:
		if (flags & F_PROPER)			result = name;
		else if (flags & F_DEFINITE)	result = "the " + name;
		else if (flags & F_BOOTS)		result = "a pair of " + name;
		else if (flags & F_PLURAL)		result = name;
		else if (flags & F_AN)			result = "an " + name;
		else							result = "a " + name;
		break;

	case FORMAT_DEF:
		if (flags & F_PROPER)			result = name;
		else							result = "the " + name;
		break;

	case FORMAT_YOUR:
		if (flags & F_PROPER)			result = name;
		else							result = "your " + name;
		break;

	case FORMAT_HIS:
		if (flags & F_MALE)				result = "his " + name;
		else if (flags & F_FEMALE)		result = "her " + name;
		else							result = "its " + name;
		break;
	}
	return cap ? util::capitalize(result) : result;
}

std::string& util::capitalize(std::string& str)
{
	str[0] = std::toupper(str[0]);
	return str;
}

std::string util::plural(std::string str)
{
	if (str.compare("armor") == 0 || str.compare("Armor") == 0) return str;
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