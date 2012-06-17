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
	TT_WALL,
	TT_WATER,
	TT_GROUND,
	TT_PIT,
	NUM_TERRAINTYPE
};

struct TileInfo
{
	bool passable;
	symbol sym;
	TCODColor color, background;

	TileInfo():
		passable(false), sym('?'), color(TCODColor::black), background(TCODColor::white) {};
	TileInfo(bool p, int s, TCODColor c, TCODColor b):
		passable(p), sym(s), color(c), background(b) {};
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
	bool isPassable(Tile t);
};

#endif