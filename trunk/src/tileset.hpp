#ifndef _TILESET_HPP
#define _TILESET_HPP

#include <libtcod.hpp>
#include <sstream>
#include "utility.hpp"

class Savegame;

enum Tile
{
  TILE_CAVE_FLOOR,
  TILE_CAVE_WALL,
  TILE_STONE_FLOOR,
  TILE_MEADOW,
  TILE_DOOR,
  TILE_GRASS,
  TILE_TREE1,
  TILE_TREE2,
  TILE_DARK_GRASS,
  TILE_WATER1,
  TILE_WATER2,
  TILE_BRIDGE,
  TILES_LENGTH
};

enum TerrainType
{
	TT_GROUND,	// walkable, transparent, !blocking
	TT_CURTAIN, // walkable, !transparent, !blocking
	TT_WALL,	// !walkable, !transparent, blocking
	TT_WINDOW,	// !walkable, transparent, blocking
	TT_WATER,	// !walkable, transparent, !blocking
	TT_PIT,		// !walkable, transparent, !blocking
	NUM_TERRAINTYPE
};

struct TileInfo
{
	TerrainType type;
	symbol sym;
	TCODColor color, background;
	std::string description;

	TileInfo():
		type(TT_WALL), sym('?'), color(TCODColor::black), background(TCODColor::white) {};
	TileInfo(TerrainType p, int s, TCODColor c, TCODColor b, std::string d):
		type(p), sym(s), color(c), background(b), description(d) {};
};

class TileSet
{
private:
	TileInfo* info;

public:
	TileSet();
	~TileSet();
	void setInfo(Tile t, TileInfo inf);
	TileInfo getInfo(Tile t);
	bool isWalkable(Tile t);
	bool isTransparent(Tile t);
	bool isBlocking(Tile t);
	bool isWater(Tile t);
	bool isPit(Tile t);
	const std::string& getDescription(Tile t);
};

#endif