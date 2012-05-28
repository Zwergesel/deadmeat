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

struct TimelineAction
{
	int time;
	Creature* actor;
	TimelineAction(int t, Creature* c):time(t),actor(c) {};
};

bool operator<(TimelineAction a, TimelineAction b)
{
	// Max heap, but we want minimum time
	return a.time > b.time;
}

int main()
{
	TCODConsole::initRoot(80,51,"deadmeat",false);
	TCODSystem::setFps(30);

	// Init hardcoded world
	LevelGen level_generator;
	world.levels[0] = level_generator.generateCaveLevel(180,150);
	world.currentLevel = 0;
	world.player->moveTo(35, 22);
	Goblin gobbo;
	FailWhale twitter;
	std::deque<std::string> messages;
	std::vector<TimelineAction> timeline;

	timeline.push_back(TimelineAction(0, &gobbo));
	timeline.push_back(TimelineAction(0, &twitter));
	make_heap(timeline.begin(), timeline.end());

	world.debugDrawWorld(&gobbo, &twitter);

	while (!TCODConsole::isWindowClosed())
	{
		while (!timeline.empty() && timeline.front().time < world.player->getActionTime() && messages.size() <= 1)
		{
			// Take one creature; update it's action
			pop_heap(timeline.begin(), timeline.end());
			int time = timeline.back().actor->action(world.levels[world.currentLevel], world.player, &messages);
			// action(...) returns the time the action took; update heap
			assert(time > 0);
			timeline.back().time += time;
			push_heap(timeline.begin(), timeline.end());
		}

		// Show new game state
		world.debugDrawWorld(&gobbo, &twitter);

		// Show oldest message, if there any
		if (!messages.empty())
		{
			std::string message = messages.front();
			messages.pop_front();
			if (!messages.empty())
			{
				message.append(" <More>");
			}
			TCODConsole::root->print(0, 50, message.c_str());
			TCODConsole::root->print(0, 50, message.c_str());
		}

		TCODConsole::root->flush();

		if (!messages.empty())
		{
			// Player has to clear pending messages
			TCOD_key_t key;
			do
			{
				key = TCODConsole::root->waitForKeypress(true);
			}
			while (!key.pressed || key.vk != TCODK_SPACE);
		}
		else
		{
			// Player's turn to do something
			bool actionDone = false;
			do
			{
				TCOD_key_t key = TCODConsole::root->waitForKeypress(true);
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
					Level* level = world.levels[world.currentLevel];
					int newx = world.player->getX() + Player::dx[direction];
					int newy = world.player->getY() + Player::dy[direction];
					if (newx >= 0 && newx < level->getWidth() && newy >= 0 && newy < level->getHeight())
					{
						if (world.tileSet->isPassable(level->getTile(newx,newy)))
						{
							world.player->move(Player::dx[direction], Player::dy[direction]);
							world.player->addActionTime(12);
							world.levelOffset.x = util::clamp(35 - world.player->getX(), 70 - level->getWidth(), 0);
							world.levelOffset.y = util::clamp(22 - world.player->getY(), 44 - level->getHeight(), 0);
						}
						else
						{
							messages.push_back("Bonk!");
						}
					}
					actionDone = true;
				}
			}
			while (!actionDone && !TCODConsole::isWindowClosed());
		}

	}

	return 0;
}
