#include "../creature.hpp"
#include "../world.hpp"

Goblin::Goblin() : Creature(Point(20, 20), "goblin", 'g', TCODColor::green) {};

int Goblin::action(Level* level, Player* player)
{
	int dx = util::sign(player->getX() - position.x);
	int dy = util::sign(player->getY() -position. y);

	if (position.x+dx == player->getX() && position.y+dy == player->getY())
	{
		world.addMessage("The " + name + " hits!");
		return 17;
	}
	else if (world.tileSet->isPassable(level->getTile(Point(position.x+dx, position.y+dy))))
	{
		position.x += dx;
		position.y += dy;
		return 9;
	}
	else
	{
		return 9;
	}
}