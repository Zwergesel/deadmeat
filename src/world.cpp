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
#include "chargen.hpp"

World::World()
{
	player = new Player("PlayerName");
	tileSet = new TileSet();
	levels = new Level*[256];
	for (int i=0; i<256; i++) levels[i] = NULL;
	fovMap = NULL;
	currentLevel = 0;
	levelOffset = Point(0,0);
	requestQuit = false;
	gameover = false;
	time = 0;
	clearMessage = false;
	deathReason = "";
	debug_fov = 0;
	debug_god = 0;
}

World::~World()
{
	clearWorld();
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

void World::clearWorld()
{
	if (player != NULL)
	{
		delete player;
		player = NULL;
	}
	if (fovMap != NULL)
	{
		delete fovMap;
		fovMap = NULL;
	}
	for (int i=0; i<256; i++)
	{
		if (levels[i] != NULL)
		{
			delete levels[i];
			levels[i] = NULL;
		}
	}
	worldNodes.clear();
	messageLog.clear();
	cleanGarbage();
}

void World::newGame()
{
	// Clean up old pointers + memory
	clearWorld();

	// Reset all other values
	currentLevel = 0;
	time = 0;
	gameover = false;
	clearMessage = false;
	deathReason = "";
	player = new Player("Richard P. Enus");

	// Generate a new character and world
	CharGen::generate();
	LevelGen::generateWorld();
	levels[0] = LevelGen::generateLevel(0, LEVELTYPE_FOREST);

	// Add player creature
	Point newPos = levels[0]->getRandomLocation(WALKABLE);
	player->getCreature()->setPos(newPos);
	levels[0]->addCreature(player->getCreature(), 0);
	levelOffset.x = util::clamp(viewLevel.width/2 - newPos.x, viewLevel.width - levels[0]->getWidth(), 0);
	levelOffset.y = util::clamp(viewLevel.height/2 - newPos.y, viewLevel.height - levels[0]->getHeight(), 0);
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
	// TODO: colors?
	if (!messageQueue.empty())
	{
		TCODConsole::root->printRect(
		  viewMsg.x, viewMsg.y, viewMsg.width, viewMsg.height, messageQueue.front().c_str()
		);
	}
}

void World::drawMessageLog()
{
	std::string log;
	for (auto it = messageLog.begin(); it != messageLog.end(); )
	{
		log += (*it);
		int h = TCODConsole::root->getHeightRect(0, 0, viewItemList.width - 4, 100, log.c_str());
		if (h > 40)
		{
			log = log.substr(0, log.length() - it->length() - 2);
			break;
		}
		it++;
		if (it != messageLog.end()) log += "\n\n";
	}
	drawBlockingWindow("Message Log", log, " ", TCODColor::black, false);
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
			// check for object
			Object* obj = level->objectAt(Point(x,y));
			if (obj != NULL && obj->isVisible())
			{
				inf.color = obj->getColor();
				inf.sym = obj->getSymbol();
			}
			if (player->isPositionVisible(Point(x,y)))
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
			else if (level->isSeen(Point(x,y)))
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
	if (!player->isPositionVisible(pos)) return;
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
	if (!player->isPositionVisible(pos)) return;
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
	buildFovMap();
	fovMap->computeFov(player->getCreature()->getPos().x, player->getCreature()->getPos().y, 0, true, FOV_BASIC);

	if (debug_fov)
	{
		for (int y=0; y<fovMap->getHeight(); y++)
			for (int x=0; x<fovMap->getWidth(); x++)
				fovMap->setInFov(x,y,true);
	}

	TCODConsole::root->clear();
	drawLevel(levels[currentLevel], levelOffset, viewLevel);
	STATE state = player->getState();
	if (state == STATE_INVENTORY || state == STATE_PICKUP || state == STATE_WIELD || state == STATE_WEAR
	    || state == STATE_DROP || state == STATE_TAKEOFF || state == STATE_EAT || state == STATE_QUIVER
	    || state == STATE_USE || state == STATE_DRINK || state == STATE_READ)
	{
		drawItemSelection(world.itemSelection);
	}
	else if (state == STATE_INSPECT)
	{
		drawCursor(player->getCursor(), levelOffset, false);
	}
	else if (state == STATE_RANGED_ATTACK)
	{
		drawCursor(player->getCursor(), levelOffset, true);
	}
	else if (state == STATE_CAST_TARGET)
	{
		drawCursor(player->getCursor(), levelOffset, g_spells[world.player->getSelectedSpell()].isProjectile());
	}
	else if (state == STATE_CHARINFO)
	{
		drawCharInfo();
	}
	drawMessage();
	drawInfo();
}

void World::drawCharInfo()
{
	PlayerCreature* creature = static_cast<PlayerCreature*>(player->getCreature());

	TCODConsole frame(viewLevel.width, viewLevel.height);

	TCODConsole charInfo(viewLevel.width / 2, 15);
	charInfo.printFrame(0, 0, charInfo.getWidth(), charInfo.getHeight(), true, TCOD_BKGND_DEFAULT, "Character Information");
	charInfo.printEx(2, 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Name:   %s", player->getName().c_str());
	charInfo.printEx(2, 4, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Race:   %s", CharGen::RACE_NAMES[creature->getRace()].c_str());
	charInfo.printEx(2, 6, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Class:  %s", CharGen::CLASS_NAMES[creature->getClass()].c_str());
	charInfo.printEx(2, 8, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Gender: %s", CharGen::GENDER_NAMES[creature->getGender()].c_str());
	TCODConsole::blit(&charInfo, 0, 0, 0, 0, &frame, 0, 0, 1.f, 1.f);

	TCODConsole attributeInfo((viewLevel.width + 3) / 2, 15);
	attributeInfo.printFrame(0, 0, attributeInfo.getWidth(), attributeInfo.getHeight(), true, TCOD_BKGND_DEFAULT, "Attributes");
	attributeInfo.printEx(8, 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Strength        %2d", player->getAttribute(ATTR_STR));
	attributeInfo.printEx(8, 4, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Dexterity       %2d", player->getAttribute(ATTR_DEX));
	attributeInfo.printEx(8, 6, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Constitution    %2d", player->getAttribute(ATTR_CON));
	attributeInfo.printEx(8, 8, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Intelligence    %2d", player->getAttribute(ATTR_INT));
	if (player->getAttributePoints() > 0)
	{
		attributeInfo.printEx(2, 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "S - ");
		attributeInfo.printEx(2, 4, TCOD_BKGND_DEFAULT, TCOD_LEFT, "D - ");
		attributeInfo.printEx(2, 6, TCOD_BKGND_DEFAULT, TCOD_LEFT, "C - ");
		attributeInfo.printEx(2, 8, TCOD_BKGND_DEFAULT, TCOD_LEFT, "I - ");
		attributeInfo.printEx(attributeInfo.getWidth()-3, attributeInfo.getHeight() - 3, TCOD_BKGND_DEFAULT, TCOD_RIGHT, "Points left = %-2d", player->getAttributePoints());
	}
	TCODConsole::blit(&attributeInfo, 0, 0, 0, 0, &frame, charInfo.getWidth() - 1, 0, 1.f, 1.f);

	TCODConsole skillInfo(frame.getWidth(), frame.getHeight() - charInfo.getHeight() + 1);
	skillInfo.printFrame(0, 0, skillInfo.getWidth(), skillInfo.getHeight(), true, TCOD_BKGND_DEFAULT, "Skills");
	skillInfo.printEx(8, 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "SKILL         VALUE  PROGRESS      REQUIREMENT");
	for (int i=0; i<NUM_SKILL; i++)
	{
		Skill sk = player->getSkill(static_cast<SKILLS>(i));
		std::vector<std::pair<int,int>> r = sk.req[sk.maxValue+1];
		std::string attrShort[4] = { "STR", "DEX", "CON", "INT" };
		bool allowTrain = sk.maxValue < sk.maxLevel;

		for (unsigned int d=0; d<r.size(); d++)
		{
			int attr = player->getAttribute(static_cast<ATTRIBUTE>(r[d].first));
			int val = r[d].second;
			if (attr < val) allowTrain = false;
			skillInfo.setColorControl(TCOD_COLCTRL_1, (attr < val ? TCODColor::red : TCODColor::green), TCODColor::black);
			skillInfo.printEx(44+d*7, 4+i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%c%s %-2d%c",
			                  TCOD_COLCTRL_1, attrShort[r[d].first].c_str(), val, TCOD_COLCTRL_STOP);
		}

		if (player->getSkillPoints() > 0 && allowTrain)
		{
			skillInfo.printEx(2, 4 + i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%c - ", util::letters[i]);
		}

		skillInfo.printEx(8, 4 + i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%-15s %2d", sk.name.c_str(), sk.value);

		if (sk.value < sk.maxValue)
		{
			int currentExp = sk.exp - Skill::expNeeded(sk.value - 1);
			int neededExp = Skill::expNeeded(sk.value) - Skill::expNeeded(sk.value - 1);
			int p = static_cast<int>(10.0 * currentExp / neededExp);
			for (int j=0; j<10; j++)
			{
				skillInfo.putCharEx(28+j, 4+i, 219, j < p ? TCODColor::green : TCODColor::white, TCODColor::black);
			}
			skillInfo.printEx(40, 4 + i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%-2d", sk.maxValue);
		}
	}
	if (player->getSkillPoints() > 0) skillInfo.printEx(skillInfo.getWidth()-3, skillInfo.getHeight()-3, TCOD_BKGND_DEFAULT, TCOD_RIGHT, "Points left = %-2d", player->getSkillPoints());

	TCODConsole::blit(&skillInfo, 0, 0, 0, 0, &frame, 0, charInfo.getHeight() - 1, 1.f, 1.f);

	// Correct corners
	frame.putChar(charInfo.getWidth()-1, 0, TCOD_CHAR_TEES);
	frame.putChar(0, charInfo.getHeight()-1, TCOD_CHAR_TEEE);
	frame.putChar(frame.getWidth()-1, attributeInfo.getHeight()-1, TCOD_CHAR_TEEW);

	TCODConsole::blit(&frame, 0, 0, 0, 0, TCODConsole::root, viewLevel.x, viewLevel.y, 1.f, 0.9f);
	TCODConsole::root->flush();
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
	std::pair<int,int> mana = player->getCreature()->getMana();
	TCODConsole::root->setColorControl(TCOD_COLCTRL_2, TCODColor(30,180,240), TCODColor::black);
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 2, TCOD_BKGND_NONE, TCOD_LEFT, "MP");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 2, TCOD_BKGND_NONE, TCOD_RIGHT, "%c%d/%d%c", TCOD_COLCTRL_2, mana.first, mana.second, TCOD_COLCTRL_STOP);
	// Attributes
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 5, TCOD_BKGND_NONE, TCOD_LEFT, "STR %2d DEX %2d", player->getAttribute(ATTR_STR), player->getAttribute(ATTR_DEX));
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 7, TCOD_BKGND_NONE, TCOD_LEFT, "CON %2d INT %2d", player->getAttribute(ATTR_CON), player->getAttribute(ATTR_INT));
	// Attack value and Armor Class
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 10, TCOD_BKGND_NONE, TCOD_LEFT, "AT %3d AC %3d", player->getCreature()->getAttack(), player->getCreature()->getDefense());
	// XP and level
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 13, TCOD_BKGND_NONE, TCOD_LEFT, "Level");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 13, TCOD_BKGND_NONE, TCOD_RIGHT, "%d", player->getLevel());
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 14, TCOD_BKGND_NONE, TCOD_LEFT, "Experience");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 15, TCOD_BKGND_NONE, TCOD_RIGHT, "%d/%d", player->getExperience(), player->getNeededExp());
	// Time
	int sec = (time/10) % 60;
	int min = (time/600) % 60;
	int hour = (time/36000) % 24;
	int days = (time/864000);
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 18, TCOD_BKGND_NONE, TCOD_LEFT, "TIME PASSED");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 20, TCOD_BKGND_NONE, TCOD_RIGHT, "%d DAY%s   ", days, days == 1 ? " " : "S");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 21, TCOD_BKGND_NONE, TCOD_RIGHT, "%d HOUR%s  ", hour, hour == 1 ? " " : "S");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 22, TCOD_BKGND_NONE, TCOD_RIGHT, "%d MINUTE%s", min, min == 1 ? " " : "S");
	TCODConsole::root->printEx(viewInfo.x + viewInfo.width - 1, viewInfo.y + 23, TCOD_BKGND_NONE, TCOD_RIGHT, "%d SECOND%s", sec, sec == 1 ? " " : "S");
	// Status
	TCODConsole::root->setColorControl(TCOD_COLCTRL_3, TCODColor::orange, TCODColor::black);
	TCODConsole::root->setColorControl(TCOD_COLCTRL_4, TCODColor::red, TCODColor::black);
	TCODConsole::root->setColorControl(TCOD_COLCTRL_5, TCODColor::yellow, TCODColor::black);
	TCODConsole::root->printEx(viewInfo.x, viewInfo.y + 26, TCOD_BKGND_NONE, TCOD_LEFT, "STATUS");
	int row = viewInfo.y + 28;
	if (world.player->getNutrition() < HUNGER_WEAK)
	{
		TCODConsole::root->printEx(viewInfo.x + 2, row++, TCOD_BKGND_NONE, TCOD_LEFT, "%cWeak%c", TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
	}
	else if (world.player->getNutrition() < HUNGER_HUNGRY)
	{
		TCODConsole::root->printEx(viewInfo.x + 2, row++, TCOD_BKGND_NONE, TCOD_LEFT, "%cHungry%c", TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
	}
	else if (world.player->getNutrition() >= HUNGER_NORMAL)
	{
		TCODConsole::root->printEx(viewInfo.x + 2, row++, TCOD_BKGND_NONE, TCOD_LEFT, "%cSatiated%c", TCOD_COLCTRL_5, TCOD_COLCTRL_STOP);
	}
	if (world.player->getWeight() > BURDEN_NORMAL && world.player->getWeight() <= BURDEN_BURDENED)
	{
		TCODConsole::root->setColorControl(TCOD_COLCTRL_1, TCODColor::lightTurquoise, TCODColor::black);
		TCODConsole::root->printEx(viewInfo.x + 2, row++, TCOD_BKGND_NONE, TCOD_LEFT, "%cBurdened%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
	}
	if (world.player->getWeight() > BURDEN_BURDENED)
	{
		TCODConsole::root->setColorControl(TCOD_COLCTRL_1, TCODColor::darkTurquoise, TCODColor::black);
		TCODConsole::root->printEx(viewInfo.x + 2, row++, TCOD_BKGND_NONE, TCOD_LEFT, "%cOverloaded%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
	}
	if (world.player->getCreature()->getStatusStrength(STATUS_FIRE) > 0)
	{
		TCODConsole::root->printEx(viewInfo.x + 2, row++, TCOD_BKGND_NONE, TCOD_LEFT, "%cBurning%c", TCOD_COLCTRL_4, TCOD_COLCTRL_STOP);
	}
	if (world.player->getCreature()->getStatusStrength(STATUS_POISON) > 0)
	{
		TCODConsole::root->setColorControl(TCOD_COLCTRL_1, TCODColor::darkGreen, TCODColor::black);
		TCODConsole::root->printEx(viewInfo.x + 2, row++, TCOD_BKGND_NONE, TCOD_LEFT, "%cPoisoned%c", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
	}
}

void World::drawCursor(Point target, Point levelOffset, bool traceLine)
{
	target += levelOffset;
	if (traceLine)
	{
		Point origin = player->getCreature()->getPos() + levelOffset;
		Point current;
		TCODLine::init(origin.x, origin.y, target.x, target.y);
		while (!TCODLine::step(&current.x, &current.y) && (current.x != target.x || current.y != target.y))
		{
			TCODConsole::root->setCharBackground(viewLevel.x + current.x, viewLevel.y + current.y, TCODColor::yellow, TCOD_BKGND_ALPHA(0.4));
		}
	}
	TCODConsole::root->setCharBackground(viewLevel.x + target.x, viewLevel.y + target.y, TCODColor::yellow, TCOD_BKGND_ALPHA(0.7));
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


void World::travel()
{
	Point ppos = player->getCreature()->getPos();
	// find the right entrance
	for (auto it = worldNodes[currentLevel].link.begin(); it<worldNodes[currentLevel].link.end(); it++)
	{
		if ((*it).pos == ppos)
		{
			levels[currentLevel]->removeCreature(player->getCreature(), false);
			// set new level and recompute whats needed
			currentLevel = (*it).to;
			while (levels[currentLevel] == NULL) levels[currentLevel] = LevelGen::generateLevel(currentLevel, worldNodes[currentLevel].type);

			// reset timeline
			player->getCreature()->setPos(worldNodes[currentLevel].link[(*it).exitId].pos);
			levels[currentLevel]->buildTimeline(world.time);
			levels[currentLevel]->addCreature(world.player->getCreature(), world.time + 10); //TODO: why 10?

			buildFovMap();

			// TODO: redundant?
			world.levelOffset.x = util::clamp(world.viewLevel.width/2 - player->getCreature()->getPos().x, world.viewLevel.width - world.levels[currentLevel]->getWidth(), 0);
			world.levelOffset.y = util::clamp(world.viewLevel.height/2 - player->getCreature()->getPos().y, world.viewLevel.height - world.levels[currentLevel]->getHeight(), 0);
			return;
		}
	}
}

void World::buildFovMap()
{
	if (fovMap != NULL) delete fovMap;
	fovMap = new TCODMap(levels[currentLevel]->getWidth(), levels[currentLevel]->getHeight());
	Level* level = levels[currentLevel];
	for (int x = 0; x < level->getWidth(); x++)
		for (int y = 0; y < level->getHeight(); y++)
		{
			world.fovMap->setProperties(x,y,level->isTransparent(Point(x,y)),level->isWalkable(Point(x,y)));
		}
}

void World::cleanGarbage()
{
	while (!garbage.empty())
	{
		Creature* current = garbage.front();
		garbage.pop_front();
		if (current != NULL) delete current;
	}
}

void World::debugInput(std::string in)
{
	if (in.compare("fov") == 0) debug_fov = (debug_fov+1)%2;
	if (in.compare("god") == 0) debug_god = (debug_god+1)%2;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int World::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this,&id)) return id;
	SaveBlock store("World", id);
	store ("currentLevel", currentLevel) ("levelOffset", levelOffset) ("time", time);
	store.ptr("player", player->save(sg)) ("#levels", 256);
	for (int i=0; i<256; i++)
	{
		store.ptr("_level", levels[i] == NULL ? 0 : levels[i]->save(sg));
	}
	store ("#worldNodes", (unsigned int) worldNodes.size());
	for (unsigned int d=0; d<worldNodes.size(); d++)
	{
		store ("_type", worldNodes[d].type) ("_#links", (unsigned int) worldNodes[d].link.size());
		for (unsigned int e=0; e<worldNodes[d].link.size(); e++)
		{
			store("__type", worldNodes[d].link[e].type);
			store("__pos", worldNodes[d].link[e].pos);
			store("__to", worldNodes[d].link[e].to);
			store("__entranceId", worldNodes[d].link[e].entranceId);
			store("__exitId", worldNodes[d].link[e].exitId);
		}
	}
	store ("#messageLog", (int) messageLog.size());
	for (auto it = messageLog.begin(); it != messageLog.end(); it++)
	{
		store ("_log", *it);
	}
	sg << store;
	return id;
}

void World::load(LoadBlock& load)
{
	clearWorld();

	// Load
	load ("currentLevel", currentLevel) ("levelOffset", levelOffset) ("time", time);
	player = static_cast<Player*>(load.ptr("player"));
	int n;
	load ("#levels", n);
	for (int i=0; i<n; i++)
	{
		levels[i] = static_cast<Level*>(load.ptr("_level"));
	}
	load ("#worldNodes", n);
	while (n-->0)
	{
		int m,l;
		worldNodes.push_back(WorldNode());
		load ("_type", l) ("_#links", m);
		if (l < 0 || l >= NUM_LEVELTYPE) throw SavegameFormatException("World::load _ leveltype out of range");
		worldNodes.back().type = static_cast<LEVELTYPE>(l);
		while (m-->0)
		{
			int t;
			worldNodes.back().link.push_back(WorldLink());
			load ("__type", t) ("__pos", worldNodes.back().link.back().pos);
			load ("__to", worldNodes.back().link.back().to) ("__entranceId", worldNodes.back().link.back().entranceId);
			load ("__exitId", worldNodes.back().link.back().exitId);
			if (t < 0 || t >= TILES_LENGTH) throw SavegameFormatException("World::load _ tile out of range");
			worldNodes.back().link.back().type = static_cast<OBJECTTYPE>(t);
		}
	}
	load ("#messageLog", n);
	while (n-->0)
	{
		std::string m;
		load ("_log", m);
		messageLog.push_back(m);
	}
	fovMap = NULL;
	buildFovMap();
}