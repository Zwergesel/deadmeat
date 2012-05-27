#include "creature.hpp"

Creature::Creature()
{
	name = "Unknown Creature";
	symbol = 'X';
}

Creature::~Creature()
{
	// nothing, override
}

Creature::action(Level level, Player player)
{
	// nothing, override
}