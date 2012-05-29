#ifndef _PLAYER_HPP
#define _PLAYER_HPP

#include <string>
#include <vector>
#include <libtcod.hpp>
#include "utility.hpp"

enum ATTRIBUTE
{
  ATTR_STR,
  ATTR_DEX,
  ATTR_CON
};

struct Skill
{
  Skill(std::string name, int value, ATTRIBUTE att);
  std::string name;
  int value;
  int exp;
  ATTRIBUTE att;
};

class Player
{
private:
	Point position;
	std::string name;
	int symbol;
	TCODColor color;
	int actionTime;
  std::vector<Skill> skills;

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