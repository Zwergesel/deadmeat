#include "player.hpp"

Player::Player(std::string name)
	: name(name), symbol('@'), x(0), y(0)
{}

void Player::draw(int offsetX, int offsetY)
{
	TCODConsole::root->putChar(x - offsetX, y - offsetY, symbol);
	TCODConsole::root->setCharForeground(x - offsetX, y - offsetY, TCODColor::black);
}

int Player::dx[] = {-1,0,1,-1,0,1,-1,0,1};
int Player::dy[] = {1,1,1,0,0,0,-1,-1,-1};