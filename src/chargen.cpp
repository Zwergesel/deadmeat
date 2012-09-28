#include <iostream>
#include <fstream>
#include <stdexcept>
#include "chargen.hpp"
#include "creature.hpp"
#include "world.hpp"
#include "utility.hpp"
#include "factory.hpp"

void CharGen::generate()
{
	// Choose class, race, gender
	PlayerClass c = CharGen::choose_class();
	PlayerRace r = CharGen::choose_race(c);
	Gender g = CharGen::choose_gender(c, r);
	std::string n = CharGen::choose_name(c, r, g);

	// If quit was requested during generation make sure the game will not get saved
	if (world.requestQuit) world.gameover = true;

	// Create a player creature
	world.player->setName(n);
	Creature* pc = new PlayerCreature(c,r,g);

	// Fill inventory
	std::vector<std::string> items = CharGen::generateStartItems(c, r, g);
	for (auto it=items.begin(); it!=items.end(); it++)
	{
		pc->addItem(factory.spawnItem(*it, true));
	}
	world.player->setCreature(pc);
	for (int i=0; i<NUM_ATTR; i++) world.player->setAttribute(static_cast<ATTRIBUTE>(i), startAttr[r][i]);
}

PlayerClass CharGen::choose_class()
{
	int selected = -1;
	while (!world.requestQuit && selected < 0)
	{
		CharGen::draw(NUM_CLASS, NUM_RACE, NUM_GENDER, "Nameless");
		char choice = CharGen::waitForChar();
		if (choice == 'r') selected = rng->getInt(0, NUM_CLASS - 1);
		if (choice >= 'a' && choice <= util::letters[NUM_CLASS - 1]) selected = util::letterToInt(choice);
	}
	return selected >= 0 ? static_cast<PlayerClass>(selected) : CLASS_WARRIOR;
}

PlayerRace CharGen::choose_race(PlayerClass c)
{
	int selected = -1;
	while (!world.requestQuit && selected < 0)
	{
		CharGen::draw(c, NUM_RACE, NUM_GENDER, "Nameless");
		char choice = CharGen::waitForChar();
		if (choice == 'r')
		{
			selected = rng->getInt(0, std::count(ClassRace[c], ClassRace[c]+NUM_RACE, true) - 1);
			for (int i = 0; i <= selected; i++) if (!ClassRace[c][i]) selected++;
		}
		if (choice >= 'a' && choice <= util::letters[NUM_CLASS - 1] && ClassRace[c][util::letterToInt(choice)])
		{
			selected = util::letterToInt(choice);
		}
	}
	return selected >= 0 ? static_cast<PlayerRace>(selected) : RACE_HUMAN;
}

Gender CharGen::choose_gender(PlayerClass c, PlayerRace r)
{
	int selected = -1;
	while (!world.requestQuit && selected < 0)
	{
		CharGen::draw(c, r, NUM_GENDER, "Nameless");
		char choice = CharGen::waitForChar();
		if (choice == 'r') selected = rng->getInt(0, NUM_GENDER - 1);
		if (choice >= 'a' && choice <= util::letters[NUM_GENDER - 1]) selected = util::letterToInt(choice);
	}
	return selected >= 0 ? static_cast<Gender>(selected) : GENDER_MALE;
}

std::string CharGen::choose_name(PlayerClass c, PlayerRace r, Gender g)
{
	std::string pn = "Nameless";
	TCOD_key_t key;
	bool first = true;
	while (!world.requestQuit)
	{
		CharGen::draw(c, r, g, pn);
		key = world.player->waitForKeypress(true);
		if (key.vk == TCODK_ENTER || key.vk == TCODK_KPENTER) break;
		if (key.vk == TCODK_BACKSPACE && pn.size() > 0)
		{
			pn.erase(pn.end() - 1);
			first = false;
		}
		else if (CharGen::isNameChar(key.c))
		{
			if (first) pn.clear();
			first = false;
			if (pn.size() < 15) pn.append(1, key.c);
		}
	}
	return pn == "" ? "Nameless" : pn;
}

