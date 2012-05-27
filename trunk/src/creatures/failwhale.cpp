#include "../creature.hpp";

FailWhale::FailWhale() : Creature(60, 35, "fail whale", 'w', TCODColor::blue) {};

// TODO: temporarily defined sign here, put it somewhere else
// TODO: this is extremely dumb!!!!
int sign2(int x)
{
	return (x > 0) - (x < 0);
}

int FailWhale::action(Level* level, Player player, std::deque<std::string> *messages)
{
	int dx = sign2(player.x - x);
	int dy = sign2(player.y - y);

	if (x+dx == player.x && y+dy == player.y)
	{
		messages->push_back("The " + name + " misses!");
		return 5;
	}
	else if (globalTileSet.info[level->getTile(x+dx, y+dy)].passable)
	{
		x += dx;
		y += dy;
		return 24;
	}
	else
	{
		return 8;
	}
}