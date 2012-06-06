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
#include "monsterfactory.hpp"

World world;
MonsterFactory monsterfactory;

Point getRandomLocation(Level* lev)
{
	TCODRandom* rng = TCODRandom::getInstance();
	Point p;
	do
	{
		p.x = rng->getInt(0, lev->getWidth()-1);
		p.y = rng->getInt(0, lev->getHeight()-1);
	}
	while (!world.tileSet->isPassable(lev->getTile(p)));
	return p;
}

int corruptSave(const std::string& fileName)
{
	TCODConsole::root->setDefaultBackground(TCODColor::blue);
	TCODConsole::root->setDefaultForeground(TCODColor::white);
	TCODConsole::root->clear();
	TCODConsole::root->printEx(40, 18, TCOD_BKGND_NONE, TCOD_CENTER, "ERROR - SAVEGAME IS CORRUPT\n\nDELETE CORRUPT SAVEGAME?\n\n[Y]es / [N]o");
	TCODConsole::root->flush();
	while (true)
	{
		TCOD_key_t key = TCODConsole::root->waitForKeypress(true);
		if (key.c == 'Y')
		{
			Savegame::deleteSave(fileName);
			return 0;
		}
		else if (key.c == 'N' || TCODConsole::root->isWindowClosed())
		{
			return 0;
		}
	}
}

int main()
{
	TCODConsole::initRoot(80,51,"deadmeat",false);
	TCODSystem::setFps(30);

	// Init hardcoded world
	LevelGen level_generator;
	world.viewLevel = Viewport(1, 1, TCODConsole::root->getWidth() - 16, TCODConsole::root->getHeight() - 5);
	world.viewMsg = Viewport(2, TCODConsole::root->getHeight() - 3, TCODConsole::root->getWidth() - 4, 2);
	world.viewInfo = Viewport(TCODConsole::root->getWidth() - 14, 3, 13, TCODConsole::root->getHeight() - 7);
	world.viewItemList = Viewport(
	                       world.viewLevel.x + world.viewLevel.width/8,
	                       world.viewLevel.y + world.viewLevel.height/8,
	                       world.viewLevel.width - world.viewLevel.width/4,
	                       world.viewLevel.height - world.viewLevel.height/4
	                     );

	if (Savegame::exists("monsters.txt"))
	{
		Savegame save;
		if (!save.loadSavegame("monsters.txt")) return corruptSave("monsters.txt");
	}

	if (false)
	{
		// Add/edit/remove creature templates here
		Goblin goblin("goblin", 'g', TCODColor::green, 35, 0,
			Weapon("claws", '#', TCODColor::pink, 11, 0, 4, 1, 3, 0, SKILL_UNARMED, 2), 15
		);
		monsterfactory.setTemplate("goblin", &goblin);
		Goblin snake("snake", 's', TCODColor::darkChartreuse, 20, 0,
			Weapon("teeth", '#', TCODColor::pink, 25, 0, 11, 2, 6, 0, SKILL_UNARMED, 0), 0
		);
		monsterfactory.setTemplate("snake", &snake);
		Goblin dragon("red dragon", 'D', TCODColor::red, 300, 100,
			Weapon("fangs", '#', TCODColor::pink, 15, 50, 19, 3, 7, 0, SKILL_UNARMED, 2), 75
		);
		monsterfactory.setTemplate("red dragon", &dragon);
		Savegame save;
		save.beginSave("monsters.txt");
		monsterfactory.save(save);
		save.endSave();
	}

	Savegame save;
	if (Savegame::exists("save.txt"))
	{
		if (!save.loadSavegame("save.txt")) return corruptSave("save.txt");
	}
	else
	{
		world.levels[0] = level_generator.generateCaveLevel(80, 50, 40.f);
		world.currentLevel = 0;
		Point newPos = getRandomLocation(world.levels[0]);
		world.player->getCreature()->moveTo(newPos);
		world.levelOffset.x = util::clamp(world.viewLevel.width/2 - newPos.x, world.viewLevel.width - world.levels[0]->getWidth(), 0);
		world.levelOffset.y = util::clamp(world.viewLevel.height/2 - newPos.y, world.viewLevel.height - world.levels[0]->getHeight(), 0);
		Weapon* sword = new Weapon("sword", '(', TCODColor::red, 10, 30, 1, 10, 20, 30, SKILL_SWORD, 1);
		Weapon* dagger = new Weapon("dagger", '(', TCODColor::red, 4, 10, 8, 1, 4, 12, SKILL_DAGGER, 1);
		Item* item1 = new Item("item1", '1', TCODColor::blue);
		Item* item2 = new Item("item2", '2', TCODColor::green);
		Weapon* mace = new Weapon("cursed mace", '(', TCODColor::red, 15, -60, 4, 1, 1, -60, SKILL_MACEFLAIL, 1);
		Armor* uber = new Armor("uber armor", ')', TCODColor::black, 150, 10, ARMOR_BODY, SKILL_PLATE_ARMOR);
		Armor* crap = new Armor("prussian pickelhaube", ')', TCODColor::black, 50, 0, ARMOR_HAT, SKILL_PLATE_ARMOR);
		Armor* boots = new Armor("clown shoes", ')', TCODColor::black, 30, 12, ARMOR_BOOTS, SKILL_CLOTH_ARMOR);
		std::string cr[6] = { "goblin","goblin","goblin","snake","snake","snake" };
		for (int i=0; i<6; i++)
		{
			Creature* c = monsterfactory.spawnCreature(cr[i]);
			c->moveTo(getRandomLocation(world.levels[0]));
			world.levels[0]->addCreature(c, 0);
		}
		world.levels[0]->addCreature(world.player->getCreature(), 0);
		world.levels[0]->addItem(dagger, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(item1, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(item2, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(sword, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(mace, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(uber, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(crap, getRandomLocation(world.levels[0]));
		world.levels[0]->addItem(boots, getRandomLocation(world.levels[0]));
	}

	while (!world.requestQuit)
	{
		Level* level = world.levels[world.currentLevel];
		while (!level->isPlayerTurn() && world.getNumMessages() <= 1 && !world.gameover)
		{
			level->performCreatureTurn();
		}

		// Show new game state
		world.drawWorld();
		TCODConsole::root->flush();

		if (world.getNumMessages() > 1)
		{
			// Player has to clear pending messages
			TCOD_key_t key;
			do
			{
				key = world.player->waitForKeypress(true);
			}
			while (key.vk != TCODK_SPACE && !world.requestQuit);
			world.popMessage();
		}
		else if (world.gameover)
		{
			// Game over
			world.drawBlockingWindow("GAME OVER", "You are dead!", TCODColor::red);
			world.requestQuit = true;
		}
		else
		{
			// This is the player turn made by a player controlled creature
			// We know this because level->isPlayerTurn() must be true here
			int nMsg = world.getNumMessages();
			level->performCreatureTurn();
			if (nMsg > 0) world.popMessage();
		}
	}

	if (world.gameover)
	{
		Savegame::deleteSave("save.txt");
	}
	else
	{
		save.saveWorld(world, "save.txt");
	}

	return 0;
}