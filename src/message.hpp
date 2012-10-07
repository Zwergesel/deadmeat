#ifndef _MESSAGE_HPP
#define _MESSAGE_HPP

#include "creature.hpp"

namespace Message
{
	void attack(Creature* attacker, Creature* defender, int damage, DamageType dtype);
	void kill(Creature* killer, Creature* victim);
}

#endif