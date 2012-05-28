#include "../creature.hpp"
#include "../world.hpp"

FailWhale::FailWhale() : Creature(Point(60, 35), "fail whale", 'w', TCODColor::blue) {};

int FailWhale::action(Level* level, Player* player)
{
	int dx = util::sign(player->getX() - position.x);
	int dy = util::sign(player->getY() - position.y);

	if (position.x+dx == player->getX() && position.y+dy == player->getY())
	{
		world.addMessage("The " + name + " misses!");
		return 5;
	}
	else if (world.tileSet->isPassable(level->getTile(Point(position.x+dx, position.y+dy))))
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