#include "tileset.hpp"

TileSet::TileSet()
{
	info = new TileInfo[TILES_LENGTH];

	// TODO: Hard-coded for now, load from file later
	// Parameters: passable (bool), symbol (int / char), foregroundcolor, backgroundcolor
	info[TILE_CAVE_FLOOR]   = TileInfo(true, '.', TCODColor::grey, TCODColor::white);
	info[TILE_CAVE_WALL]    = TileInfo(false, TCOD_CHAR_BLOCK1, TCODColor::grey, TCODColor(125,95,64));
	info[TILE_STONE_FLOOR]  = TileInfo(true, '.', TCODColor::grey, TCODColor::white);
	info[TILE_MEADOW]       = TileInfo(true, '.', TCODColor::green, TCODColor::white);
	info[TILE_DOOR]         = TileInfo(true, '+', TCODColor::yellow, TCODColor::white);
	info[TILE_GRASS]        = TileInfo(true, '.', TCODColor::lightGreen, TCODColor::lerp(TCODColor::lightGreen, TCODColor::white, 0.7f));
	info[TILE_DARK_GRASS]   = TileInfo(true, '.', TCODColor::darkGreen, TCODColor(123, 160, 126));
	info[TILE_TREE1]        = TileInfo(false, 5, TCODColor::desaturatedGreen, TCODColor(123, 160, 126));
	info[TILE_TREE2]        = TileInfo(false, 6, TCODColor::desaturatedGreen, TCODColor(123, 160, 126));
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

bool TileSet::isPassable(Tile t)
{
	return info[t].passable;
}