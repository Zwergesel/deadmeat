#include <libtcod.hpp>
#include <string>

#ifndef _CREATURE_HPP
#define _CREATURE_HPP

class Creature
{

	private:
	std::string name;
	int symbol;
	
	public:
	Creature();
	~Creature();
	action(Level, Player);

}

#endif