#include "../creature.hpp"
#include "../world.hpp"

FailWhale::FailWhale() : Creature(60, 35, "fail whale", 'w', TCODColor::blue) {};

int FailWhale::action(Level* level, Player* player, std::deque<std::string> *messages)
{
	int dx = util::sign(player->getX() - x);
	int dy = util::sign(player->getY() - y);

	if (x+dx == player->getX() && y+dy == player->getY())
	{
		messages->push_back("The " + name + " misses!");
		return 5;
	}
	else if (world.tileSet->isPassable(level->getTile(x+dx, y+dy)))
	{
		x += dx;
		y += dy;
		return 21;
	}
	else
	{
		return 8;
	}
}