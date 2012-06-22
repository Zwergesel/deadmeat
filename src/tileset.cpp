#include "tileset.hpp"

TileSet::TileSet()
{
	info = new TileInfo[TILES_LENGTH];

	// TODO: Hard-coded for now, load from file later
	// Parameters: walkable, transparent, symbol, foregroundcolor, backgroundcolor
	info[TILE_CAVE_FLOOR]   = TileInfo(TT_GROUND, '.', TCODColor::grey, TCODColor::white, "a stone tunnel");
	info[TILE_CAVE_WALL]    = TileInfo(TT_WALL, TCOD_CHAR_BLOCK1, TCODColor::grey, TCODColor(125,95,64), "a stone wall");
	info[TILE_STONE_FLOOR]  = TileInfo(TT_GROUND, '.', TCODColor::grey, TCODColor::white, "a stone tunnel");
	info[TILE_MEADOW]       = TileInfo(TT_GROUND, '.', TCODColor::green, TCODColor::white, "a meadow");
	info[TILE_GRASS]        = TileInfo(TT_GROUND, '.', TCODColor::lightGreen, TCODColor::lerp(TCODColor::lightGreen, TCODColor::white, 0.7f), "grass");
	info[TILE_DARK_GRASS]   = TileInfo(TT_GROUND, '.', TCODColor::darkGreen, TCODColor(123, 160, 126), "forest ground");
	info[TILE_TREE1]        = TileInfo(TT_WALL, TCOD_CHAR_CLUB, TCODColor(31, 97, 41), TCODColor(123, 160, 126), "a tree");
	info[TILE_TREE2]        = TileInfo(TT_WALL, TCOD_CHAR_SPADE, TCODColor(48, 79, 53), TCODColor(123, 160, 126), "a tree");
	info[TILE_WATER1]       = TileInfo(TT_WATER, 247, TCODColor(75, 170, 230), TCODColor(70, 150, 210), "a river");
	info[TILE_WATER2]       = TileInfo(TT_WATER, 247, TCODColor(60, 140, 230), TCODColor(50, 120, 195), "a river");
	info[TILE_BRIDGE]       = TileInfo(TT_GROUND, TCOD_CHAR_BLOCK3, TCODColor(125,95,64), TCODColor(90, 60, 30), "a bridge");
}

TileSet::~TileSet()
{
	if (info != NULL)
	{
		delete[] info;
		info = NULL;
	}
}

void TileSet::setInfo(Tile t, TileInfo inf)
{
	info[t] = inf;
}

TileInfo TileSet::getInfo(Tile t)
{
	return info[t];
}

bool TileSet::isWalkable(Tile t)
{
	return (info[t].type == TT_GROUND || info[t].type == TT_CURTAIN);
}

bool TileSet::isTransparent(Tile t)
{
	return (info[t].type != TT_WALL && info[t].type != TT_CURTAIN);
}

bool TileSet::isBlocking(Tile t)
{
	return (info[t].type == TT_WALL || info[t].type == TT_WINDOW);
}

bool TileSet::isWater(Tile t)
{
	return (info[t].type == TT_WATER);
}

bool TileSet::isPit(Tile t)
{
	return (info[t].type == TT_PIT);
}

const std::string& TileSet::getDescription(Tile t)
{
	return info[t].description;
}