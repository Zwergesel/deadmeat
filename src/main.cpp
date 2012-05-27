#include <libtcod.hpp>
#include <algorithm>
#include <string>
#include "levelgen.hpp"
#include "player.hpp"
#include "tileset.hpp"

Player player("test");
TileSet tileSet;

int main()
{
	TCODConsole::initRoot(80,51,"deadmeat",false);
	TCODSystem::setFps(30);

	LevelGen level_generator;
	Level* l = level_generator.generateCaveLevel(180,150);
	player.x = 40;
	player.y = 25;
	int levelScrollX = 0;
	int levelScrollY = 0;
	std::string message;

	while (!TCODConsole::isWindowClosed())
	{
		/// RENDER
		TCODConsole::root->clear();
		int startX = (levelScrollX < 0) ? -levelScrollX : 0;
		int startY = (levelScrollY < 0) ? -levelScrollY : 0;
		int rangeX = std::min(l->getWidth() - levelScrollX, TCODConsole::root->getWidth());
		int rangeY = std::min(l->getHeight() - levelScrollY, TCODConsole::root->getHeight() - 1);
		for (int y=startY; y<rangeY; y++)
		{
			for (int x=startX; x<rangeX; x++)
			{
				TileInfo inf = tileSet.info[l->getTile(x + levelScrollX, y + levelScrollY)];
				TCODConsole::root->putCharEx(x, y, inf.symbol, inf.color, inf.background);
			}
		}
		TCODConsole::root->print(0, 50, message.c_str());
		TCODConsole::root->setChar(player.x - levelScrollX, player.y - levelScrollY, player.symbol);
		TCODConsole::root->flush();

		/// INPUT
		TCOD_key_t key = TCODConsole::root->waitForKeypress(true);
		bool move(false);
		int direction(0);

		if (key.vk == TCODK_ENTER && key.lalt)
		{
			// toogle fullscreen
			TCODConsole::root->setFullscreen(!TCODConsole::isFullscreen());
		}
		else if (key.vk >= TCODK_KP1 && key.vk <= TCODK_KP9)
		{
			// numpad player movement
			move = true;
			direction = key.vk - TCODK_KP1;
		}
		else if (key.vk >= TCODK_1 && key.vk <= TCODK_9)
		{
			// number keys player movement
			move = true;
			direction = key.vk - TCODK_1;
		}

		if (move)
		{
			// execute movement
			message="";
			int newx = player.x + Player::dx[direction];
			int newy = player.y + Player::dy[direction];
			if (newx >= 0 && newx < l->getWidth() && newy >= 0 && newy < l->getHeight())
			{
				if (tileSet.info[l->getTile(newx, newy)].passable)
				{
					player.x = newx;
					player.y = newy;
					levelScrollX = std::min(l->getWidth() - 80, std::max(0, player.x - 40));
					levelScrollY = std::min(l->getHeight() - 50, std::max(0, player.y - 25));
				}
				else
				{
					message = "Bonk!";
				}
			}
		}
	}

	delete l;
	return 0;
}