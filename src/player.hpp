#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <string>
#include <libtcod.hpp>

class Player
{
private:
	int x, y;
	std::string name;
	int symbol;
	TCODColor color;
	int actionTime;

public:
	static int dx[9];
	static int dy[9];

	Player(std::string name);
	std::string getName();
	int getX();
	int getY();
	int getSymbol();
	TCODColor getColor();
	int getActionTime();
	void move(int dx, int dy);
	void moveTo(int x, int y);
	void addActionTime(int dt);
};

#endif