void CharGen::draw(PlayerClass c, PlayerRace r, Gender g, const std::string& name)
{
	const Viewport& view = CharGen::view;
	TCODConsole window(view.width, view.height);
	int imgWidth, imgHeight;

	TCODConsole playerClass(view.width / 3, view.height * 3 / 4 );
	playerClass.printFrame(0, 0, playerClass.getWidth(), playerClass.getHeight(), true, TCOD_BKGND_DEFAULT, "Class");
	if (c == NUM_CLASS)
	{
		for (int i=0; i<NUM_CLASS; i++)
		{
			playerClass.printEx(2, 2 + 2*i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%c - %s", util::letters[i], CLASS_NAMES[i].c_str());
		}
		playerClass.printEx(2, playerClass.getHeight() - 3, TCOD_BKGND_DEFAULT, TCOD_LEFT, "r - random");
	}
	else
	{
		std::stringstream ss;
		ss << "assets/chargen/class-" << CLASS_NAMES[c] << ".png";
		TCODImage classImage(ss.str().c_str());
		classImage.setKeyColor(TCODColor::fuchsia);
		classImage.getSize(&imgWidth, &imgHeight);
		classImage.blitRect(&playerClass, playerClass.getWidth() / 2 - imgWidth / 2, 3);
		playerClass.printEx(playerClass.getWidth() / 2, playerClass.getHeight() - 5, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", CLASS_NAMES[c].c_str());
	}
	TCODConsole::blit(&playerClass, 0, 0, 0, 0, &window, 0, 0, 1.f, 1.f);

	TCODConsole playerRace(view.width / 3, view.height * 3 / 4 );
	playerRace.printFrame(0, 0, playerRace.getWidth(), playerRace.getHeight(), true, TCOD_BKGND_DEFAULT, "Race");
	if (c != NUM_CLASS && r == NUM_RACE)
	{
		for (int i=0; i<NUM_RACE; i++)
		{
			if (ClassRace[c][i])	playerRace.printEx(2, 2 + 2*i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%c - %s", util::letters[i], RACE_NAMES[i].c_str());
		}
		playerRace.printEx(2, playerRace.getHeight() - 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "r - random");
	}
	else if (r != NUM_RACE)
	{
		std::stringstream ss;
		ss << "assets/chargen/race-" << RACE_NAMES[r] << ".png";
		TCODImage raceImage(ss.str().c_str());
		raceImage.setKeyColor(TCODColor::fuchsia);
		raceImage.getSize(&imgWidth,&imgHeight);
		raceImage.blitRect(&playerRace, playerRace.getWidth() / 2 - imgWidth / 2, 3);
		playerRace.printEx(playerRace.getWidth() / 2, playerRace.getHeight() - 5, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", RACE_NAMES[r].c_str());
	}
	TCODConsole::blit(&playerRace, 0, 0, 0, 0, &window, playerClass.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerGender(view.width / 3, view.height * 3 / 4 );
	playerGender.printFrame(0, 0, playerGender.getWidth(), playerGender.getHeight(), true, TCOD_BKGND_DEFAULT, "Gender");
	if (r != NUM_RACE && g == NUM_GENDER)
	{
		for (int i=0; i<NUM_GENDER; i++)
		{
			playerGender.printEx(2, 2 + 2*i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%c - %s", util::letters[i], GENDER_NAMES[i].c_str());
		}
		playerGender.printEx(2, playerGender.getHeight() - 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "r - random");
	}
	else if (g != NUM_GENDER)
	{
		std::stringstream ss;
		ss << "assets/chargen/gender-" << GENDER_NAMES[g] << ".png";
		TCODImage genderImage(ss.str().c_str());
		genderImage.setKeyColor(TCODColor::fuchsia);
		genderImage.getSize(&imgWidth, &imgHeight);
		genderImage.blitRect(&playerGender, playerGender.getWidth() / 2 - imgWidth / 2, 3);
		playerGender.printEx(playerGender.getWidth() / 2, playerGender.getHeight() - 5, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", GENDER_NAMES[g].c_str());
	}
	TCODConsole::blit(&playerGender, 0, 0, 0, 0, &window, playerClass.getWidth() + playerRace.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerName(view.width, view.height - view.height * 3 / 4);
	playerName.printFrame(0, 0, playerName.getWidth(), playerName.getHeight(), true, TCOD_BKGND_DEFAULT, "Name");
	if (g != NUM_GENDER) playerName.printEx(20, playerName.getHeight() / 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "Name: %s", name.c_str());
	TCODConsole::blit(&playerName, 0, 0, 0, 0, &window, 0, playerClass.getHeight(), 1.f, 1.f);

	TCODConsole::root->clear();
	TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, view.x, view.y, 1.f, 0.9f);
	TCODConsole::root->flush();
}

char CharGen::waitForChar()
{
	TCOD_key_t key;
	while (!world.requestQuit)
	{
		key = world.player->waitForKeypress(true);
		if (key.c >= 'a' && key.c <= 'z') return key.c;
	}
	return 'a';
}

bool CharGen::isNameChar(char c)
{
	if (c >= 'a' && c <= 'z') return true;
	if (c >= 'A' && c <= 'Z') return true;
	if (c == ' ' || c == '.' || c == '-' || c == '\'') return true;
	if (c >= '0' && c <= '9') return true;
	return false;
}

std::vector<std::string> CharGen::generateStartItems(PlayerClass c, PlayerRace r, Gender g)
{
	std::vector<std::string> items;
	std::ifstream itemFile;
	itemFile.open("data/chargen.cfg");
	if (!itemFile.is_open()) return items;
	std::string line;
	bool discard = true;
	while (!itemFile.eof())
	{
		getline(itemFile,line);
		if (line.size() == 0) continue;
		if (line[0] == '#') continue;
		if (line[0] == '[')
		{
			size_t split = line.find_first_of(' ');
			size_t end = line.find_first_of(']');
			std::string type = line.substr(1, split-1);
			std::string compare = line.substr(split+1, end-split-1);
			if (type == "class" && compare == CharGen::CLASS_NAMES[c]) discard = false;
			else if (type == "race" && compare == CharGen::RACE_NAMES[r]) discard = false;
			else discard = true;
		}
		else if (!discard && factory.itemExists(line))
		{
			items.push_back(line);
		}
	}
	itemFile.close();
	return items;
}