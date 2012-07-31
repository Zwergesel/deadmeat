#ifndef _SPELLBOOK_HPP
#define _SPELLBOOK_CPP

#include "../item.hpp"
#include "../spell.hpp"

class Savegame;
class LoadBlock;


class SpellBook : public Item
{
private:
	SPELL spell;

public:
	SpellBook();
	SpellBook(std::string name, uint formatFlags, symbol sym, TCODColor clr, int weight, SPELL spell);
	~SpellBook();
	Item* clone();

	SPELL getSpell();

	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

#endif