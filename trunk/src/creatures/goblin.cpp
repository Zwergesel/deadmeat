#include "../creature.hpp";

Goblin::Goblin() : Creature(20, 20, "goblin", 'g', TCODColor::green) {};

// TODO: temporarily defined sign here, put it somewhere else
int sign(int x)
{
	return (x > 0) - (x < 0);
}

int Goblin::action(Level* level, Player player, std::deque<std::string> *messages)
{
	int dx = sign(player.x - x);
	int dy = sign(player.y - y);

	if (x+dx == player.x && y+dy == player.y)
	{
		messages->push_back("The " + name + " hits!");
		return 17;
	}
	else if (globalTileSet.info[level->getTile(x+dx, y+dy)].passable)
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