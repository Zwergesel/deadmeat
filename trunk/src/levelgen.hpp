#ifndef _LEVELGEN_HPP
#define _LEVELGEN_HPP

#include "utility.hpp"
#include "object.hpp"
#include <vector>

class Level;

enum LEVELTYPE
{
  LEVELTYPE_CAVE,
  LEVELTYPE_ROOM,
  LEVELTYPE_BSP,
  LEVELTYPE_PLAIN,
  LEVELTYPE_FOREST,
  LEVELTYPE_SMAUGS_LAIR, // TODO: this is a temporary type for testing =)
  NUM_LEVELTYPE
};

struct WorldLink
{
	OBJECTTYPE type;
	Point pos;
	int to;
	int entranceId;
	int exitId;
};

struct WorldNode
{
	LEVELTYPE type;
	std::vector<WorldLink> link;
};

class LevelGen
{
public:
	static void loadLootTable();
	static void generateWorld();
	// level generation functions, returns NULL on invalid parameters
	static Level* generateLevel(int levelId, LEVELTYPE type);
	static Level* generateCaveLevel(int levelId, int width, int height, float density = 40.f);
	static Level* generateRoomLevel(int levelId, int width, int height, float roomDensity = 30.f);
	static Level* generateBSPLevel(int levelId, int width, int height, int numSplits = 6, float squareness = 1.5f);
	static Level* generatePlainLevel(int levelId, int width, int height);
	static Level* generateForestLevel(int levelId, int width, int height);
private:
	static bool placeEntrances(int levelId, Level* l);
};

#endif