#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <string>
#include <libtcod.hpp>

class Player
{
public:
	static int dx[9];
	static int dy[9];

	int x,y;
	std::string name;
	int symbol;
	int nextAction;

	Player(std::string name);
	void draw(int offsetX, int offsetY);
};

#endif