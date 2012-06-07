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
	fovMap = NULL;
	currentLevel = 0;
	levelOffset = Point(0,0);
	requestQuit = false;
	gameover = false;
	time = 0;
	clearMessage = false;
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
	if (fovMap != NULL)
	{
		delete fovMap;
		fovMap = NULL;
	}
}

/* forceBreak is optional (default: false) */
void World::addMessage(std::string m, bool forceBreak)
{
	if (messageQueue.empty() || (messageQueue.size() == 1 && clearMessage))
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
	messageLog.push_back(m);
	while (messageLog.size() > 20) messageLog.pop_front();
}

int World::getNumMessages()
{
	return messageQueue.size();
}

void World::popMessage()
{
	if (!messageQueue.empty() && clearMessage)
	{
		messageQueue.pop_front();
	}
	clearMessage = false;
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

void World::drawMessageLog()
{
	std::stringstream log;
	for (auto it = messageLog.begin(); it != messageLog.end(); )
	{
		log << (*it);
		it++;
		if (it != messageLog.end()) log << std::endl << std::endl;
	}
	drawBlockingWindow("Message Log", log.str(), " ", TCODColor::black, false);
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
			if (fovMap->isInFov(x,y))
			{
				level->setSeen(Point(x,y), true);
				TCODConsole::root->putCharEx(
				  view.x + x + offset.x,
				  view.y + y + offset.y,
				  inf.sym,
				  inf.color,
				  inf.background
				);
			}
			else if (!fovMap->isInFov(x,y) && level->isSeen(Point(x,y)))
			{
				TCODColor seenColor = inf.color;
				TCODColor seenBackground = inf.background;
				seenColor.scaleHSV(0.5f, 0.5f);
				seenBackground.scaleHSV(0.5f, 0.5f);
				TCODConsole::root->putCharEx(
				  view.x + x + offset.x,
				  view.y + y + offset.y,
				  inf.sym,
				  seenColor,
				  seenBackground
				);
			}
		}
	}
	// draw items
	std::vector<std::pair<Point, Item*> > items = level->getItems();
	for (std::vector<std::pair<Point, Item*> >::iterator it=items.begin(); it<items.end(); it++)
	{
		drawItem((*it).second, (*it).first, offset, view);
	}
	// draw creatures
	std::vector<TimelineAction> creatures = level->getCreatures();
	for (std::vector<TimelineAction>::iterator it=creatures.begin(); it<creatures.end(); it++)
	{
		drawCreature(it->actor, offset, view);
	}
}

void World::drawItem(Item* i, Point pos, Point offset, Viewport view)
{
	if (!fovMap->isInFov(pos.x, pos.y)) return;
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
	if (!fovMap->isInFov(pos.x, pos.y)) return;
	pos += offset;
	if (pos.x >= 0 && pos.x < view.width && pos.y >= 0 && pos.y < view.height)
	{
		TCODConsole::root->putChar(view.x + pos.x, view.y + pos.y, c->getSymbol());
		TCODConsole::root->setCharForeground(view.x + pos.x, view.y + pos.y, c->getColor());
	}
}

void World::drawWorld()
{
	// fov
	fovMap->computeFov(player->getCreature()->getPos().x, player->getCreature()->getPos().y, 0, true, FOV_BASIC);
	TCODConsole::root->clear();
	drawLevel(levels[currentLevel], levelOffset, viewLevel);
	STATE state = player->getState();
	if (state == STATE_INVENTORY || state == STATE_PICKUP || state == STATE_WIELD || state == STATE_WEAR || state == STATE_DROP || state == STATE_TAKEOFF)
	{
		drawItemSelection(world.itemSelection);
	}
	drawMessage();
	drawInfo();
}

