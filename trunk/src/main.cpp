#include <libtcod.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <deque>
#include <iostream>
#include <cassert>
#include "levelgen.hpp"
#include "player.hpp"
#include "tileset.hpp"
#include "creature.hpp"

Player player("test");
TileSet globalTileSet;

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

	LevelGen level_generator;
	Level* l = level_generator.generateCaveLevel(180,150);
	player.x = 40;
	player.y = 25;
	player.nextAction = 0;
	Goblin gobbo;
	FailWhale twitter;
	int levelScrollX = 0;
	int levelScrollY = 0;
	std::deque<std::string> messages;
	std::vector<TimelineAction> timeline;

	timeline.push_back(TimelineAction(0, &gobbo));
	timeline.push_back(TimelineAction(0, &twitter));
	make_heap(timeline.begin(), timeline.end());

	TCODConsole::root->clear();
	l->display(levelScrollX, levelScrollY);
	player.draw(levelScrollX, levelScrollY);
	gobbo.draw(levelScrollX, levelScrollY);

	while (!TCODConsole::isWindowClosed())
	{
		while (!timeline.empty() && timeline.front().time < player.nextAction && messages.size() <= 1)
		{
			// Take one creature; update it's action
			pop_heap(timeline.begin(), timeline.end());
			int time = timeline.back().actor->action(l, player, &messages);
			// action(...) returns the time the action took; update heap
			assert(time > 0);
			timeline.back().time += time;
			push_heap(timeline.begin(), timeline.end());
		}

		// Show new game state
		TCODConsole::root->clear();
		l->display(levelScrollX, levelScrollY);
		player.draw(levelScrollX, levelScrollY);
		// TODO: this stuff belong in level later
		gobbo.draw(levelScrollX, levelScrollY);
		twitter.draw(levelScrollX, levelScrollY);

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
					int newx = player.x + Player::dx[direction];
					int newy = player.y + Player::dy[direction];
					if (newx >= 0 && newx < l->getWidth() && newy >= 0 && newy < l->getHeight())
					{
						if (globalTileSet.info[l->getTile(newx, newy)].passable)
						{
							player.x = newx;
							player.y = newy;
							player.nextAction += 12;
							levelScrollX = std::min(l->getWidth() - 80, std::max(0, player.x - 40));
							levelScrollY = std::min(l->getHeight() - 50, std::max(0, player.y - 25));
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

	delete l;
	return 0;
}
