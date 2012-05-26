#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

class Player
{

	private:
	std::string name;
	char symbol;
	
	public:
	Player(std::string);	// name
  int x,y;
  static int dx[9];
  static int dy[9];

};

#endif