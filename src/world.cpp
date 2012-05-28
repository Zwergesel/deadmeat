#include "world.hpp"

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

int World::drawMessages(std::deque<std::string>* msg, Viewport view)
{
	// TODO: implement: draw as many messages as fit in the viewport
	//       with proper line breaks; modifiy deque and return number
	//       of messages drawn
  return 0;
}

void World::debugDrawWorld(Goblin* g, FailWhale* w)
{
	Viewport vpMap(1, 1, TCODConsole::root->getWidth() - 10, TCODConsole::root->getHeight() - 4);
	
	TCODConsole::root->clear();
	drawLevel(levels[currentLevel], levelOffset, vpMap);
	drawCreature(g, levelOffset, vpMap);
	drawCreature(w, levelOffset, vpMap);
	drawPlayer(player, levelOffset, vpMap); 
}