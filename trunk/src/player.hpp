#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <string>

class Player
{
public:
  Player(std::string name);
	std::string name;
	char symbol;
  int x,y;
  static int dx[9];
  static int dy[9];
};

#endif