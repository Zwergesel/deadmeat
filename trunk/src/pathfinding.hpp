#ifndef _PATHFINDING_HPP
#define _PATHFINDING_HPP

#include <libtcod.hpp>

class PathFindingCallback: public ITCODPathCallback
{
public:
	float getWalkCost(int xFrom, int yFrom, int xTo, int yTo, void* userData) const;
};
#endif