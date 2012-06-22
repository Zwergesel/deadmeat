#include "pathfinding.hpp"
#include "level.hpp"
#include "world.hpp"
#include "utility.hpp"
#include "creature.hpp"

float PathFindingCallback::getWalkCost(int xFrom, int yFrom, int xTo, int yTo, void* userData) const
{
	Level* level = static_cast<Level*>(userData);
	Point target = Point(xTo, yTo);
	if (!level->isWalkable(target)) return 0.0;
	Creature* c = level->creatureAt(target);
	if (c != NULL && c->isControlled() == false) return 0.0;
	return 1.0;
}