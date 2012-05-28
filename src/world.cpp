#include "world.hpp"
#include <iostream>

World::World()
{
	player = new Player("Mustermann");
	tileSet = new TileSet();
	levels = new Level*[10];
	currentLevel = 0;
	levelOffset = Point(0,0);
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

void World::addMessage(std::string m)
{
	if (messageQueue.empty())
	{
		messageQueue.push_back(m);
	}
	else
	{
		std::string combine = messageQueue.back() + " " + m;
		int expectedHeight = TCODConsole::root->getHeightRect(
			viewMsg.x, viewMsg.y, viewMsg.width, viewMsg.height,
			"%s <More>", combine.c_str()
		);
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
	int startX = (offset.x < 0) ? -offset.x : 0;
	int startY = (offset.y < 0) ? -offset.y : 0;
	int rangeX = std::min(level->getWidth(), view.width - offset.x);
	int rangeY = std::min(level->getHeight(), view.height - offset.y);
	for (int y=startY; y<rangeY; y++) {
		for (int x=startX; x<rangeX; x++) {
			TileInfo inf = tileSet->getInfo(level->getTile(x,y));
			TCODConsole::root->putCharEx(
				view.x + x + offset.x,
				view.y + y + offset.y,
				inf.symbol,
				inf.color,
				inf.background
			);
		}
	}
}

void World::drawCreature(Creature* c, Point offset, Viewport view)
{
	int drawX = c->getX() + offset.x;
	int drawY = c->getY() + offset.y;
	if (drawX >= 0 && drawX < view.width && drawY >= 0 && drawY < view.height)
	{
		TCODConsole::root->putChar(view.x + drawX, view.y + drawY, c->getSymbol());
		TCODConsole::root->setCharForeground(view.x + drawX, view.y + drawY, c->getColor());
	}
}

void World::drawPlayer(Player* p, Point offset, Viewport view)
{
	int drawX = p->getX() + offset.x;
	int drawY = p->getY() + offset.y;
	if (drawX >= 0 && drawX < view.width && drawY >= 0 && drawY < view.height)
	{
		TCODConsole::root->putChar(view.x + drawX, view.y + drawY, p->getSymbol());
		TCODConsole::root->setCharForeground(view.x + drawX, view.y + drawY, p->getColor());
	}
}

void World::debugDrawWorld(Goblin* g, FailWhale* w)
{		
	TCODConsole::root->clear();
	drawLevel(levels[currentLevel], levelOffset, viewLevel);
	drawCreature(g, levelOffset, viewLevel);
	drawCreature(w, levelOffset, viewLevel);
	drawMessage();
	drawPlayer(player, levelOffset, viewLevel); 
}