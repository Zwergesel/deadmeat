#include "ammo.hpp"
#include "../savegame.hpp"
#include <iostream>

Ammo::Ammo()
{
	type = ITEM_AMMO;
	strType = "ammunition";
}

Ammo::Ammo(std::string n, uint f, symbol s, TCODColor c, int x, int ench, WeaponEffect e, AmmoType a):
	Item(n,f,s,c,x),enchantment(ench),effect(e),ammoType(a)
{
	type = ITEM_AMMO;
	strType = "ammunition";
}

Ammo::~Ammo()
{
}

Item* Ammo::clone()
{
	Ammo* copy = new Ammo(name, formatFlags, sym, color, amount, enchantment, effect, ammoType);
	copy->active = active;
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

AmmoType Ammo::getAmmoType()
{
	return ammoType;
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
	store ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	store ("color", color) ("amount", amount) ("active", active);
	store ("enchantment", enchantment) ("effect", effect);
	store ("ammoType", ammoType);
	sg << store;
	return id;
}

void Ammo::load(LoadBlock& load)
{
	int e,a;
	load ("name", name) ("formatFlags", formatFlags) ("symbol", sym);
	load ("color", color) ("amount", amount) ("active", active);
	load ("enchantment", enchantment) ("effect", e);
	load ("ammoType", a);
	if (a < 0 || a >= NUM_AMMOTYPE) throw SavegameFormatException("Ammo::load _ ammoType out of range");
	ammoType = static_cast<AmmoType>(a);
	if (e < 0 || e >= NUM_EFFECT) throw SavegameFormatException("Ammo::load _ effect out of range");
	effect = static_cast<WeaponEffect>(e);
}