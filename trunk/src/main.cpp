#include <libtcod.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <deque>
#include <iostream>
#include <cassert>
#include "levelgen.hpp"
#include "world.hpp"
#include "utility.hpp"
#include "player.hpp"
#include "creature.hpp"
#include "level.hpp"
#include "items/weapon.hpp"

World world;

int main()
{
	TCODConsole::initRoot(80,51,"deadmeat",false);
	TCODSystem::setFps(30);

	// Init hardcoded world
	//LevelGen level_generator;
	world.viewLevel = Viewport(1, 1, TCODConsole::root->getWidth() - 10, TCODConsole::root->getHeight() - 5);
	world.viewMsg = Viewport(2, TCODConsole::root->getHeight() - 3, TCODConsole::root->getWidth() - 4, 2);
	world.levels[0] = new Level(80,50);//level_generator.generateCaveLevel(80, 50, 40.f);
	world.currentLevel = 0;
	world.player->getCreature()->moveTo(Point(35, 22));
	Goblin* gobbo = new Goblin();
	FailWhale* twitter = new FailWhale();
	Creature* defaulto = new Creature(Point(10,10), "default", 'd', TCODColor::amber, 20);
	Weapon* dagger = new Weapon(Point(33,33), "dagger", '(', TCODColor::red, 10, 30, 1, 10, 20, 30, SKILL_DAGGER, 1);
	Item* item1 = new Item(Point(40,40), "item1", '1', TCODColor::blue);
	Item* item2 = new Item(Point(40,40), "item2", '2', TCODColor::green);
	world.levels[0]->addCreature(gobbo);
	world.levels[0]->addCreature(twitter);
	world.levels[0]->addCreature(defaulto);
	world.levels[0]->addCreature(world.player->getCreature());
	world.levels[0]->addItem(dagger);
	world.levels[0]->addItem(item1);
	world.levels[0]->addItem(item2);

	while (!world.requestQuit)
	{
		Level* level = world.levels[world.currentLevel];
		while (!level->isPlayerTurn() && world.getNumMessages() <= 1)
		{
			level->performCreatureTurn();
		}

		// Show new game state
		world.drawWorld();
		// TODO: this somewhat breaks fullscreen, so fix it later
		world.popMessage();
		TCODConsole::root->flush();

		if (world.getNumMessages() > 0)
		{
			// Player has to clear pending messages
			TCOD_key_t key;
			do
			{
				key = world.player->waitForKeypress(true);
			}
			while (key.vk != TCODK_SPACE && !world.requestQuit);
		}
		else
		{
			// This is the player turn made by a player controlled creature
			// We know this because level->isPlayerTurn() must be true here
			level->performCreatureTurn();
		}
	}

	return 0;
}
