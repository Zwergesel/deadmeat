#include <cassert>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "world.hpp"
#include "player.hpp"
#include "tileset.hpp"
#include "level.hpp"
#include "creature.hpp"
#include "items/weapon.hpp"
#include "itemselection.hpp"
#include "savegame.hpp"

World::World()
{
	player = new Player("PlayerName");
	tileSet = new TileSet();
	levels = new Level*[10];
	currentLevel = 0;
	levelOffset = Point(0,0);
	requestQuit = false;
  gameover = false;
}

World::~World()
{
	if (player != NULL)
	{
		delete player;
		player = NULL;
	}
	if (tileSet != NULL)
	{
		delete tileSet;
		tileSet = NULL;
	}
	if (levels != NULL)
	{
		delete[] levels;
		levels = NULL;
	}
}

/* forceBreak is optional (default: false) */
void World::addMessage(std::string m, bool forceBreak)
{
	if (messageQueue.empty())
	{
		messageQueue.push_back(m);
	}
	else if (forceBreak)
	{
		messageQueue.back().append(" <More>");
		messageQueue.push_back(m);
	}
	else
	{
		std::string combine = messageQueue.back() + " " + m;
		int expectedHeight = TCODConsole::root->getHeightRect(
		                       viewMsg.x, 0, viewMsg.width, 100,
		                       "%s <More>", combine.c_str());
		if (expectedHeight <= viewMsg.height)
		{
			messageQueue.pop_back();
			messageQueue.push_back(combine);
		}
		else
		{
			messageQueue.back().append(" <More>");
			messageQueue.push_back(m);
		}
	}
}

int World::getNumMessages()
{
	return messageQueue.size();
}

void World::popMessage()
{
	if (!messageQueue.empty())
	{
		messageQueue.pop_front();
	}
}

void World::drawMessage()
{
	// TODO: colors
	if (!messageQueue.empty())
	{
		TCODConsole::root->printRect(
		  viewMsg.x, viewMsg.y, viewMsg.width, viewMsg.height, messageQueue.front().c_str()
		);
	}
}

void World::drawLevel(Level* level, Point offset, Viewport view)
{
	// draw level
	int startX = (offset.x < 0) ? -offset.x : 0;
	int startY = (offset.y < 0) ? -offset.y : 0;
	int rangeX = std::min(level->getWidth(), view.width - offset.x);
	int rangeY = std::min(level->getHeight(), view.height - offset.y);
	for (int y=startY; y<rangeY; y++)
	{
		for (int x=startX; x<rangeX; x++)
		{
			TileInfo inf = tileSet->getInfo(level->getTile(Point(x, y)));
			TCODConsole::root->putCharEx(
			  view.x + x + offset.x,
			  view.y + y + offset.y,
			  inf.symbol,
			  inf.color,
			  inf.background
			);
		}
	}
	// draw items
	std::vector<std::pair<Point, Item*> > items = level->getItems();
	for (std::vector<std::pair<Point, Item*> >::iterator it=items.begin(); it<items.end(); it++)
	{
		drawItem((*it).second, (*it).first, offset, view);
	}
	// draw creatures
	std::vector<Creature*> creatures = level->getCreatures();
	for (std::vector<Creature*>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		drawCreature(*it, offset, view);
	}
}

void World::drawItem(Item* i, Point pos, Point offset, Viewport view)
{
	pos += offset;
	if (pos.x >= 0 && pos.x < view.width && pos.y >= 0 && pos.y < view.height)
	{
		TCODConsole::root->putChar(view.x + pos.x, view.y + pos.y, i->getSymbol());
		TCODConsole::root->setCharForeground(view.x + pos.x, view.y + pos.y, i->getColor());
	}
}

void World::drawCreature(Creature* c, Point offset, Viewport view)
{
	Point pos = c->getPos();
	pos += offset;
	if (pos.x >= 0 && pos.x < view.width && pos.y >= 0 && pos.y < view.height)
	{
		TCODConsole::root->putChar(view.x + pos.x, view.y + pos.y, c->getSymbol());
		TCODConsole::root->setCharForeground(view.x + pos.x, view.y + pos.y, c->getColor());
	}
}

void World::drawWorld()
{
	TCODConsole::root->clear();
	drawLevel(levels[currentLevel], levelOffset, viewLevel);
	STATE state = player->getState();
	if (state == STATE_INVENTORY || state == STATE_PICKUP || state == STATE_WIELD || state == STATE_WEAR || state == STATE_DROP) drawItemSelection(world.itemSelection);
	drawMessage();
	drawInfo();
}

