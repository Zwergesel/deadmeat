#include "../creature.hpp"
#include "../world.hpp"
#include "../player.hpp"
#include "../tileset.hpp"
#include "../level.hpp"

Goblin::Goblin() : Creature(Point(20, 20), "goblin", 'g', TCODColor::green, 20) {};

int Goblin::action()
{
	Point ppos = world.player->getCreature()->getPos();
	int dx = util::sign(ppos.x - position.x);
	int dy = util::sign(ppos.y - position.y);
	Point target(position.x+dx, position.y+dy);

	if (target.x == ppos.x && target.y == ppos.y)
	{
		return attack(world.player->getCreature());
	}
	else if (world.tileSet->isPassable(level->getTile(target)) && level->creatureAt(target) == NULL)
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