#include "player.hpp"

Player::Player(std::string name)
	: name(name), symbol('@'), color(TCODColor::black), position(Point(0,0)), actionTime(0)
{
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