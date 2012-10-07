#include "message.hpp"
#include "world.hpp"
#include <sstream>

void Message::attack(Creature* attacker, Creature* defender, int damage, DamageType dtype)
{
	if (!world.player->isPositionVisible(defender->getPos())) return;
	std::stringstream msg;
	if (attacker->isControlled())
		msg << "You hit ";
	else
		msg << util::format(FORMAT_DEF, attacker, true) << " hits ";
	if (defender->isControlled())
		msg << "you for ";
	else
		msg << util::format(FORMAT_DEF, defender) << " for ";
	msg << damage << " damage.";
	world.addMessage(msg.str());	
}

void Message::kill(Creature* killer, Creature* victim)
{
	// Player death is not handled here, so victim is never the player
	if (!world.player->isPositionVisible(victim->getPos())) return;
	if (killer != NULL)
	{
		std::stringstream msg;
		if (killer->isControlled())
			msg << "You kill ";
		else
			msg << util::format(FORMAT_DEF, killer, true) << " kills";
		msg << util::format(FORMAT_DEF, victim) << ".";
		world.addMessage(msg.str());
	}
	else
	{
		std::stringstream msg;
		msg << util::format(FORMAT_DEF, victim, true) << " dies.";
		world.addMessage(msg.str());
	}

}
