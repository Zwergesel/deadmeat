#include "../creature.hpp"
#include "../world.hpp"

FailWhale::FailWhale() : Creature(Point(60, 35), "fail whale", 'w', TCODColor::blue) {};

int FailWhale::action(Level* level, Player* player)
{
	Point ppos = player->getPos();
	int dx = util::sign(ppos.x - position.x);
	int dy = util::sign(ppos.y - position.y);
	Point target(position.x+dx, position.y+dy);

	if (target.x == ppos.x && target.y == ppos.y)
	{
		world.addMessage("The " + name + " misses!");
		return 5;
	}
	else if (world.tileSet->isPassable(level->getTile(target)) && level->creatureAt(target) == NULL)
	{
		position.x += dx;
		position.y += dy;
		return 21;
	}
	else
	{
		return 8;
	}
}