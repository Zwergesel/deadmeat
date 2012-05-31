#include <cassert>
#include <sstream>
#include "world.hpp"
#include "player.hpp"
#include "tileset.hpp"
#include "level.hpp"
#include "creature.hpp"
#include "items/weapon.hpp"

World::World()
{
	player = new Player("PlayerName");
	tileSet = new TileSet();
	levels = new Level*[10];
	currentLevel = 0;
	levelOffset = Point(0,0);
	requestQuit = false;
	substateCounter = 0;
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
	std::vector<Item*> items = level->getItems();
	for (std::vector<Item*>::iterator it=items.begin(); it<items.end(); it++)
	{
		drawItem(*it, offset, view);
	}
	// draw creatures
	std::vector<Creature*> creatures = level->getCreatures();
	for (std::vector<Creature*>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		drawCreature(*it, offset, view);
	}
}

void World::drawItem(Item* i, Point offset, Viewport view)
{
	Point pos = i->getPos();
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
	if (player->getState() == STATE_INVENTORY) drawInventory(substateCounter);
	if (player->getState() == STATE_PICKUP) drawItemList(substateCounter, "What do you want to pick up?", levels[currentLevel]->itemsAt(player->getCreature()->getPos()));
	if (player->getState() == STATE_WIELD)
	{
		std::vector<ITEM_TYPE> filter;
		filter.push_back(ITEM_WEAPON);
		drawItemList(substateCounter, "What do you want to wield?", player->getInventory(), filter);
	}
	drawMessage();
}

void World::toggleFullscreen()
{
	TCODConsole::root->setFullscreen(!TCODConsole::root->isFullscreen());
	drawWorld();
	TCODConsole::root->flush();
}

void World::drawItemList(int page, std::string title,std::vector<Item*> items)
{
	std::vector<std::pair<int, Item*> > mappedItems;
	int r = 0;
	for (std::vector<Item*>::iterator it=items.begin(); it<items.end(); it++)
	{
		mappedItems.push_back(std::pair<int, Item*>(r, *it));
		r++;
	}
	drawItemList(page, title, mappedItems);
}

void World::drawItemList(int page, std::string title, std::vector<std::pair<int, Item*> > items, std::vector<ITEM_TYPE> filter)
{
	std::vector<std::pair<int, Item*> > filteredItems;
	for (std::vector<std::pair<int, Item*> >::iterator it=items.begin(); it<items.end(); it++)
	{
		for (std::vector<ITEM_TYPE>::iterator filterIt=filter.begin(); filterIt<filter.end(); filterIt++)
		{
			if ((*it).second->getType() == (*filterIt))
			{
				filteredItems.push_back(*it);
				break;
			}
		}
	}
	drawItemList(page, title, filteredItems);
}

bool operator<(std::pair<int, Item*> a, std::pair<int, Item*> b)
{
	if (a.second->getType() < b.second->getType()) return true;
	if (a.second->getType() == b.second->getType() && a.first < b.first) return true;
	return false;
}

void World::drawItemList(int page, std::string title, std::vector<std::pair<int, Item*> > items)
{
	std::sort(items.begin(), items.end());
	TCODConsole* window[256];
	for (int i=0; i<256; i++) window[i] = NULL;
	window[0] = new TCODConsole(viewLevel.width - viewLevel.width / 4, viewLevel.height - viewLevel.height / 4);
	window[0]->printFrame(0, 0, window[0]->getWidth(), window[0]->getHeight(), true, TCOD_BKGND_DEFAULT, title.c_str());
	bool first[NUM_ITEM_TYPE];
	for (int i=0; i<NUM_ITEM_TYPE; i++) first[i] = true;
	int nline = 3;
	int p = 0;

	for (std::vector<std::pair<int, Item*> >::iterator it=items.begin(); it<items.end(); it++)
	{
		if (nline >= window[p]->getHeight() - 3)
		{
			nline = 3;
			p++;
			window[p] = new TCODConsole(viewLevel.width - viewLevel.width / 4, viewLevel.height - viewLevel.height / 4);
			window[p]->printFrame(0, 0, window[p]->getWidth(), window[p]->getHeight(), true, TCOD_BKGND_DEFAULT, title.c_str());
		}
		if (first[(*it).second->getType()])
		{
			window[p]->printEx(window[p]->getWidth() / 2, nline, TCOD_BKGND_DEFAULT, TCOD_CENTER, util::plural((*it).second->typeString()).c_str());
			nline += 2;
			first[(*it).second->getType()] = false;
		}
		std::stringstream ss;
		ss << util::letters[(*it).first] << " - " << (*it).second->toString();
		window[p]->printEx(4, nline, TCOD_BKGND_DEFAULT, TCOD_LEFT, ss.str().c_str());
		nline++;
	}

	page = page % (p+1);
	TCODConsole::blit(window[page], 0, 0, 0, 0, TCODConsole::root, viewLevel.x + viewLevel.width / 8, viewLevel.y + viewLevel.height / 8, 1.f, 0.9f);
	for (int i=0; i<256; i++)
	{
		if (window[i] != NULL)
		{
			delete window[i];
			window[i] = NULL;
		}
	}
}

void World::drawInventory(int page)
{
	drawItemList(page, "inventory", player->getInventory());
}