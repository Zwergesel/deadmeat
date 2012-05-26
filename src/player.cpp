#include "player.hpp"

Player::Player(std::string name) {
	this->name = name;
	this->symbol = '@';
}

int Player::dx[] = {-1,0,1,-1,0,1,-1,0,1};
int Player::dy[] = {1,1,1,0,0,0,-1,-1,-1};