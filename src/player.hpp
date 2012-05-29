#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <string>
#include <libtcod.hpp>
#include "utility.hpp"

class Player
{
private:
	Point position;
	std::string name;
	int symbol;
	TCODColor color;
	int actionTime;

public:
	static int dx[9];
	static int dy[9];

	Player(std::string name);
	std::string getName();
	Point getPos();
	int getSymbol();
	TCODColor getColor();
	int getActionTime();
	void move(Point dpos);
	void moveTo(Point p);
	void addActionTime(int dt);
};

#endif