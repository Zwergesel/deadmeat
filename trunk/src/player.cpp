#include "player.hpp"

Player::Player(std::string name)
	: name(name), symbol('@'), color(TCODColor::black), x(0), y(0), actionTime(0)
{
}

int Player::getX()
{
	return x;
}

int Player::getY()
{
	return y;
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

void Player::move(int dx, int dy)
{
	x += dx;
	y += dy;
}

void Player::moveTo(int x, int y)
{
	this->x = x;
	this->y = y;
}

void Player::addActionTime(int dt)
{
	actionTime += dt;
}


int Player::dx[] = {-1,0,1,-1,0,1,-1,0,1};
int Player::dy[] = {1,1,1,0,0,0,-1,-1,-1};