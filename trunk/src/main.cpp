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

World world;

int main()
{
	TCODConsole::initRoot(80,51,"deadmeat",false);
	TCODSystem::setFps(30);

	// Init hardcoded world
	LevelGen level_generator;
	world.viewLevel = Viewport(1, 1, TCODConsole::root->getWidth() - 10, TCODConsole::root->getHeight() - 5);
	world.viewMsg = Viewport(2, TCODConsole::root->getHeight() - 3, TCODConsole::root->getWidth()-4, 2);
	world.levels[0] = level_generator.generateCaveLevel(80, 50, 40.f);
	world.currentLevel = 0;
	world.player->moveTo(Point(35, 22));
	Goblin* gobbo = new Goblin();
	FailWhale* twitter = new FailWhale();
	Creature* defaulto = new Creature(Point(10,10), "default", 'd', TCODColor::amber, 20);
	TCODRandom rngGauss;
	rngGauss.setDistribution(TCOD_DISTRIBUTION_GAUSSIAN_RANGE);
	world.levels[0]->addCreature(gobbo);
	world.levels[0]->addCreature(twitter);
	world.levels[0]->addCreature(defaulto);

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
					Point newPos = Point(ppos.x + Player::dx[direction], ppos.y + Player::dy[direction]);
					if (newPos.x >= 0 && newPos.y < level->getWidth() && newPos.y >= 0 && newPos.y < level->getHeight())
					{
						Creature* c = level->creatureAt(newPos);
						if (c != NULL)
						{
							int attack,damage,speed;
							world.player->attack(attack, damage, speed);
							int hit = rngGauss.getInt(-300,300,attack - c->getDefense());
							if (hit >= -70)
							{
								if (hit <= 0) damage /= 2;
								if (hit > 175) damage *= 2;
								std::stringstream msg;
								if (c->hurt(damage, NULL))
								{
									msg << "You kill the " << c->getName() << ".";
									level->removeCreature(c);
								}
								else
								{
									msg << "You hit the " << c->getName() << " for " << damage << " damage.";
								}
								world.addMessage(msg.str());
							}
							else
							{
								std::stringstream msg;
								msg << "You miss the " << c->getName() << ".";
								world.addMessage(msg.str());
							}
							world.player->addActionTime(speed);
						}
						else if (world.tileSet->isPassable(level->getTile(newPos)))
						{
							world.player->move(Point(Player::dx[direction], Player::dy[direction]));
							world.player->addActionTime(12);
							// TODO: this is hardcoded width. not good.
							ppos = world.player->getPos();
							world.levelOffset.x = util::clamp(35 - ppos.x, 70 - level->getWidth(), 0);
							world.levelOffset.y = util::clamp(22 - ppos.y, 46 - level->getHeight(), 0);
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