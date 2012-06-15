#include "factory.hpp"
#include "creature.hpp"
#include "item.hpp"
#include "items/weapon.hpp"
#include "items/armor.hpp"
#include "items/food.hpp"
#include "items/ammo.hpp"

void fillFactoryTemplates()
{
	// Add/edit/remove creature templates here
	Goblin goblin("goblin", F_MALE, 'g', TCODColor::green, 35, 0,
				  Weapon("claws", F_DEFAULT, '#', TCODColor::pink, 11, 0, 4, 1, 3, 0, 2, EFFECT_NONE, 1), 15, 10, 300
				 );
	Goblin snake("snake", F_MALE , 's', TCODColor::darkChartreuse, 20, 0,
				 Weapon("teeth", F_DEFAULT, '#', TCODColor::pink, 12, 0, 6, 1, 4, 0, 0, EFFECT_NONE, 1), 0, 5, 300
				);
	Goblin troll("troll", F_MALE, 'T', TCODColor::darkBlue, 80, 0,
				 Weapon("hands", F_DEFAULT, '#', TCODColor::pink, 18, 20, 14, 1, 5, 0, 2, EFFECT_NONE, 1), 40, 15, 750
				);
	Goblin dragon("Smaug", F_PROPER | F_MALE, 'D', TCODColor::red, 300, 100,
				  Weapon("fangs", F_DEFAULT, '#', TCODColor::pink, 15, 50, 19, 3, 7, 0, 2, EFFECT_NONE, 1), 75, 30, 2500
				 );

	factory.setTemplate("goblin", &goblin);
	factory.setTemplate("snake", &snake);
	factory.setTemplate("troll", &troll);
	factory.setTemplate("red dragon", &dragon);

	Weapon sword("sword", F_NEUTER, '(', TCODColor::red, 10, 30, 1, 10, 20, 30, 1, EFFECT_NONE, 1);
	Weapon dagger("dagger", F_NEUTER, '(', TCODColor::red, 4, 10, 8, 1, 4, 12, 1, EFFECT_NONE, 1);
	Item item1("item1", F_NEUTER | F_AN, '1', TCODColor::blue);
	Item item2("item2", F_NEUTER | F_AN, '2', TCODColor::green);
	Weapon bow("longbow", F_NEUTER, '(', TCODColor::cyan, 25, 25, 30, 2, 10, 0, 1, EFFECT_NONE, 20);
	Armor body("uber armor", F_NEUTER | F_AN, ')', TCODColor::black, 150, 2, 10, ARMOR_BODY);
	Armor helmet("prussian pickelhaube", F_NEUTER, ')', TCODColor::black, 50, 1, 0, ARMOR_HAT);
	Armor boots("clown shoes", F_NEUTER | F_BOOTS, ')', TCODColor::black, -30, -1, 12, ARMOR_BOOTS);
	Food food1("beefsteak", F_NEUTER, '%', TCODColor::darkOrange, 2500);
	Food food2("meat ball", F_NEUTER, '%', TCODColor::orange, 1000);
	Ammo arrow("arrow", F_NEUTER, '!', TCODColor::cyan, 0, EFFECT_NONE);
	
	factory.setTemplate("sword", &sword);
	factory.setTemplate("dagger", &dagger);
	factory.setTemplate("item1", &item1);
	factory.setTemplate("item2", &item2);
	factory.setTemplate("longbow", &bow);
	factory.setTemplate("uber armor", &body);
	factory.setTemplate("pickelhaube", &helmet);
	factory.setTemplate("clown shoes", &boots);
	factory.setTemplate("beefsteak", &food1);
	factory.setTemplate("meatball", &food2);
	factory.setTemplate("arrows", &arrow);
}