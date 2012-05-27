#include "creature.hpp"

Creature::Creature(int x, int y, std::string name, int symbol, TCODColor color)
	:x(x), y(y), name(name), symbol(symbol), color(color)
{
}

Creature::~Creature()
{
	// nothing, override
}

void Creature::draw(int offsetX, int offsetY)
{
	if (x - offsetX >= 0 && x - offsetX < TCODConsole::root->getWidth()
	    && y - offsetY >= 0 && y - offsetY < TCODConsole::root->getHeight())
	{
		TCODConsole::root->putChar(x - offsetX, y - offsetY, symbol);
		TCODConsole::root->setCharForeground(x - offsetX, y - offsetY, color);
	}
}