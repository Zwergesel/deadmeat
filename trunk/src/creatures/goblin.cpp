#include "../creature.hpp"
#include "../world.hpp"

Goblin::Goblin() : Creature(20, 20, "goblin", 'g', TCODColor::green) {};

int Goblin::action(Level* level, Player* player)
{
	int dx = util::sign(player->getX() - x);
	int dy = util::sign(player->getY() - y);

	if (x+dx == player->getX() && y+dy == player->getY())
	{
		world.addMessage("The " + name + " hits!");
		return 17;
	}
	else if (world.tileSet->isPassable(level->getTile(x+dx, y+dy)))
	{
		x += dx;
		y += dy;
		return 9;
	}
	else
	{
		return 9;
	}
}