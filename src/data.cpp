#include "factory.hpp"
#include "creature.hpp"
#include "item.hpp"
#include "items/weapon.hpp"
#include "items/armor.hpp"
#include "items/food.hpp"
#include "items/ammo.hpp"
#include "items/gold.hpp"
#include "items/tool.hpp"
#include "items/potion.hpp"
#include "items/spellbook.hpp"
#include "items/corpse.hpp"
#include "world.hpp"

void fillFactoryTemplates()
{
	// Add/edit/remove creature templates here
	/*
	BasicMonster troll("troll", F_MALE, 'T', TCODColor::darkBlue, 80, 0, Weapon(18, 20, 14, 1, 5, EFFECT_NONE, 1, AMMO_NONE), 40, 15, 750, "", false, false, 0);
	BasicMonster spider("spider", F_FEMALE, 's', TCODColor::pink, 15, 0, Weapon(14, 0, 3, 2, 3, EFFECT_NONE, 1, AMMO_NONE), 60, 8, 150, "", false, false, 0);
	BasicMonster soldier("soldier", F_MALE, '@', TCODColor::blue, 120, 0, Weapon(16, 0, 3, 1, 2, EFFECT_NONE, 1, AMMO_NONE), 0, 14, 600, "", true, true, 0.25);
	BasicMonster sergeant("sergeant", F_MALE, '@', TCODColor::red, 180, 0, Weapon(13, 0, 3, 1, 2, EFFECT_NONE, 1, AMMO_NONE), 40, 14, 1000, "", true, true, 0.25);
	BasicMonster medic("medic", F_FEMALE, '@', TCODColor::pink, 180, 0, Weapon(25, 0, 0, 0, 0, EFFECT_NONE, 1, AMMO_NONE), 0, 12, 10, "", true, true, 0.5);
	BasicMonster redbaby("red baby dragon", F_MALE, 'd', TCODColor::red, 55, 0, Weapon(16, 15, 5, 1, 10, EFFECT_NONE, 1, AMMO_NONE), 20, 13, 600, "", false, false, 0);
	BasicMonster purbaby("purple baby dragon", F_MALE, 'd', TCODColor::violet, 55, 0, Weapon(16, 15, 5, 1, 10, EFFECT_NONE, 1, AMMO_NONE), 20, 13, 600, "", false, false, 0);
	BasicMonster blubaby("blue baby dragon", F_MALE, 'd', TCODColor::blue, 55, 0, Weapon(16, 15, 5, 1, 10, EFFECT_NONE, 1, AMMO_NONE), 20, 13, 600, "", false, false, 0);
	BasicMonster dragon("Smaug", F_PROPER | F_MALE, 'D', TCODColor::red, 300, 100, Weapon(15, 50, 19, 3, 7, EFFECT_NONE, 1, AMMO_NONE), 75, 16, 2500, "", false, false, 0);
	*/
	
	BasicMonster deer("deer", F_MALE, 'd', TCODColor::desaturatedOrange, 25, 0, Weapon(20, 0, 2, 1, 3, EFFECT_NONE, 1, AMMO_NONE), 0, 13, 90, "deer corpse", false, false, 100.f);
	BasicMonster rat("rat", F_FEMALE, 'r', TCODColor::darkYellow, 13, 0, Weapon(14, 20, 3, 1, 5, EFFECT_DISEASE, 1, AMMO_NONE), 30, 10, 180, "rat corpse", false, false, 15.f);
	BasicMonster kobold("kobold", F_MALE, 'k', TCODColor::lightMagenta, 21, 0, Weapon(20, 0, 2, 1, 3, EFFECT_NONE, 1, AMMO_NONE), 0, 14, 200, "kobold corpse", true, true, 25.f);
	BasicMonster goblin("goblin", F_MALE, 'g', TCODColor::green, 30, 0, Weapon(21, 30, 6, 2, 5, EFFECT_NONE, 1, AMMO_NONE), 15, 20, 300, "goblin corpse", true, true, 0.f);
	BasicMonster wisp("wisp", F_NEUTER, 'W', TCODColor::lightYellow, 25, 100, Weapon(42, 50, 2, 3, 4, EFFECT_NONE, 6, AMMO_NONE), 25, 13, 330, "", false, false, 0.f);
	BasicMonster boar("boar", F_MALE, 'B', TCODColor::orange, 45, 0, Weapon(32, 40, 12, 3, 7, EFFECT_NONE, 1, AMMO_NONE), 50, 32, 550, "boar corpse", false, false, 0.f);
	BasicMonster snake("snake", F_MALE, 's', TCODColor::darkChartreuse, 23, 0, Weapon(30, 22, 14, 1, 5, EFFECT_POISON, 1, AMMO_NONE), 0, 12, 420, "snake corpse", false, false, 0.f);
	BasicMonster wolf("wolf", F_MALE, 'W', TCODColor::darkerOrange, 40, 0, Weapon(20, 22, 9, 2, 7, EFFECT_NONE, 1, AMMO_NONE), 25, 18, 590, "wolf corpse", false, false, 20.f);
	BasicMonster carnitree("carnivorous tree", F_MALE, 6, TCODColor(48, 79, 53), 60, 0, Weapon(50, 0, 25, 4, 9, EFFECT_NONE, 1, AMMO_NONE), 0, 500, 520, "", false, false, 0.f);
	BasicMonster bear("bear", F_MALE, 6, TCODColor(100, 70, 40), 66, 0, Weapon(34, 0, 20, 2, 13, EFFECT_KNOCKBACK, 1, AMMO_NONE), 40, 37, 850, "bear corpse", false, false, 0.f);
	// name, flags, symbol, color, health, mana, weapon(spd, hit, dmg, dice, dmax, effect, range, ammo), armor, walkspeed, experience, corpse, meele wep, ranged wep, fleeperc (0-100)

	Corpse deer_c("deer corpse", F_NEUTER, '%', TCODColor::desaturatedOrange, 100, 2800, 100, 2000);
	Corpse rat_c("rat corpse", F_NEUTER, '%', TCODColor::darkYellow, 10, 500, 20, 3200);
	Corpse kobold_c("kobold corpse", F_NEUTER, '%', TCODColor::lightMagenta, 40, 800, 30, 3200);
	Corpse goblin_c("goblin corpse", F_NEUTER, '%', TCODColor::green, 50, 900, 30, 2000);
	Corpse boar_c("boar corpse", F_NEUTER, '%', TCODColor::orange, 175, 2200, 100, 2000);
	Corpse snake_c("snake corpse", F_NEUTER, '%', TCODColor::darkChartreuse, 10, 400, 20, 2000);
	Corpse wolf_c("wolf corpse", F_NEUTER, '%', TCODColor::darkerOrange, 100, 1400, 80, 2000);
	Corpse bear_c("bear corpse", F_NEUTER, '%', TCODColor(100, 70, 40), 250, 2400, 110, 2000);

	InventoryTable inv_kobold;
	inv_kobold.add("slingshot", 1000);
	inv_kobold.add("bullets", 1000);

	factory.setTemplate("deer", &deer);
	factory.setTemplate("rat", &rat);
	factory.setTemplate("kobold", &kobold, inv_kobold);
	factory.setTemplate("goblin", &goblin);
	factory.setTemplate("wisp", &wisp);
	factory.setTemplate("boar", &boar);
	factory.setTemplate("snake", &snake);
	factory.setTemplate("wolf", &wolf);
	factory.setTemplate("carnitree", &carnitree);
	factory.setTemplate("bear", &bear);

	factory.setTemplate("deer corpse", &deer_c);
	factory.setTemplate("rat corpse", &rat_c);
	factory.setTemplate("kobold corpse", &kobold_c);
	factory.setTemplate("goblin corpse", &goblin_c);
	factory.setTemplate("boar corpse", &boar_c);
	factory.setTemplate("snake corpse", &snake_c);
	factory.setTemplate("wolf corpse", &wolf_c);
	factory.setTemplate("bear corpse", &bear_c);

	Weapon lightsaber("lightsaber", F_NEUTER, '(', TCODColor::red, 1, 1, 8, 30, 1, 10, 20, 30, 1, EFFECT_NONE, 1, AMMO_NONE, -1);
	Weapon sword("long sword", F_NEUTER, '(', TCODColor::red, 1, 10, 15, 20, 12, 2, 8, 0, 1, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon dagger("dagger", F_NEUTER, '(', TCODColor::red, 1, 2, 7, 10, 8, 1, 4, 0, 1, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon halberd("halberd", F_NEUTER, '(', TCODColor::red, 1, 20, 22, 40, 15, 2, 10, 0, 2, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon bow("longbow", F_NEUTER, '(', TCODColor::cyan, 1, 10, 25, 25, 30, 2, 10, 0, 1, EFFECT_NONE, 30, AMMO_ARROW, 0);
	Weapon crossbow("crossbow", F_NEUTER, '(', TCODColor::blue, 1, 15, 35, 25, 40, 3, 10, 0, 1, EFFECT_NONE, 20, AMMO_BOLT, 0);
	Weapon slingshot("slingshot", F_NEUTER, '(', TCODColor::lightBlue, 1, 1, 15, 35, 10, 1, 10, 0, 1, EFFECT_NONE, 10, AMMO_BULLET, 0);
	Armor body("uber armor", F_NEUTER | F_AN, ')', TCODColor::black, 1, 30, 100, 2, 10, ARMOR_BODY);
	Armor helmet("prussian pickelhaube", F_NEUTER, ')', TCODColor::black, 1, 5, 50, 1, 0, ARMOR_HAT);
	Armor boots("clown shoes", F_NEUTER | F_BOOTS, ')', TCODColor::black, 1, 5, -30, -1, 12, ARMOR_BOOTS);
	Food food1("beefsteak", F_NEUTER, '%', TCODColor::darkOrange, 1, 3, 2500, 100);
	Food food2("meat ball", F_NEUTER, '%', TCODColor::orange, 1, 1, 1000, 15);
	Ammo arrow("arrow", F_NEUTER, '/', TCODColor::cyan, 5, 1, 0, EFFECT_NONE, AMMO_ARROW);
	Ammo bolt("bolt", F_NEUTER, '/', TCODColor::blue, 5, 1, 0, EFFECT_NONE, AMMO_BOLT);
	Ammo bullet("bullet", F_NEUTER, '/', TCODColor::lightBlue, 5, 1, 0, EFFECT_NONE, AMMO_BULLET);
	Tool lockpick("lockpick", F_NEUTER, '[', TCODColor::red, 1, 1, TOOL_LOCKPICK);
	Potion minorHeal("minor heal potion", F_NEUTER, '!', TCODColor::lightRed, 1, 3, POTION_MINORHEAL);
	Potion heal("heal potion", F_NEUTER, '!', TCODColor::red, 1, 3, POTION_HEAL);
	Potion fullHeal("full heal potion", F_NEUTER, '!', TCODColor::darkRed, 1, 3, POTION_FULLHEAL);
	Potion haste("haste potion", F_NEUTER, '!', TCODColor::blue, 1, 3, POTION_HASTE);
	SpellBook v_cripple("voodoo book named 'Cripple'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::purple, 0.4f), 6, SPELL_V_CRIPPLE);
	SpellBook v_frenzy("voodoo book named 'Frenzy'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::purple, 0.5f), 6, SPELL_V_FRENZY);
	SpellBook v_innerdemons("voodoo book named 'Inner Demons'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::purple, 0.6f), 6, SPELL_V_INNER_DEMONS);
	SpellBook v_feast("voodoo book named 'Feast'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::purple, 0.7f), 6, SPELL_V_FEAST);
	SpellBook e_fire("elemental book named 'Fireball'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::azure, 0.4f), 6, SPELL_E_FIRE);
	Gold gold(1);

	factory.setTemplate("lightsaber", &lightsaber);
	factory.setTemplate("sword", &sword);
	factory.setTemplate("dagger", &dagger);
	factory.setTemplate("halberd", &halberd);
	factory.setTemplate("longbow", &bow);
	factory.setTemplate("crossbow", &crossbow);
	factory.setTemplate("slingshot", &slingshot);
	factory.setTemplate("uber armor", &body);
	factory.setTemplate("pickelhaube", &helmet);
	factory.setTemplate("clown shoes", &boots);
	factory.setTemplate("beefsteak", &food1);
	factory.setTemplate("meatball", &food2);
	factory.setTemplate("arrows", &arrow);
	factory.setTemplate("bolts", &bolt);
	factory.setTemplate("bullets", &bullet);
	factory.setTemplate("gold", &gold);
	factory.setTemplate("lockpick", &lockpick);
	factory.setTemplate("pMinorHeal", &minorHeal);
	factory.setTemplate("pHeal", &heal);
	factory.setTemplate("pFullHeal", &fullHeal);
	factory.setTemplate("pHaste", &haste);
	factory.setTemplate("sbCripple", &v_cripple);
	factory.setTemplate("sbFrenzy", &v_frenzy);
	factory.setTemplate("sbInnerDemons", &v_innerdemons);
	factory.setTemplate("sbFeast", &v_feast);
	factory.setTemplate("sbFire", &e_fire);
}