#ifndef _LEVELGEN_HPP
#define _LEVELGEN_HPP

#include "level.hpp"

class LevelGen
{
public:
	// level generation functions, returns NULL on invalid parameters
	Level* generateCaveLevel(int width, int height, float density = 40.f);
};

#endif