#include "spellbook.hpp"
#include "../savegame.hpp"
#include "../world.hpp"

SpellBook::SpellBook()
{
	// constructor for savegames
	type = ITEM_SPELLBOOK;
	strType = "spellbooks";
}

SpellBook::SpellBook(Name n, symbol s, TCODColor c, int w, SPELL p):
	Item(n,s,c,1,w), spell(p)
{
	type = ITEM_SPELLBOOK;
	strType = "spellbooks";
}

SpellBook::~SpellBook() {}

Item* SpellBook::clone()
{
	SpellBook* copy = new SpellBook(name, sym, color, weight, spell);
	copy->amount = copy->amount;
	copy->active = active;
	return copy;
}

SPELL SpellBook::getSpell()
{
	return spell;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int SpellBook::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("SpellBook", id);
	store ("name", name) ("symbol", sym) ("color", color);
	store ("amount", amount) ("weight", weight) ("active", active);
	store ("spell", spell);
	sg << store;
	return id;
}

void SpellBook::load(LoadBlock& load)
{
	int s;
	load ("name", name) ("symbol", sym) ("color", color);
	load ("amount", amount) ("weight", weight) ("active", active);
	load ("spell", s);
	if (s < 0 || s >= NUM_SPELL) throw SavegameFormatException("SpellBook::load _ spell out of bounds");
	spell = static_cast<SPELL>(s);
}