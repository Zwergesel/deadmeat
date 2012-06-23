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
#include "items/food.hpp"
#include "items/ammo.hpp"
#include "factory.hpp"

World world;
Factory factory;
void fillFactoryTemplates();

int corruptSave(const std::string& fileName)
{
	TCODConsole::root->setDefaultBackground(TCODColor::blue);
	TCODConsole::root->setDefaultForeground(TCODColor::white);
	TCODConsole::root->clear();
	TCODConsole::root->printEx(40, 18, TCOD_BKGND_NONE, TCOD_CENTER, "ERROR - SAVEGAME \"%s\" IS CORRUPT\n\nDELETE CORRUPT SAVEGAME?\n\n[Y]es / [N]o", fileName.c_str());
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
	TCODConsole::setCustomFont("Md_curses_16x16.png",TCOD_FONT_LAYOUT_ASCII_INROW);
	TCODConsole::initRoot(80,50,"deadmeat",false);
	TCODSystem::setFps(30);

	// Init hardcoded world
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

	LevelGen::loadLootTable(); // TODO: temporarily here

	if (true)
	{
		// TODO: this code should not be necessary once all data is stored in a savegame
		fillFactoryTemplates();
		Savegame save;
		save.beginSave("monsters.txt");
		factory.save(save);
		save.endSave();
	}

	Savegame save;
	if (Savegame::exists("save.txt"))
	{
		if (!save.loadSavegame("save.txt")) return corruptSave("save.txt");
	}
	else
	{
		LevelGen::generateWorld();
		world.levels[0] = LevelGen::generateLevel(0, LEVELTYPE_ROOM);

		// Add player creature
		Point newPos = world.levels[0]->getRandomLocation(WALKABLE);
		world.player->getCreature()->moveTo(newPos);
		world.levels[0]->addCreature(world.player->getCreature(), 0);
		world.levelOffset.x = util::clamp(world.viewLevel.width/2 - newPos.x, world.viewLevel.width - world.levels[0]->getWidth(), 0);
		world.levelOffset.y = util::clamp(world.viewLevel.height/2 - newPos.y, world.viewLevel.height - world.levels[0]->getHeight(), 0);

		world.player->getCreature()->addItem(factory.spawnItem("lightsaber"));
		world.player->getCreature()->addItem(factory.spawnItem("longbow"));
		world.player->getCreature()->addItem(factory.spawnItem("arrows"));
		world.player->getCreature()->addItem(factory.spawnItem("crossbow"));
		world.player->getCreature()->addItem(factory.spawnItem("bolts"));
		world.player->getCreature()->addItem(factory.spawnItem("slingshot"));
		world.player->getCreature()->addItem(factory.spawnItem("bullets"));
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
			world.drawBlockingWindow("GAME OVER", "You are dead!", " ", TCODColor::red);
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