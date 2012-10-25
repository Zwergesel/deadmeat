#include "ammo.hpp"
#include "../savegame.hpp"
#include "../world.hpp"
#include <iostream>

Ammo::Ammo()
{
	type = ITEM_AMMO;
	strType = "ammunition";
}

Ammo::Ammo(Name n, symbol s, TCODColor c, int x, int w, int ench, WeaponEffect e, AmmoType a):
	Item(n,s,c,x,w),enchantment(ench),effect(e),ammoType(a)
{
	type = ITEM_AMMO;
	strType = "ammunition";
}

Ammo::~Ammo()
{
}

Item* Ammo::clone()
{
	Ammo* copy = new Ammo(name, sym, color, amount, weight, enchantment, effect, ammoType);
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
	ss << (enchantment < 0 ? "" : "+") << enchantment << " " << name.name;
	return ss.str();
}

void Ammo::randomize(int level)
{
	amount = rng->getInt(1,30,5) + rng->getInt(0,30,5);
	int roll = rng->getInt(0,999);
	enchantment = (roll / 800) + (roll / 950) + (roll / 995);
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Ammo::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("Ammo", id);
	store ("name", name) ("symbol", sym) ("color", color);
	store ("amount", amount) ("weight", weight) ("active", active);
	store ("enchantment", enchantment) ("effect", effect);
	store ("ammoType", ammoType);
	sg << store;
	return id;
}

void Ammo::load(LoadBlock& load)
{
	int e,a;
	load ("name", name) ("symbol", sym) ("color", color);
	load ("amount", amount) ("weight", weight) ("active", active);
	load ("enchantment", enchantment) ("effect", e);
	load ("ammoType", a);
	if (a < 0 || a >= NUM_AMMOTYPE) throw SavegameFormatException("Ammo::load _ ammoType out of range");
	ammoType = static_cast<AmmoType>(a);
	if (e < 0 || e >= NUM_EFFECT) throw SavegameFormatException("Ammo::load _ effect out of range");
	effect = static_cast<WeaponEffect>(e);
}