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
	if (player->isInventoryOpen()) drawInventory();
	drawMessage();
}

void World::toggleFullscreen()
{
	TCODConsole::root->setFullscreen(!TCODConsole::root->isFullscreen());
	drawWorld();
	TCODConsole::root->flush();
}
#include <typeinfo>
void World::drawInventory()
{
	TCODConsole inv(viewLevel.width - viewLevel.width / 4, viewLevel.height - viewLevel.height / 4);
	inv.printFrame(0, 0, inv.getWidth(), inv.getHeight(), true, TCOD_BKGND_DEFAULT, "inventory");
	Item** inventory = player->getInventory();
	assert(inventory != NULL);
	int nline = 3;
	// list weapons
	bool firstWeapon = true;
	for (int i='A'; i<='z'; i++)
	{
		if (inventory[i] != NULL && typeid(*(inventory[i])).name() == typeid(Weapon).name())
//   Weapon* w = dynamic_cast<Weapon*>(inventory[i]);
//   if(w != NULL)
		{
			Weapon* w = static_cast<Weapon*>(inventory[i]);
			if (firstWeapon)
			{
				inv.printEx(inv.getWidth() / 2, nline, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Weapons");
				nline += 2;
				firstWeapon = false;
			}
			std::stringstream ss;
			ss << "  " << static_cast<unsigned char>(i) << " - " << w->getName() << " " << w->getEnchantment();
			inv.printEx(4, nline, TCOD_BKGND_DEFAULT, TCOD_LEFT, ss.str().c_str());
			nline++;
		}
	}
	// list items
	bool firstItem = true;
	for (int i='A'; i<='z'; i++)
	{
		Item* item = inventory[i];
		if (item != NULL)
		{
			if (firstItem)
			{
				inv.printEx(inv.getWidth() / 2, nline, TCOD_BKGND_DEFAULT, TCOD_CENTER, "Items");
				nline += 2;
				firstItem = false;
			}
			std::stringstream ss;
			ss << "  " << static_cast<unsigned char>(i) << " - " << item->getName();
			inv.printEx(4, nline, TCOD_BKGND_DEFAULT, TCOD_LEFT, ss.str().c_str());
			nline++;
		}
	}
	TCODConsole::blit(&inv, 0, 0, 0, 0, TCODConsole::root, viewLevel.x + viewLevel.width / 8, viewLevel.y + viewLevel.height / 8, 1.f, 0.9f);
}