void World::drawInfo()
{
	std::pair<int,int> health = player->getCreature()->getHealth();
	// Healthpoints color
	float coef = 1.0f * health.first / health.second;
	TCODColor hcol = TCODColor::red;
	if (coef >= 0.0f && coef < 0.5f) hcol = TCODColor::lerp(TCODColor::red, TCODColor::yellow, 2.0f * coef);
	else if (coef >= 0.5 && coef <= 1.0f) hcol = TCODColor::lerp(TCODColor::yellow, TCODColor::green, 2.0f * coef - 1.0f);
	else if (coef > 1.0f) hcol = TCODColor::green;
	// Health points
	TCODConsole::root->setColorControl(TCOD_COLCTRL_1, hcol, TCODColor::black);
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y, TCOD_BKGND_NONE, TCOD_LEFT, "HP");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y, TCOD_BKGND_NONE, TCOD_RIGHT, "%c%d/%d%c", TCOD_COLCTRL_1, health.first, health.second, TCOD_COLCTRL_STOP);
	// Mana points
	TCODConsole::root->setColorControl(TCOD_COLCTRL_2, TCODColor(30,180,240), TCODColor::black);
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 2, TCOD_BKGND_NONE, TCOD_LEFT, "MP");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 2, TCOD_BKGND_NONE, TCOD_RIGHT, "%c0/0%c", TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);
	// Attributes
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 5, TCOD_BKGND_NONE, TCOD_LEFT, "STR %2d DEX %2d", player->getAttribute(ATTR_STR), player->getAttribute(ATTR_DEX));
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 7, TCOD_BKGND_NONE, TCOD_LEFT, "CON %2d INT %2d", player->getAttribute(ATTR_CON), player->getAttribute(ATTR_INT));
	// Attack value and Armor Class
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 10, TCOD_BKGND_NONE, TCOD_LEFT, "AT %3d AC %3d", player->getCreature()->getAttack(), player->getCreature()->getDefense());
	// Time
	int sec = (time/10) % 60;
	int min = (time/600) % 60;
	int hour = (time/36000) % 24;
	int days = (time/864000);
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 13, TCOD_BKGND_NONE, TCOD_LEFT, "TIME PASSED");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 15, TCOD_BKGND_NONE, TCOD_RIGHT, "%d DAY%s   ", days, days == 1 ? " " : "S");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 16, TCOD_BKGND_NONE, TCOD_RIGHT, "%d HOUR%s  ", hour, hour == 1 ? " " : "S");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 17, TCOD_BKGND_NONE, TCOD_RIGHT, "%d MINUTE%s", min, min == 1 ? " " : "S");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 18, TCOD_BKGND_NONE, TCOD_RIGHT, "%d SECOND%s", sec, sec == 1 ? " " : "S");
	// Score
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 21, TCOD_BKGND_NONE, TCOD_LEFT, "SCORE");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 22, TCOD_BKGND_NONE, TCOD_RIGHT, "14324");
}

void World::toggleFullscreen()
{
	TCODConsole::root->setFullscreen(!TCODConsole::root->isFullscreen());
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

unsigned char World::drawBlockingWindow(const std::string& title, const std::string& text, const std::string& acceptedKeys, TCODColor color, bool center)
{
	int w = viewItemList.width;
	int h = 4 + TCODConsole::root->getHeightRect(0,0, w - 4, 100, text.c_str());
	TCODConsole window(w, h);
	window.setDefaultBackground(color);
	window.printFrame(0, 0, window.getWidth(), window.getHeight(), true, TCOD_BKGND_DEFAULT, title.c_str());
	if (center)
	{
		window.printRectEx(window.getWidth()/2, 2, w - 4, h - 4, TCOD_BKGND_DEFAULT, TCOD_CENTER, text.c_str());
	}
	else
	{
		window.printRectEx(2, 2, w - 4, h - 4, TCOD_BKGND_DEFAULT, TCOD_LEFT, text.c_str());	
	}
	TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, viewItemList.x, viewLevel.y + viewLevel.height/2 - h/2, 1.f, 0.9f);
	TCODConsole::root->flush();
	TCOD_key_t key;
	do
	{
		key = player->waitForKeypress(true);
		if (acceptedKeys.find_first_of(key.c) != std::string::npos) return key.c;
	}
	while (!world.requestQuit && key.vk != TCODK_ESCAPE);
	return '\0';
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int World::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("World", id);
	store ("currentLevel", currentLevel) ("levelOffset", levelOffset) ("time", time);
	// TODO : all levels
	store.ptr("player", player->save(sg)) ("#levels", 1) .ptr("_level", levels[0]->save(sg));
	sg << store;
	return id;
}

void World::load(LoadBlock& load)
{
	load ("currentLevel", currentLevel) ("levelOffset", levelOffset) ("time", time);
	// TODO: delete player and levels [MEMORY LEAK]
	player = static_cast<Player*>(load.ptr("player"));
	int n;
	load ("#levels", n);
	for (int i=0; i<n; i++)
	{
		levels[i] = static_cast<Level*>(load.ptr("_level"));
	}
}
