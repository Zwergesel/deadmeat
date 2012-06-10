#ifndef _LEVELGEN_HPP
#define _LEVELGEN_HPP

class Level;

class LevelGen
{
public:
	// level generation functions, returns NULL on invalid parameters
	Level* generateCaveLevel(int width, int height, float density = 40.f);
  Level* generateRoomLevel(int width, int height, float roomDensity = 30.f);
};

#endif