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
#include "world.hpp"

void fillFactoryTemplates()
{
	// Add/edit/remove creature templates here
	BasicMonster goblin("goblin", F_MALE, 'g', TCODColor::green, 35, 0, Weapon(11, 0, 4, 1, 3, EFFECT_NONE, 1, AMMO_NONE), 15, 10, 300, true, true);
	BasicMonster snake("snake", F_MALE , 's', TCODColor::darkChartreuse, 20, 0, Weapon(12, 0, 6, 1, 4, EFFECT_NONE, 1, AMMO_NONE), 0, 5, 300, false, false);
	BasicMonster troll("troll", F_MALE, 'T', TCODColor::darkBlue, 80, 0, Weapon(18, 20, 14, 1, 5, EFFECT_NONE, 1, AMMO_NONE), 40, 15, 750, false, false);
	BasicMonster spider("spider", F_FEMALE, 's', TCODColor::pink, 15, 0, Weapon(14, 0, 3, 2, 3, EFFECT_NONE, 1, AMMO_NONE), 60, 8, 150, false, false);
	BasicMonster rat("rat", F_FEMALE, 'r', TCODColor::darkYellow, 40, 0, Weapon(10, 0, 1, 1, 12, EFFECT_NONE, 1, AMMO_NONE), 20, 11, 250, false, false);
	BasicMonster ant("soldier ant", F_FEMALE, 'a', TCODColor::blue, 25, 0, Weapon(13, 40, 12, 2, 8, EFFECT_NONE, 1, AMMO_NONE), 0, 14, 500, false, false);
	BasicMonster soldier("soldier", F_MALE, '@', TCODColor::blue, 120, 0, Weapon(16, 0, 3, 1, 2, EFFECT_NONE, 1, AMMO_NONE), 0, 14, 600, true, true);
	BasicMonster sergeant("sergeant", F_MALE, '@', TCODColor::red, 180, 0, Weapon(13, 0, 3, 1, 2, EFFECT_NONE, 1, AMMO_NONE), 40, 14, 1000, true, true);
	BasicMonster medic("medic", F_FEMALE, '@', TCODColor::pink, 180, 0, Weapon(25, 0, 0, 0, 0, EFFECT_NONE, 1, AMMO_NONE), 0, 12, 10, true, true);
	BasicMonster redbaby("red baby dragon", F_MALE, 'd', TCODColor::red, 55, 0, Weapon(16, 15, 5, 1, 10, EFFECT_NONE, 1, AMMO_NONE), 20, 13, 600, false, false);
	BasicMonster purbaby("purple baby dragon", F_MALE, 'd', TCODColor::violet, 55, 0, Weapon(16, 15, 5, 1, 10, EFFECT_NONE, 1, AMMO_NONE), 20, 13, 600, false, false);
	BasicMonster blubaby("blue baby dragon", F_MALE, 'd', TCODColor::blue, 55, 0, Weapon(16, 15, 5, 1, 10, EFFECT_NONE, 1, AMMO_NONE), 20, 13, 600, false, false);
	BasicMonster dragon("Smaug", F_PROPER | F_MALE, 'D', TCODColor::red, 300, 100, Weapon(15, 50, 19, 3, 7, EFFECT_NONE, 1, AMMO_NONE), 75, 16, 2500, false, false);

	InventoryTable fighter_weapons;
	std::vector<std::string> wpns;
	wpns.push_back("halberd");
	wpns.push_back("sword");
	wpns.push_back("dagger");
	fighter_weapons.add(wpns, 850, 1, 1);
	fighter_weapons.add("longbow", 200);

	factory.setTemplate("goblin", &goblin);
	factory.setTemplate("snake", &snake);
	factory.setTemplate("troll", &troll);
	factory.setTemplate("spider", &spider);
	factory.setTemplate("rat", &rat);
	factory.setTemplate("soldier ant", &ant);
	factory.setTemplate("soldier", &soldier, fighter_weapons);
	factory.setTemplate("sergeant", &sergeant, fighter_weapons);
	factory.setTemplate("medic", &medic);
	factory.setTemplate("red baby dragon", &redbaby);
	factory.setTemplate("purple baby dragon", &purbaby);
	factory.setTemplate("blue baby dragon", &blubaby);
	factory.setTemplate("Smaug", &dragon);

	Weapon lightsaber("lightsaber", F_NEUTER, '(', TCODColor::red, 1, 1, 8, 30, 1, 10, 20, 30, 1, EFFECT_NONE, 1, AMMO_NONE);
	Weapon sword("long sword", F_NEUTER, '(', TCODColor::red, 1, 10, 15, 20, 12, 2, 8, 0, 1, EFFECT_NONE, 1, AMMO_NONE);
	Weapon dagger("dagger", F_NEUTER, '(', TCODColor::red, 1, 2, 7, 10, 8, 1, 4, 0, 1, EFFECT_NONE, 1, AMMO_NONE);
	Weapon halberd("halberd", F_NEUTER, '(', TCODColor::red, 1, 20, 22, 40, 15, 2, 10, 0, 2, EFFECT_NONE, 1, AMMO_NONE);
	Weapon bow("longbow", F_NEUTER, '(', TCODColor::cyan, 1, 10, 25, 25, 30, 2, 10, 0, 1, EFFECT_NONE, 30, AMMO_ARROW);
	Weapon crossbow("crossbow", F_NEUTER, '(', TCODColor::blue, 1, 15, 35, 25, 40, 3, 10, 0, 1, EFFECT_NONE, 20, AMMO_BOLT);
	Weapon slingshot("slingshot", F_NEUTER, '(', TCODColor::lightBlue, 1, 1, 15, 35, 10, 1, 10, 0, 1, EFFECT_NONE, 10, AMMO_BULLET);
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
	Gold gold(12);

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
}