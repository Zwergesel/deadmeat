#ifndef _LEVELGEN_HPP
#define _LEVELGEN_HPP

#include <libtcod.hpp>
#include <algorithm>

class Level;

class LevelGen
{
public:
	// level generation functions, returns NULL on invalid parameters
	Level* generateCaveLevel(int width, int height, float density = 40.f);
};

#endif