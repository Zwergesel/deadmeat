#include "player.hpp"

Skill::Skill()
{
}

Skill::Skill(std::string name, int value, ATTRIBUTE att)
{
  this->name = name;
  this->value = value;
  this->att = att;
  this->exp = 0;
}

Player::Player(std::string name)
	: name(name), symbol('@'), color(TCODColor::black), position(Point(0,0)), actionTime(0)
{
  skills[SKILL_MELEE_COMBAT] = Skill("melee combat", 0, ATTR_STR);
  skills[SKILL_RANGED_COMBAT] = Skill("ranged combat", 0, ATTR_DEX);
  skills[SKILL_HEALTH] = Skill("health", 0, ATTR_CON);
  skills[SKILL_UNARMORED] = Skill("unarmored", 0, ATTR_DEX);
  skills[SKILL_LEATHER_ARMOR] = Skill("leather armor", 0, ATTR_DEX);
  skills[SKILL_SCALE_ARMOR] = Skill("scale armor", 0, ATTR_DEX);
  skills[SKILL_RING_ARMOR] = Skill("ring armor", 0, ATTR_DEX);
  skills[SKILL_CLOTH_ARMOR] = Skill("cloth armor", 0, ATTR_DEX);
  skills[SKILL_PLATE_ARMOR] = Skill("plate armor", 0, ATTR_DEX);
  skills[SKILL_AXE] = Skill("axe", 0, ATTR_STR);
  skills[SKILL_SWORD] = Skill("sword", 0, ATTR_STR);
  skills[SKILL_MACEFLAIL] = Skill("mace & flail", 0, ATTR_STR);
  skills[SKILL_STAFF] = Skill("staff", 0, ATTR_STR);
  skills[SKILL_DAGGER] = Skill("dagger", 0, ATTR_DEX);
  skills[SKILL_WHIP] = Skill("whip", 0, ATTR_DEX);
  skills[SKILL_PIKE] = Skill("pike", 0, ATTR_STR);
  skills[SKILL_BOW] = Skill("bow", 0, ATTR_DEX);
  skills[SKILL_CROSSBOW] = Skill("crossbow", 0, ATTR_DEX);
  skills[SKILL_SLING] = Skill("sling", 0, ATTR_DEX);
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