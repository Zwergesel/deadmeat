#include "ammo.hpp"
#include "../savegame.hpp"
#include <iostream>

Ammo::Ammo()
{
	type = ITEM_AMMO;
	strType = "ammunition";
}

Ammo::Ammo(std::string n, uint f, symbol s, TCODColor c, int ench, WeaponEffect e):
	Item(n,f,s,c),enchantment(ench),effect(e)
{
	type = ITEM_AMMO;
	strType = "ammunition";
}

Ammo::~Ammo()
{
}

Item* Ammo::clone()
{
	Ammo* copy = new Ammo(name, formatFlags, sym, color, enchantment, effect);
	return copy;
}

int Ammo::getEnchantment()
{
	return enchantment;
}

WeaponEffect Ammo::getEffect()
{
	return effect;
}

std::string Ammo::toString()
{
	std::stringstream ss;
	ss << (enchantment < 0 ? "" : "+") << enchantment << " " << name;
	return ss.str();
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Ammo::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Ammo", id);
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color);
	store ("enchantment", enchantment) ("effect", effect);
	sg << store;
	return id;
}

void Ammo::load(LoadBlock& load)
{
	int e;
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym) ("color", color);
	load ("enchantment", enchantment) ("effect", e);
	if (e < 0 || e >= NUM_EFFECT) throw SavegameFormatException("Ammo::load _ effect out of range");
	effect = static_cast<WeaponEffect>(e);
}