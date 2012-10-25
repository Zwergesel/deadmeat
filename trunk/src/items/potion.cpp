#include "potion.hpp"
#include "../savegame.hpp"
#include "../creature.hpp"

Potion::Potion()
{
	type = ITEM_POTION;
	strType = "potions";
}

Potion::~Potion() {}

Potion::Potion(Name n, symbol s, TCODColor c, int a, int w, PotionType t) :
	Item(n,s,c,a,w), potion(t)
{
	type = ITEM_POTION;
	strType = "potions";
}

Item* Potion::clone()
{
	Potion* copy = new Potion(name, sym, color, amount, weight, potion);
	copy->active = active;
	return copy;
}

void Potion::effect(Creature* c)
{
	// TODO: clean this up, fix numbers, static functions?, c->hurt used for heal?
	switch (potion)
	{
	case POTION_MINORHEAL:
		c->heal(15);
		break;
	case POTION_HEAL:
		c->heal(std::max(30, c->getHealth().second / 4));
		break;
	case POTION_FULLHEAL:
		c->heal(c->getHealth().second);
		break;
	case POTION_HASTE:
		c->affect(STATUS_HASTE, 0, 100, 15);
		break;
	default:
		break;
	}
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Potion::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Potion", id);
	store ("name", name) ("symbol", sym) ("color", color);
	store ("amount", amount) ("weight", weight) ("active", active);
	store ("potion", potion);
	sg << store;
	return id;
}

void Potion::load(LoadBlock& load)
{
	int p;
	load ("name", name) ("symbol", sym) ("color", color);
	load ("amount", amount) ("weight", weight) ("active", active);
	load ("potion", p);
	if (p < 0 || p >= NUM_POTIONTYPE) throw SavegameFormatException("Tool::load _ potion out of bounds");
	potion = static_cast<PotionType>(p);
}