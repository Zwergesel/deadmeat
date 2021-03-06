#include "utility.hpp"
#include "creature.hpp"
#include "item.hpp"
#include "object.hpp"
#include <cctype>

std::string Name::getPlural()
{
	if (plural.size() > 0) return plural;
	std::string ret = name;
	if (ret[ret.size()-1] == 's') ret.append("es");
	else if (ret[ret.size()-1] == 'y') ret.replace(ret.size()-1, 1, "ies");
	else ret.append("s");
	return ret;
}

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

std::string util::format(FormatRequest req, Creature* c, bool cap)
{
	Name info = c->getName();
	return format(req, info.name, info.flags, cap);
}

std::string util::format(FormatRequest req, Item* i, bool cap)
{
	Name name = i->getName();
	if (i->getAmount() > 1 || (name.flags & F_PLURAL))
	{
		int flags = name.flags;
		flags |= F_PLURAL;
		std::stringstream ss;
		ss << i->getAmount() << " " << name.getPlural();
		if (i->isActive()) ss << " (" << Item::ACTIVE_STRINGS[i->getType()] << ")";
		return format(req, ss.str(), flags, cap);
	}
	else
	{
		std::stringstream ss;
		ss << i->toString();
		if (i->isActive()) ss << " (" << Item::ACTIVE_STRINGS[i->getType()] << ")";
		return format(req, ss.str(), name.flags, cap);
	}
}

std::string util::format(FormatRequest req, Object* obj, bool cap)
{
	return format(req, obj->toString(), obj->getFormatFlags(), cap);
}

std::string util::capitalize(std::string str)
{
	str[0] = std::toupper(str[0]);
	return str;
}

int util::letterToInt(char c)
{
	if (isupper(c)) return (c + 26 - 'A');
	else if (islower(c)) return (c - 'a');
	return -1;
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

int Point::sqlen(Point p)
{
	return p.x*p.x + p.y*p.y;
}