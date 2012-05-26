#include "level.hpp"
#include <libtcod.hpp>
#include <algorithm>
#include <iostream>

Level::Level(int width, int height) {
	this->width = width;
	this->height = height;
	this->map = new Tile[width*height];
	std::fill(map, map+width*height, Tile('.', true));
}

Level::~Level() {
	if (map != NULL) delete map;
}

inline int Level::coord(int x, int y) {
	return y*width + x;
}

void Level::display(int offsetX, int offsetY) {
	// TODO: 80 and 50 should be CONSOLE_WIDTH and CONSOLE_HEIGHT from somewhere
	for (int y=0; y<50; y++) {
	for (int x=0; x<80; x++) {
		std::cerr << coord(x+offsetX,y+offsetY) << std::endl;
		char s = map[coord(x+offsetX,y+offsetY)].symbol;
		TCODConsole::root->setChar(x,y,s);
	}
	}
}