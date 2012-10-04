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
	deer.changeAttitude(ATTITUDE_PEACEFUL);
	BasicMonster rat("rat", F_FEMALE, 'r', TCODColor::darkYellow, 13, 0, Weapon(14, 0, 4, 1, 3, EFFECT_DISEASE, 1, AMMO_NONE), 30, 10, 180, "rat corpse", false, false, 15.f);
	BasicMonster kobold("kobold", F_MALE, 'k', TCODColor::lightMagenta, 21, 0, Weapon(20, 0, 3, 1, 2, EFFECT_NONE, 1, AMMO_NONE), 0, 14, 200, "kobold corpse", true, true, 25.f);
	BasicMonster goblin("goblin", F_MALE, 'g', TCODColor::green, 30, 0, Weapon(21, 0, 7, 1, 3, EFFECT_NONE, 1, AMMO_NONE), 15, 20, 300, "goblin corpse", true, true, 0.f);
	BasicMonster wisp("wisp", F_NEUTER, 'W', TCODColor::lightYellow, 25, 100, Weapon(42, 0, 5, 2, 4, EFFECT_NONE, 6, AMMO_NONE), 25, 13, 330, "", false, false, 0.f);
	BasicMonster boar("boar", F_MALE, 'b', TCODColor::darkOrange, 45, 0, Weapon(32, 15, 12, 2, 5, EFFECT_NONE, 1, AMMO_NONE), 50, 32, 550, "boar corpse", false, false, 0.f);
	BasicMonster snake("snake", F_MALE, 's', TCODColor::darkChartreuse, 23, 0, Weapon(30, 0, 6, 1, 4, EFFECT_POISON, 1, AMMO_NONE), 0, 12, 420, "snake corpse", false, false, 0.f);
	BasicMonster wolf("wolf", F_MALE, 'w', TCODColor::darkerOrange, 40, 0, Weapon(20, 10, 8, 2, 5, EFFECT_NONE, 1, AMMO_NONE), 25, 18, 590, "wolf corpse", false, false, 20.f);
	BasicMonster carnitree("carnivorous tree", F_MALE, 6, TCODColor(48, 79, 53), 60, 0, Weapon(50, 20, 25, 4, 5, EFFECT_NONE, 1, AMMO_NONE), 0, 500, 520, "", false, false, 0.f);
	BasicMonster bear("bear", F_MALE, 'B', TCODColor::darkSepia, 66, 0, Weapon(34, 25, 20, 1, 10, EFFECT_KNOCKBACK, 1, AMMO_NONE), 40, 37, 850, "bear corpse", false, false, 0.f);
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

	//                                                                     A   W   S   H   D   d dm  E  H  ef  R
	Weapon lightsaber("lightsaber", F_NEUTER, '(', TCODColor::red,         1,  1,  8, 30,  1, 10,20,30, 1, EFFECT_NONE, 1, AMMO_NONE, -1);
	Weapon dagger("dagger", F_NEUTER, '(', TCODColor::lightRed,            1,  4, 10, 25,  8,  1, 4, 0, 1, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon shortsword("shortsword", F_NEUTER, '(', TCODColor::red,         1, 20, 15, 23, 12,  1, 8, 0, 1, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon longsword("longsword", F_NEUTER, '(', TCODColor::darkRed,       1, 30, 20, 20, 18,  2, 8, 0, 1, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon halberd("halberd", F_NEUTER, '(', TCODColor::yellow,            1, 80, 30, 40, 20,  2,10, 0, 2, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon staff("staff", F_NEUTER, '(', TCODColor::black,                 1, 40, 20, 40, 10,  2, 8, 0, 2, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon axe("axe", F_NEUTER, '(', TCODColor::grey,                      1, 60, 20, 18, 18,  2,10, 0, 2, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon forestersaxe("foresters axe", F_NEUTER, '(', TCODColor::silver, 1, 40, 15, 15, 12,  2, 4, 0, 2, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon hammer("hammer", F_NEUTER, '(', TCODColor::orange,              1, 40, 15, 15, 12,  1, 4, 0, 2, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon warhammer("warhammer", F_NEUTER, '(', TCODColor::darkOrange,    1, 60, 20, 15, 10,  3,10, 0, 2, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon club("club", F_NEUTER, '(', TCODColor::desaturatedOrange,       1, 40, 20, 15, 10,  1,10, 0, 2, EFFECT_NONE, 1, AMMO_NONE, 0);
	Weapon shortbow("shortbow", F_NEUTER, '(', TCODColor::lightCyan,       1, 20, 15, 25,  8,  2, 4, 0, 2, EFFECT_NONE, 30, AMMO_ARROW, 0);
	Weapon longbow("longbow", F_NEUTER, '(', TCODColor::darkCyan,          1, 30, 20, 25, 10,  2, 6, 0, 2, EFFECT_NONE, 30, AMMO_ARROW, 0);
	Weapon crossbow("crossbow", F_NEUTER, '(', TCODColor::blue,            1, 40, 30, 25, 15,  2, 8, 0, 2, EFFECT_NONE, 20, AMMO_BOLT, 0);
	Weapon slingshot("slingshot", F_NEUTER, '(', TCODColor::lightBlue,     1,  4, 10, 30,  6,  2, 4, 0, 2, EFFECT_NONE, 10, AMMO_BULLET, 0);
	Armor pot("cooking pot", F_NEUTER, ')', TCODColor::grey, 1, 30, 2, 0, 5, ARMOR_HAT);
	Armor wizardhat("wizard hat", F_NEUTER, ')', TCODColor::purple, 1, 10, 1, 0, 0, ARMOR_HAT);
	Armor voodoomask("voodoo mask", F_NEUTER, ')', TCODColor::green, 1, 20, 4, 0, 2, ARMOR_HAT);
	Armor bandana("bandana", F_NEUTER, ')', TCODColor::red, 1, 4, 1, 1, 0, ARMOR_HAT);
	Armor leathercap("leather cap", F_NEUTER, ')', TCODColor::desaturatedOrange, 1, 20, 3, 1, 1, ARMOR_HAT);
	Armor ironhelmet("iron helmet", F_NEUTER, ')', TCODColor::silver, 1, 30, 5, 1, 2, ARMOR_HAT);
	Armor leatherboots("leather boots", F_NEUTER | F_BOOTS, ')', TCODColor::desaturatedOrange, 1, 20, 10, 0, 2, ARMOR_BOOTS);
	Armor ironboots("iron boots", F_NEUTER | F_BOOTS, ')', TCODColor::silver, 1, 30, 15, 0, 5, ARMOR_BOOTS);
	Armor elvensandals("elven sandals", F_NEUTER | F_BOOTS, ')', TCODColor::chartreuse, 1, 10, 10, 0, 0, ARMOR_BOOTS);
	Armor leatherarmor("leather armor", F_NEUTER, ')', TCODColor::desaturatedOrange, 1, 30, 10, 0, 1, ARMOR_BODY);
	Armor chainmail("chainmail", F_NEUTER, ')', TCODColor::lightGrey, 1, 40, 15, 0, 3, ARMOR_BODY);
	Armor scalemail("scalemail", F_NEUTER, ')', TCODColor::grey, 1, 50, 20, 0, 5, ARMOR_BODY);
	Armor platemail("platemail", F_NEUTER, ')', TCODColor::darkGrey, 1, 60, 25, 0, 7, ARMOR_BODY);
	Armor wizardrobe("wizard robe", F_NEUTER, ')', TCODColor::purple, 1, 20, 10, 0, 2, ARMOR_BODY);
	Food meat("meat (raw)", F_NEUTER, '%', TCODColor::lightRed, 1, 6, 500, 100);
	Food sandwich("sandwich", F_NEUTER, '%', TCODColor::orange, 1, 6, 2500, 100);
	Food bread("loaf of bread", F_NEUTER, '%', TCODColor::desaturatedOrange, 1, 6, 2000, 70);
	Food apple("apple", F_NEUTER | F_AN, '%', TCODColor::green, 1, 1, 1000, 15);
	Food berries("berry", F_NEUTER, '%', TCODColor::darkPurple, 1, 1, 1000, 15);
	Ammo arrow("arrow", F_NEUTER, '/', TCODColor::cyan, 5, 1, 0, EFFECT_NONE, AMMO_ARROW);
	Ammo bolt("bolt", F_NEUTER, '/', TCODColor::blue, 5, 1, 0, EFFECT_NONE, AMMO_BOLT);
	Ammo bullet("bullet", F_NEUTER, '/', TCODColor::lightBlue, 5, 1, 0, EFFECT_NONE, AMMO_BULLET);
	Tool lockpick("lockpick", F_NEUTER, '[', TCODColor::black, 1, 5, TOOL_LOCKPICK);
	Tool metalscrap("metal scrap", F_NEUTER, '[', TCODColor::darkerGrey, 1, 5, TOOL_METAL);
	Potion minorHeal("minor heal potion", F_NEUTER, '!', TCODColor::lightRed, 1, 3, POTION_MINORHEAL);
	Potion heal("heal potion", F_NEUTER, '!', TCODColor::red, 1, 3, POTION_HEAL);
	Potion fullHeal("full heal potion", F_NEUTER, '!', TCODColor::darkRed, 1, 3, POTION_FULLHEAL);
	Potion haste("haste potion", F_NEUTER, '!', TCODColor::blue, 1, 3, POTION_HASTE);
	SpellBook v_cripple("voodoo book named 'Cripple'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::purple, 0.4f), 40, SPELL_V_CRIPPLE);
	SpellBook v_frenzy("voodoo book named 'Frenzy'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::purple, 0.5f), 40, SPELL_V_FRENZY);
	SpellBook v_innerdemons("voodoo book named 'Inner Demons'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::purple, 0.6f), 40, SPELL_V_INNER_DEMONS);
	SpellBook v_feast("voodoo book named 'Feast'", F_NEUTER, 21, TCODColor::lerp(TCODColor::white, TCODColor::purple, 0.7f), 40, SPELL_V_FEAST);
	SpellBook e_fire("elemental book named 'Fireball'", F_NEUTER | F_AN, 21, TCODColor::lerp(TCODColor::white, TCODColor::azure, 0.4f), 40, SPELL_E_FIRE);
	Gold gold(1);

	factory.setTemplate("lightsaber", &lightsaber);
	factory.setTemplate("dagger", &dagger);
	factory.setTemplate("longsword", &longsword);
	factory.setTemplate("shortsword", &shortsword);
	factory.setTemplate("halberd", &halberd);
	factory.setTemplate("staff", &staff);
	factory.setTemplate("axe", &axe);
	factory.setTemplate("foresters axe", &forestersaxe);
	factory.setTemplate("hammer", &hammer);
	factory.setTemplate("warhammer", &warhammer);
	factory.setTemplate("club", &club);
	factory.setTemplate("longbow", &longbow);
	factory.setTemplate("shortbow", &shortbow);
	factory.setTemplate("crossbow", &crossbow);
	factory.setTemplate("slingshot", &slingshot);
	factory.setTemplate("cooking pot", &pot);
	factory.setTemplate("wizardhat", &wizardhat);
	factory.setTemplate("voodoomask", &voodoomask);
	factory.setTemplate("bandana", &bandana);
	factory.setTemplate("leathercap", &leathercap);
	factory.setTemplate("ironhelmet", &ironhelmet);
	factory.setTemplate("leatherboots", &leatherboots);
	factory.setTemplate("ironboots", &ironboots);
	factory.setTemplate("elvensandals", &elvensandals);
	factory.setTemplate("leatherarmor", &leatherarmor);
	factory.setTemplate("chainmail", &chainmail);
	factory.setTemplate("scalemail", &scalemail);
	factory.setTemplate("platemail", &platemail);
	factory.setTemplate("wizardrobe", &wizardrobe);
	factory.setTemplate("meat", &meat);
	factory.setTemplate("sandwich", &sandwich);
	factory.setTemplate("bread", &bread);
	factory.setTemplate("apple", &apple);
	factory.setTemplate("berries", &berries);
	factory.setTemplate("arrows", &arrow);
	factory.setTemplate("bolts", &bolt);
	factory.setTemplate("bullets", &bullet);
	factory.setTemplate("gold", &gold);
	factory.setTemplate("lockpick", &lockpick);
	factory.setTemplate("metalscrap", &metalscrap);

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