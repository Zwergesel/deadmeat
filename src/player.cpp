#include "player.hpp"

Skill::Skill(std::string name, int value, ATTRIBUTE att)
{
  this->name = name;
  this->value = value;
  this->att = att;
}

Player::Player(std::string name)
	: name(name), symbol('@'), color(TCODColor::black), position(Point(0,0)), actionTime(0)
{
  skills.push_back(Skill("melee combat", 0, ATTR_STR));
  skills.push_back(Skill("ranged combat", 0, ATTR_DEX));
  skills.push_back(Skill("health", 0, ATTR_CON));
  skills.push_back(Skill("unarmored", 0, ATTR_DEX));
  skills.push_back(Skill("leather armor", 0, ATTR_DEX));
  skills.push_back(Skill("scale armor", 0, ATTR_DEX));
  skills.push_back(Skill("ring armor", 0, ATTR_DEX));
  skills.push_back(Skill("cloth armor", 0, ATTR_DEX));
  skills.push_back(Skill("plate armor", 0, ATTR_DEX));
  skills.push_back(Skill("axe", 0, ATTR_STR));
  skills.push_back(Skill("sword", 0, ATTR_STR));
  skills.push_back(Skill("mace & flail", 0, ATTR_STR));
  skills.push_back(Skill("staff", 0, ATTR_STR));
  skills.push_back(Skill("dagger", 0, ATTR_DEX));
  skills.push_back(Skill("whip", 0, ATTR_DEX));
  skills.push_back(Skill("pike", 0, ATTR_STR));
  skills.push_back(Skill("bow", 0, ATTR_DEX));
  skills.push_back(Skill("crossbow", 0, ATTR_DEX));
  skills.push_back(Skill("sling", 0, ATTR_DEX));
}

Point Player::getPos()
{
	return position;
}

int Player::getSymbol()
{
	return symbol;
}

TCODColor Player::getColor()
{
	return color;
}

int Player::getActionTime()
{
	return actionTime;
}

void Player::move(Point dpos)
{
	position.x += dpos.x;
	position.y += dpos.y;
}

void Player::moveTo(Point p)
{
	position = p;
}

void Player::addActionTime(int dt)
{
	actionTime += dt;
}


int Player::dx[] = {-1,0,1,-1,0,1,-1,0,1};
int Player::dy[] = {1,1,1,0,0,0,-1,-1,-1};