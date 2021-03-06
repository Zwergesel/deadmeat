#ifndef _WORLD_HPP
#define _WORLD_HPP

#include <deque>
#include <vector>
#include <sstream>
#include "utility.hpp"
#include "item.hpp"
#include "itemselection.hpp"
#include "levelgen.hpp"

class Player;
class Creature;
class Level;
class TileSet;
class Savegame;
class LoadBlock;

class World
{
private:
	std::deque<std::string> messageQueue;
	std::deque<std::string> messageLog;
	World(const World& copy);

public:
	TileSet* tileSet;
	Player* player;
	Level** levels;
	TCODMap* fovMap;
	int currentLevel;
	Point levelOffset;
	Viewport viewLevel, viewMsg, viewItemList, viewInfo;
	bool requestQuit;
	ItemSelection itemSelection;
	bool gameover;
	int time;
	bool clearMessage;
	std::vector<WorldNode> worldNodes;
	std::deque<Creature*> garbage;
	std::string deathReason;

	/* debug parameters / cheat codes */
	int debug_fov;
	int debug_god;

	World();
	~World();

	void clearWorld();
	void newGame();

	void addMessage(std::string m, bool forceBreak=false);
	int getNumMessages();
	void popMessage();
	void drawMessage();
	void drawMessageLog();
	unsigned char drawBlockingWindow(const std::string& title, const std::string& text, const std::string& acceptedKeys, TCODColor color = TCODColor::black, bool center = true);

	void toggleFullscreen();

	void drawLevel(Level* l, Point offset, Viewport view);
	void drawItem(Item* i, Point pos, Point offset, Viewport view);
	void drawCreature(Creature* c, Point offset, Viewport view);
	void drawWorld();
	void drawInfo();
	void drawCursor(Point target, Point offset, bool traceLine);
	void drawItemSelection(ItemSelection& sel);
	void drawCharInfo();

	void buildFovMap();
	void travel();

	void cleanGarbage();

	void debugInput(std::string);

	unsigned int save(Savegame& sg);
	void load(LoadBlock& load);
};

// Global world variable
extern World world;
extern TCODRandom* rng;

#endif