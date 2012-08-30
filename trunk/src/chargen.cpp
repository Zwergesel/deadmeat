#include "chargen.hpp"
#include "creature.hpp"
#include "world.hpp"
#include "utility.hpp"

void CharGen::generate()
{
	PlayerClass c = CharGen::choose_class();
	PlayerRace r = CharGen::choose_race(c);
	Gender g = CharGen::choose_gender(c, r);
	std::string n = CharGen::choose_name(c, r, g);
	world.player->setName(n);
	world.player->setCreature(new PlayerCreature(c,r,g));
}

PlayerClass CharGen::choose_class()
{
	PlayerClass pc = CLASS_WARRIOR;
	TCODConsole window(world.viewLevel.width, world.viewLevel.height);

	TCODConsole playerClass(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerClass.printFrame(0, 0, playerClass.getWidth(), playerClass.getHeight(), true, TCOD_BKGND_DEFAULT, "Class");
	for (int i=0; i<NUM_CLASS; i++)
	{
		playerClass.printEx(2, 2 + 2*i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%c - %s", util::letters[i], CLASS_NAMES[i].c_str());
	}
	playerClass.printEx(2, playerClass.getHeight() - 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "r - random");
	TCODConsole::blit(&playerClass, 0, 0, 0, 0, &window, 0, 0, 1.f, 1.f);

	TCODConsole playerRace(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerRace.printFrame(0, 0, playerRace.getWidth(), playerRace.getHeight(), true, TCOD_BKGND_DEFAULT, "Race");

	TCODConsole::blit(&playerRace, 0, 0, 0, 0, &window, playerClass.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerGender(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerGender.printFrame(0, 0, playerGender.getWidth(), playerGender.getHeight(), true, TCOD_BKGND_DEFAULT, "Gender");

	TCODConsole::blit(&playerGender, 0, 0, 0, 0, &window, playerClass.getWidth() + playerRace.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerName((world.viewLevel.width / 3) * 3, (world.viewLevel.height / 4));
	playerName.printFrame(0, 0, playerName.getWidth(), playerName.getHeight(), true, TCOD_BKGND_DEFAULT, "Name");

	TCODConsole::blit(&playerName, 0, 0, 0, 0, &window, 0, playerClass.getHeight(), 1.f, 1.f);

	TCODConsole::root->clear();
	TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, world.viewLevel.x, world.viewLevel.y, 1.f, 0.9f);
	TCODConsole::root->flush();
	TCOD_key_t key;
	do
	{
		key = world.player->waitForKeypress(true);
		if (key.c == 'r')
		{
			pc = static_cast<PlayerClass>(rng->getInt(0, NUM_CLASS - 1));
			break;
		}
		if (key.c >= 'a' && key.c < util::letters[NUM_CLASS])
		{
			pc = static_cast<PlayerClass>(util::letterToInt(key.c));
			break;
		}
	}
	while (!world.requestQuit);
	return pc;
}

PlayerRace CharGen::choose_race(PlayerClass c)
{
	PlayerRace pr = RACE_HUMAN;
	TCODConsole window(world.viewLevel.width, world.viewLevel.height);

	TCODConsole playerClass(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerClass.printFrame(0, 0, playerClass.getWidth(), playerClass.getHeight(), true, TCOD_BKGND_DEFAULT, "Class");
	TCODImage classImage("warrior.png");
	classImage.blit(&playerClass, playerClass.getWidth() / 2.f, playerClass.getHeight() / 2.f);
	playerClass.printEx(playerClass.getWidth() / 2, (playerClass.getHeight() / 4) * 3, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", CLASS_NAMES[c].c_str());
	TCODConsole::blit(&playerClass, 0, 0, 0, 0, &window, 0, 0, 1.f, 1.f);

	TCODConsole playerRace(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerRace.printFrame(0, 0, playerRace.getWidth(), playerRace.getHeight(), true, TCOD_BKGND_DEFAULT, "Race");
	for (int i=0; i<NUM_RACE; i++)
	{
    if(ClassRace[c][i])	playerRace.printEx(2, 2 + 2*i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%c - %s", util::letters[i], RACE_NAMES[i].c_str());
	}
	playerRace.printEx(2, playerRace.getHeight() - 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "r - random");
	TCODConsole::blit(&playerRace, 0, 0, 0, 0, &window, playerClass.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerGender(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerGender.printFrame(0, 0, playerGender.getWidth(), playerGender.getHeight(), true, TCOD_BKGND_DEFAULT, "Gender");

	TCODConsole::blit(&playerGender, 0, 0, 0, 0, &window, playerClass.getWidth() + playerRace.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerName((world.viewLevel.width / 3) * 3, (world.viewLevel.height / 4));
	playerName.printFrame(0, 0, playerName.getWidth(), playerName.getHeight(), true, TCOD_BKGND_DEFAULT, "Name");

	TCODConsole::blit(&playerName, 0, 0, 0, 0, &window, 0, playerClass.getHeight(), 1.f, 1.f);

	TCODConsole::root->clear();
	TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, world.viewLevel.x, world.viewLevel.y, 1.f, 0.9f);
	TCODConsole::root->flush();
	TCOD_key_t key;
	do
	{
		key = world.player->waitForKeypress(true);
		if (key.c == 'r')
		{
			pr = static_cast<PlayerRace>(rng->getInt(0, NUM_RACE - 1));
			break;
		}
		if (key.c >= 'a' && key.c < util::letters[NUM_RACE])
		{
			pr = static_cast<PlayerRace>(util::letterToInt(key.c));
			break;
		}
	}
	while (!world.requestQuit);
	return pr;
}

Gender CharGen::choose_gender(PlayerClass c, PlayerRace r)
{
	Gender pg = GENDER_MALE;
	TCODConsole window(world.viewLevel.width, world.viewLevel.height);

	TCODConsole playerClass(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerClass.printFrame(0, 0, playerClass.getWidth(), playerClass.getHeight(), true, TCOD_BKGND_DEFAULT, "Class");
	TCODImage classImage("warrior.png");
	classImage.blit(&playerClass, playerClass.getWidth() / 2.f, playerClass.getHeight() / 2.f);
	playerClass.printEx(playerClass.getWidth() / 2, (playerClass.getHeight() / 4) * 3, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", CLASS_NAMES[c].c_str());
	TCODConsole::blit(&playerClass, 0, 0, 0, 0, &window, 0, 0, 1.f, 1.f);

	TCODConsole playerRace(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerRace.printFrame(0, 0, playerRace.getWidth(), playerRace.getHeight(), true, TCOD_BKGND_DEFAULT, "Race");
	TCODImage raceImage("warrior.png");
	raceImage.blit(&playerRace, playerRace.getWidth() / 2.f, playerRace.getHeight() / 2.f);
	playerRace.printEx(playerRace.getWidth() / 2, (playerRace.getHeight() / 4) * 3, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", RACE_NAMES[r].c_str());
	TCODConsole::blit(&playerRace, 0, 0, 0, 0, &window, playerClass.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerGender(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerGender.printFrame(0, 0, playerGender.getWidth(), playerGender.getHeight(), true, TCOD_BKGND_DEFAULT, "Gender");
	for (int i=0; i<NUM_GENDER; i++)
	{
		playerGender.printEx(2, 2 + 2*i, TCOD_BKGND_DEFAULT, TCOD_LEFT, "%c - %s", util::letters[i], GENDER_NAMES[i].c_str());
	}
	playerGender.printEx(2, playerGender.getHeight() - 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "r - random");
	TCODConsole::blit(&playerGender, 0, 0, 0, 0, &window, playerClass.getWidth() + playerRace.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerName((world.viewLevel.width / 3) * 3, (world.viewLevel.height / 4));
	playerName.printFrame(0, 0, playerName.getWidth(), playerName.getHeight(), true, TCOD_BKGND_DEFAULT, "Name");

	TCODConsole::blit(&playerName, 0, 0, 0, 0, &window, 0, playerClass.getHeight(), 1.f, 1.f);

	TCODConsole::root->clear();
	TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, world.viewLevel.x, world.viewLevel.y, 1.f, 0.9f);
	TCODConsole::root->flush();
	TCOD_key_t key;
	do
	{
		key = world.player->waitForKeypress(true);
		if (key.c == 'r')
		{
			pg = static_cast<Gender>(rng->getInt(0, NUM_GENDER - 1));
			break;
		}
		if (key.c >= 'a' && key.c < util::letters[NUM_GENDER])
		{
			pg = static_cast<Gender>(util::letterToInt(key.c));
			break;
		}
	}
	while (!world.requestQuit);
	return pg;
}

std::string CharGen::choose_name(PlayerClass c, PlayerRace r, Gender g)
{
	std::string pn = "Nameless";
	TCODConsole window(world.viewLevel.width, world.viewLevel.height);

	TCODConsole playerClass(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerClass.printFrame(0, 0, playerClass.getWidth(), playerClass.getHeight(), true, TCOD_BKGND_DEFAULT, "Class");
	TCODImage classImage("warrior.png");
	classImage.blit(&playerClass, playerClass.getWidth() / 2.f, playerClass.getHeight() / 2.f);
	playerClass.printEx(playerClass.getWidth() / 2, (playerClass.getHeight() / 4) * 3, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", CLASS_NAMES[c].c_str());
	TCODConsole::blit(&playerClass, 0, 0, 0, 0, &window, 0, 0, 1.f, 1.f);

	TCODConsole playerRace(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerRace.printFrame(0, 0, playerRace.getWidth(), playerRace.getHeight(), true, TCOD_BKGND_DEFAULT, "Race");
	TCODImage raceImage("warrior.png");
	raceImage.blit(&playerRace, playerRace.getWidth() / 2.f, playerRace.getHeight() / 2.f);
	playerRace.printEx(playerRace.getWidth() / 2, (playerRace.getHeight() / 4) * 3, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", RACE_NAMES[r].c_str());
	TCODConsole::blit(&playerRace, 0, 0, 0, 0, &window, playerClass.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerGender(world.viewLevel.width / 3, (world.viewLevel.height / 4) * 3 );
	playerGender.printFrame(0, 0, playerGender.getWidth(), playerGender.getHeight(), true, TCOD_BKGND_DEFAULT, "Gender");
	TCODImage genderImage("warrior.png");
	genderImage.blit(&playerGender, playerGender.getWidth() / 2.f, playerGender.getHeight() / 2.f);
	playerGender.printEx(playerGender.getWidth() / 2, (playerGender.getHeight() / 4) * 3, TCOD_BKGND_DEFAULT, TCOD_CENTER, "%s", GENDER_NAMES[g].c_str());
	TCODConsole::blit(&playerGender, 0, 0, 0, 0, &window, playerClass.getWidth() + playerRace.getWidth(), 0, 1.f, 1.f);

	TCODConsole playerName((world.viewLevel.width / 3) * 3, (world.viewLevel.height / 4));
	TCOD_key_t key;
	do
	{
		playerName.clear();
		playerName.printFrame(0, 0, playerName.getWidth(), playerName.getHeight(), true, TCOD_BKGND_DEFAULT, "Name");
		playerName.printEx(20, playerName.getHeight() / 2, TCOD_BKGND_DEFAULT, TCOD_LEFT, "NAME: %s", pn.c_str());
		TCODConsole::blit(&playerName, 0, 0, 0, 0, &window, 0, playerClass.getHeight(), 1.f, 1.f);
		TCODConsole::root->clear();
		TCODConsole::blit(&window, 0, 0, 0, 0, TCODConsole::root, world.viewLevel.x, world.viewLevel.y, 1.f, 0.9f);
		TCODConsole::root->flush();
		key = world.player->waitForKeypress(true);
		if ( key.vk == TCODK_ENTER || key.vk == TCODK_KPENTER) break;
		if ( key.vk == TCODK_BACKSPACE && pn.size() > 0) pn.erase(pn.end() - 1);
		else if ( key.vk == TCODK_CHAR || key.vk == TCODK_SPACE || (key.c >= '0' && key.c <= '9'))
		{
			if ( pn.size() < 15) pn.append(1, key.c);
		}
	}
	while (!world.requestQuit);
	return pn;
}