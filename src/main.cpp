#include <libtcod.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <deque>
#include <iostream>
#include <cassert>
#include "levelgen.hpp"
#include "world.hpp"
#include "utility.hpp"

World world;

int main()
{
	TCODConsole::initRoot(80,51,"deadmeat",false);
	TCODSystem::setFps(30);

	// Init hardcoded world
	LevelGen level_generator;
	world.viewLevel = Viewport(1, 1, TCODConsole::root->getWidth() - 10, TCODConsole::root->getHeight() - 4);
	world.viewMsg = Viewport(1, TCODConsole::root->getHeight() - 3, TCODConsole::root->getWidth(), 2);
	world.levels[0] = level_generator.generateCaveLevel(80, 50, 40.f);
	world.currentLevel = 0;
	world.player->moveTo(Point(35, 22));
	Goblin gobbo;
	FailWhale twitter, twitter2;
	world.levels[0]->addCreature(&gobbo);
	world.levels[0]->addCreature(&twitter);

	bool quit=false;
	while (!TCODConsole::isWindowClosed() && !quit)
	{
		Level* level = world.levels[world.currentLevel];
		while (level->isCreatureTurn(world.player->getActionTime()) && world.getNumMessages() <= 1)
		{
			level->performCreatureTurn();
		}

		// Show new game state
		world.drawWorld();
		world.popMessage();
		TCODConsole::root->flush();

		if (world.getNumMessages() > 0)
		{
			// Player has to clear pending messages
			TCOD_key_t key;
			do
			{
				key = TCODConsole::root->waitForKeypress(true);
				if (key.pressed && (key.lalt || key.ralt) && key.vk == TCODK_F4) quit = true;
			}
			while (!(key.pressed && key.vk == TCODK_SPACE) && !TCODConsole::isWindowClosed() && !quit);
		}
		else
		{
			// Player's turn to do something
			bool actionDone = false;
			do
			{
				TCOD_key_t key = TCODConsole::root->waitForKeypress(true);
				if (key.pressed && (key.lalt || key.ralt) && key.vk == TCODK_F4) quit = true;
				bool move(false);
				int direction(0);

				if (key.pressed && (key.vk >= TCODK_KP1 && key.vk <= TCODK_KP9))
				{
					// numpad player movement
					move = true;
					direction = key.vk - TCODK_KP1;
				}
				else if (key.pressed && (key.vk >= TCODK_1 && key.vk <= TCODK_9))
				{
					// number keys player movement
					move = true;
					direction = key.vk - TCODK_1;
				}

				if (move)
				{
					// execute movement
					Point ppos = world.player->getPos();
					int newx = ppos.x + Player::dx[direction];
					int newy = ppos.y + Player::dy[direction];
					if (newx >= 0 && newx < level->getWidth() && newy >= 0 && newy < level->getHeight())
					{
						if (world.tileSet->isPassable(level->getTile(Point(newx,newy))))
						{
							world.player->move(Point(Player::dx[direction], Player::dy[direction]));
							world.player->addActionTime(12);
							// TODO: this is hardcoded width. not good.
							ppos = world.player->getPos();
							world.levelOffset.x = util::clamp(35 - ppos.x, 70 - level->getWidth(), 0);
							world.levelOffset.y = util::clamp(22 - ppos.y, 44 - level->getHeight(), 0);
						}
						else
						{
							world.addMessage("Bonk!");
						}
					}
					actionDone = true;
				}
			}
			while (!actionDone && !TCODConsole::isWindowClosed() && !quit);
		}
	}

	return 0;
}