void World::drawInfo()
{
	std::pair<int,int> health = player->getCreature()->getHealth();
	TCODConsole::root->setColorControl(TCOD_COLCTRL_1, TCODColor::green, TCODColor::black);
	TCODConsole::root->printEx(TCODConsole::root->getWidth()-2, 3, TCOD_BKGND_NONE, TCOD_RIGHT, "%c%d/%d%c", TCOD_COLCTRL_1, health.first, health.second, TCOD_COLCTRL_STOP);
}

void World::toggleFullscreen()
{
	TCODConsole::root->setFullscreen(!TCODConsole::root->isFullscreen());
	drawWorld();
	TCODConsole::root->flush();
}

void World::drawItemSelection(ItemSelection& sel)
{
	TCODConsole window(viewItemList.width, viewItemList.height);
	window.printFrame(0, 0, window.getWidth(), window.getHeight(), true, TCOD_BKGND_DEFAULT, sel.getTitle().c_str());

	sel.resetDraw();
	bool category;
	int row;
	std::string text;
	while (sel.hasDrawLine())
	{
		text = sel.getNextLine(&row, &category);
		if (category)
		{
			window.printEx(window.getWidth() / 2, row, TCOD_BKGND_DEFAULT, TCOD_CENTER, text.c_str());
		}
		else
		{
			window.printEx(4, row, TCOD_BKGND_DEFAULT, TCOD_LEFT, text.c_str());
		}
	}

	TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, viewItemList.x, viewItemList.y, 1.f, 0.9f);
}

unsigned char World::drawBlockingWindow(std::string title, std::string text, TCODColor color, std::string acceptedKeys)
{
	int w = viewItemList.width;
	int h = 4 + TCODConsole::root->getHeightRect(0,0, w - 4, 100, text.c_str());
	TCODConsole window(w, h);
  window.setDefaultBackground(color);
	window.printFrame(0, 0, window.getWidth(), window.getHeight(), true, TCOD_BKGND_DEFAULT, title.c_str());
	window.printRectEx(window.getWidth()/2, 2, w - 4, h - 4, TCOD_BKGND_DEFAULT, TCOD_CENTER, text.c_str());
	TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, viewItemList.x, viewLevel.y + viewLevel.height/2 - h/2, 1.f, 0.9f);
	TCODConsole::root->flush();
	TCOD_key_t key;
	bool done=false;
	do
	{
		key = player->waitForKeypress(true);
    if(acceptedKeys.find_first_of(key.c)  != std::string::npos) done = true;
	}
	while (!done && !world.requestQuit);
	return key.c;
}

void World::drawBlockingWindow(std::string title, std::string text, TCODColor color)
{
  int w = viewItemList.width;
	int h = 4 + TCODConsole::root->getHeightRect(0,0, w - 4, 100, text.c_str());
	TCODConsole window(w, h);
  window.setDefaultBackground(color);
	window.printFrame(0, 0, window.getWidth(), window.getHeight(), true, TCOD_BKGND_DEFAULT, title.c_str());
	window.printRectEx(window.getWidth()/2, 2, w - 4, h - 4, TCOD_BKGND_DEFAULT, TCOD_CENTER, text.c_str());
	TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, viewItemList.x, viewLevel.y + viewLevel.height/2 - h/2, 1.f, 0.9f);
	TCODConsole::root->flush();
	TCOD_key_t key;
	do
	{
		key = player->waitForKeypress(true);
	}
  while (key.vk != TCODK_SPACE && !world.requestQuit);
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int World::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("World", id);
	store ("currentLevel", currentLevel) ("levelOffset", levelOffset);
	// TODO : all levels
	store.ptr("player", player->save(sg)) ("#levels", 1) .ptr("_level", levels[0]->save(sg));
	sg << store;
	return id;
}

void World::load(LoadBlock& load)
{
	load ("currentLevel", currentLevel) ("levelOffset", levelOffset);
	// TODO: delete player and levels [MEMORY LEAK]
	player = static_cast<Player*>(load.ptr("player"));
	int n;
	load ("#levels", n);
	for (int i=0; i<n; i++)
	{
		levels[i] = static_cast<Level*>(load.ptr("_level"));
	}
}