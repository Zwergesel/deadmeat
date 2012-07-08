#include "levelgen.hpp"
#include "level.hpp"
#include "world.hpp"
#include "creature.hpp"
#include <cassert>
#include <iostream>
#include <cstring>
#include <deque>

RandomTable loot;

void LevelGen::generateWorld()
{
	// hardcoded world for now
	WorldNode over[4];
	over[0].type = LEVELTYPE_FOREST;
	over[1].type = LEVELTYPE_FOREST;
	over[2].type = LEVELTYPE_FOREST;
	WorldNode secret;
	secret.type = LEVELTYPE_ROOM;
	WorldNode house;
	house.type = LEVELTYPE_BSP;
	WorldNode* dungeon = new WorldNode[3];
	dungeon[0].type = LEVELTYPE_ROOM;
	dungeon[1].type = LEVELTYPE_CAVE;
	dungeon[2].type = LEVELTYPE_CAVE;
	WorldNode boss;
	boss.type = LEVELTYPE_SMAUGS_LAIR;
	WorldLink in1 = {OBJ_STAIRSSAME, Point(), 1, 0, 0};
	WorldLink out1 = {OBJ_STAIRSSAME, Point(), 0, 0, 0};
	over[0].link.push_back(in1);
	over[1].link.push_back(out1);
	WorldLink in2 = {OBJ_STAIRSSAME, Point(), 2, 1, 0};
	WorldLink out2 = {OBJ_STAIRSSAME, Point(), 0, 0, 1};
	over[0].link.push_back(in2);
	over[2].link.push_back(out2);
	WorldLink in3 = {OBJ_STAIRSSAME, Point(), 2, 1, 1};
	WorldLink out3 = {OBJ_STAIRSSAME, Point(), 1, 1, 1};
	over[1].link.push_back(in3);
	over[2].link.push_back(out3);
	WorldLink inS = {OBJ_STAIRSDOWN, Point(), 3, 2, 0};
	WorldLink outS = {OBJ_STAIRSUP, Point(), 1, 0, 2};
	over[1].link.push_back(inS);
	secret.link.push_back(outS);
	WorldLink inH = {OBJ_STAIRSSAME, Point(), 4, 2, 0};
	WorldLink outH = {OBJ_STAIRSSAME, Point(), 0, 0, 2};
	over[0].link.push_back(inH);
	house.link.push_back(outH);
	WorldLink inD1 = {OBJ_STAIRSDOWN, Point(), 5, 2, 0};
	WorldLink outD1 = {OBJ_STAIRSUP, Point(), 2, 0, 2};
	over[2].link.push_back(inD1);
	dungeon[0].link.push_back(outD1);
	WorldLink inD2 = {OBJ_STAIRSDOWN, Point(), 6, 1, 0};
	WorldLink outD2 = {OBJ_STAIRSUP, Point(), 5, 0, 1};
	dungeon[0].link.push_back(inD2);
	dungeon[1].link.push_back(outD2);
	WorldLink inD3 = {OBJ_STAIRSDOWN, Point(), 7, 1, 0};
	WorldLink outD3 = {OBJ_STAIRSUP, Point(), 6, 0, 1};
	dungeon[1].link.push_back(inD3);
	dungeon[2].link.push_back(outD3);
	WorldLink inB = {OBJ_STAIRSDOWN, Point(), 8, 1, 0};
	WorldLink outB = {OBJ_STAIRSUP, Point(), 7, 0, 1};
	dungeon[2].link.push_back(inB);
	boss.link.push_back(outB);

	world.worldNodes.push_back(over[0]);
	world.worldNodes.push_back(over[1]);
	world.worldNodes.push_back(over[2]);
	world.worldNodes.push_back(secret);
	world.worldNodes.push_back(house);
	world.worldNodes.push_back(dungeon[0]);
	world.worldNodes.push_back(dungeon[1]);
	world.worldNodes.push_back(dungeon[2]);
	world.worldNodes.push_back(boss);
}

void LevelGen::loadLootTable()
{
	/* Melee Weapons */
	loot.add("sword", 45);
	loot.add("dagger", 90);
	loot.add("halberd", 30);
	loot.add("lightsaber", 5);
	
	/* Ranged Weapons */
	loot.add("longbow", 30);
	loot.add("crossbow", 30);
	loot.add("slingshot", 80);
	
	/* Armor */
	loot.add("uber armor", 30);
	loot.add("pickelhaube", 70);
	loot.add("clown shoes", 50);
	
	/* Food */
	loot.add("beefsteak", 150);
	loot.add("meatball", 300);
	
	/* Ammo */
	loot.add("arrows", 100);
	loot.add("bolts", 100);
	loot.add("bullets", 150);
	
	/* Tools */
	loot.add("lockpick", 25);
	
	/* Gold */
	loot.add("gold", 350);
}

Level* LevelGen::generateLevel(int levelId, LEVELTYPE type)
{
	switch (type)
	{
	case LEVELTYPE_SMAUGS_LAIR:
	{
		Level* l = generateCaveLevel(levelId, 100, 80);
		RandomTable lair;
		lair.add("red baby dragon", 100).add("purple baby dragon", 200).add("blue baby dragon", 100);
		l->populate(lair, 12);
		Creature* smaug = factory.spawnCreature("Smaug");
		smaug->moveTo(l->getRandomLocation(WALKABLE | NO_CREATURE));
		l->addCreature(smaug, world.time);
		return l;
	}
	case LEVELTYPE_CAVE:
		return generateCaveLevel(levelId, rng->getInt(40, 100), rng->getInt(20, 80));
	case LEVELTYPE_ROOM:
		return generateRoomLevel(levelId, rng->getInt(40, 100), rng->getInt(20, 80));
	case LEVELTYPE_BSP:
		return generateBSPLevel(levelId, rng->getInt(20, 50), rng->getInt(20, 40));
	case LEVELTYPE_FOREST:
		return generateForestLevel(levelId, rng->getInt(30, 90), rng->getInt(25, 60));
	default:
	case LEVELTYPE_PLAIN:
		return generatePlainLevel(levelId, rng->getInt(10,100), rng->getInt(10,100));
	}
	return NULL;
}

Level* LevelGen::generateCaveLevel(int levelId, int width, int height, float density)
{
	if (width <= 2 || height <= 2) return NULL;
	Level* m = new Level(width,height);
	int* swap1 = new int[width*height];
	int* swap2 = new int[width*height];

	// 0=solid, 1=open
	std::fill(swap1,swap1+width*height,0);
	std::fill(swap2,swap2+width*height,0);
	for (int x=2; x<width-2; x++) for (int y=2; y<height-2; y++)
			swap1[x + y * width] = (rng->getFloat(0.f,100.f) <= density) ? 0 : 1;

	for (int i=0; i<4; i++)
	{
		for (int x=2; x<width-2; x++) for (int y=2; y<height-2; y++)
			{
				// count solid spaces in radius one and two
				int R1 = 9;
				int R2 = 21;
				for (int offx=-1; offx<=1; offx++) for (int offy=-1; offy<=1; offy++)
						R1 -= swap1[x + offx + (y + offy) * width];
				for (int offx=-2; offx<=2; offx++) for (int offy=-2; offy<=2; offy++)
					{
						if (abs(offx) == 2 && abs(offy) == 2) continue;
						R2 -= swap1[x + offx + (y + offy) * width];
					}
				// CA rule #1
				if (R1 >= 5 || R2 <= 2) swap2[x + y * width] = 0;
				else swap2[x + y * width] = 1;
			}
		for (int i=0; i<width*height; i++) swap1[i] = swap2[i];
	}

	for (int i=0; i<3; i++)
	{
		for (int x=2; x<width-2; x++) for (int y=2; y<height-2; y++)
			{
				int R1 = 9;
				for (int offx=-1; offx<=1; offx++) for (int offy=-1; offy<=1; offy++)
						R1 -= swap1[x + offx + (y + offy) * width];
				// CA rule #2
				if (R1 >=5) swap2[x + y * width] = 0;
				else swap2[x + y * width] = 1;
			}
		for (int i=0; i<width*height; i++) swap1[i] = swap2[i];
	}

	for (int x=0; x<width; x++) for (int y=0; y<height; y++)
		{
			if (swap1[x + y * width] == 0) m->setTile(Point(x, y), TILE_CAVE_WALL);
			else if (swap1[x + y * width] == 1) m->setTile(Point(x, y), TILE_CAVE_FLOOR);
		}

	delete[] swap1;
	delete[] swap2;

	if (!placeEntrances(levelId, m)) return NULL;

	// Cave monsters
	RandomTable cave;
	cave.add("goblin", 180).add("spider", 400).add("rat", 250).add("troll", 80);
	m->populate(cave, width*height/250);
	m->placeItems(loot, width*height/350);

	return m;
}

class RoomLevelCorridorsPathfinding : public ITCODPathCallback
{
public:
	virtual float getWalkCost( int xFrom, int yFrom, int xTo, int yTo, void *userData) const
	{
		int* dat = static_cast<int*>(userData);
		int width = dat[0];
		int from = yFrom*width+xFrom;
		int to = yTo*width+xTo;
		if (dat[1+from] == 3 || dat[1+to] == 3) return 0.0f;
		if (dat[1+to] == 1) return 0.3f;
		if (dat[1+to] == 2) return 5.0f;
		return 1.0f;
	}
};

Level* LevelGen::generateRoomLevel(int levelId, int width, int height, float roomDensity)
{
	if (width <= 2 || height <= 2) return NULL;
	// 0=wall, 1=free, 2=door, 3=roomWall
	int* swap1 = new int[1 + width*height];
	std::fill(swap1,swap1+1+width*height,0);
	swap1[0] = width;

	int roomNumber = static_cast<int>((static_cast<float>(width*height) / (5*5)) * roomDensity  / 100.f);
	if (roomNumber <= 0) return NULL;

	std::vector<Point> doors;

	for (int r=0; r<roomNumber; r++)
	{
		int roomType = rng->getInt(0,100);
		// rectangle
		if (roomType <= 80)
		{
			int sizeX = rng->getInt(3, 10);
			int sizeY = rng->getInt(3, 10);
			int x = rng->getInt(1, width - sizeX - 1);
			int y = rng->getInt(1, height - sizeY - 1);
			bool free = true;
			for (int i=-1; i<sizeX+1; i++) for (int j=-1; j<sizeY+1; j++)
				{
					if (swap1[1+x+i+(y+j)*width] != 0) free = false;
				}
			if (!free) continue;
			for (int i=-1; i<sizeX+1; i++) for (int j=-1; j<sizeY+1; j++)
				{
					if (i<0 || j<0 || i==sizeX || j==sizeY) swap1[1+x+i+(y+j)*width] = 3;
					else swap1[1+x+i+(y+j)*width] = 1;
				}
			int numDoors = rng->getInt(1, std::min(sizeX/2, sizeY/2));
			for (int i=0; i<numDoors; i++)
			{
				Point p;
				int side = rng->getInt(0,3);
				int d = rng->getInt(0, (side%2 == 0)?(sizeX - 1):(sizeY - 1));
				if (side == 0)
				{
					p.x = x + d;
					p.y = y - 1;
				}
				if (side == 1)
				{
					p.x = x - 1;
					p.y = y + d;
				}
				if (side == 2)
				{
					p.x = x + d;
					p.y = y + sizeY;
				}
				if (side == 3)
				{
					p.x = x + sizeX;
					p.y = y + d;
				}
				if ( p.x > 0 && p.x < width-1 && p.y > 0 && p.y < height -1)
				{
					doors.push_back(p);
					swap1[1+p.x + p.y * width] = 2;
				}
			}
		}
		// diamond
		else if (roomType <= 90)
		{
			int size = rng->getInt(3, 10);
			int x = rng->getInt(0, width - 2*size - 1);
			int y = rng->getInt(0, height - 2*size - 1);
			bool free = true;
			for (int i=0; i<2*size+1; i++) for (int j=0; j<2*size+1; j++)
				{
					int d = abs(i-size) + abs(j-size);
					if (d <= size && swap1[1+x+i+(y+j)*width] != 0) free = false;
				}
			if (!free) continue;
			for (int i=0; i<2*size+1; i++) for (int j=0; j<2*size+1; j++)
				{
					int d = abs(i-size) + abs(j-size);
					if (d == size || d == size + 1) swap1[1+x+i+(y+j)*width] = 3;
					if (d < size) swap1[1+x+i+(y+j)*width] = 1;
				}
			int numDoors = rng->getInt(1, 4);
			for (int i=0; i<numDoors; i++)
			{
				Point p;
				int side = rng->getInt(0,3);
				if (side == 0)
				{
					p.x = x;
					p.y = y + size;
				}
				if (side == 1)
				{
					p.x = x + size;
					p.y = y;
				}
				if (side == 2)
				{
					p.x = x + 2*size;
					p.y = y + size;
				}
				if (side == 3)
				{
					p.x = x + size;
					p.y = y + 2*size;
				}
				if ( p.x > 0 && p.x < width-1 && p.y > 0 && p.y < height -1)
				{
					doors.push_back(p);
					swap1[1+p.x + p.y * width] = 2;
				}
			}
		}
		// rect. diamond
		else
		{
			int sizeX = rng->getInt(1, 10);
			int sizeY = rng->getInt(3, 10);
			int x = rng->getInt(0, width - 2*sizeY - sizeX - 2);
			int y = rng->getInt(0, height - 2*sizeY - 1);
			bool free = true;
			for (int i=0; i<2*sizeY+sizeX+2; i++) for (int j=0; j<2*sizeY+1; j++)
				{
					int dl = abs(i-sizeY) + abs(j-sizeY);
					int dr = abs(i - sizeY - sizeX - 1) + abs(j - sizeY);
					if (i <= sizeY)
					{
						if (dl <= sizeY && swap1[1+x+i+(y+j)*width] != 0) free = false;
					}
					else if (i >= sizeY + sizeX + 1)
					{
						if (dr <= sizeY && swap1[1+x+i+(y+j)*width] != 0) free = false;
					}
					else
					{
						if (swap1[1+x+i+(y+j)*width] != 0) free = false;
					}
				}
			if (!free) continue;
			for (int i=0; i<2*sizeY+sizeX+2; i++) for (int j=0; j<2*sizeY+1; j++)
				{
					int dl = abs(i-sizeY) + abs(j-sizeY);
					int dr = abs(i - sizeY - sizeX - 1) + abs(j - sizeY);
					if (i <= sizeY)
					{
						if (dl == sizeY  || dl == sizeY + 1) swap1[1+x+i+(y+j)*width] = 3;
						if (dl < sizeY) swap1[1+x+i+(y+j)*width] = 1;
					}
					else if (i >= sizeY + sizeX + 1)
					{
						if (dr == sizeY  || dr == sizeY + 1) swap1[1+x+i+(y+j)*width] = 3;
						if (dr < sizeY) swap1[1+x+i+(y+j)*width] = 1;
					}
					else
					{
						if (j==0 || j==2*sizeY) swap1[1+x+i+(y+j)*width] = 3;
						else swap1[1+x+i+(y+j)*width] = 1;
					}
				}
			int numDoors = rng->getInt(1, 4);
			for (int i=0; i<numDoors; i++)
			{
				Point p;
				int side = rng->getInt(0,3);
				int d = rng->getInt(0, sizeX + 2);
				if (side == 0)
				{
					p.x = x;
					p.y = y + sizeY;
				}
				if (side == 1)
				{
					p.x = x + sizeY + d;
					p.y = y;
				}
				if (side == 2)
				{
					p.x = x + 2*sizeY + sizeX + 1;
					p.y = y + sizeY;
				}
				if (side == 3)
				{
					p.x = x + sizeY + d;
					p.y = y + 2*sizeY;
				}
				if ( p.x > 0 && p.x < width-1 && p.y > 0 && p.y < height -1)
				{
					doors.push_back(p);
					swap1[1+p.x + p.y * width] = 2;
				}
			}
		}
	}

	// corridors
	TCODMap map(width, height);
	for (int x=0; x<width; x++) for (int y=0; y<height; y++)
		{
			map.setProperties(x,y,false,false);
			if (swap1[1+x+y*width] == 0 || swap1[1+x+y*width] == 2) map.setProperties(x,y,false,true);
		}
	for (int c=0; c<(int)doors.size()*10; c++)
	{
		int a = rng->getInt(0, doors.size() - 1);
		int b = rng->getInt(0, doors.size() - 1);
		if (a == b) continue;

		RoomLevelCorridorsPathfinding pathFinding;
		TCODDijkstra path(width,height,&pathFinding,swap1,0.0f);
		path.compute(doors[a].x, doors[a].y);
		path.setPath(doors[b].x, doors[b].y);
		while (!path.isEmpty())
		{
			int x,y;
			path.walk(&x,&y);
			if (swap1[1+x+y*width] == 0)
			{
				swap1[1+x+y*width] = 1;
			}
		}
	}

	Level* m = new Level(width,height);
	for (int x=0; x<width; x++) for (int y=0; y<height; y++)
		{
			if (swap1[1+x + y * width] == 0 || swap1[1+x + y *width] == 3) m->setTile(Point(x, y), TILE_CAVE_WALL);
			else if (swap1[1+x + y * width] == 1) m->setTile(Point(x, y), TILE_CAVE_FLOOR);
			else if (swap1[1+x + y * width] == 2)
			{
				m->setTile(Point(x,y), TILE_CAVE_FLOOR);
				m->addObject(Object(rng->getFloat(0,1) < 0.2 ? OBJ_DOOR_LOCKED : OBJ_DOOR_CLOSED), Point(x,y));
			}
		}
	delete[] swap1;
	if (!placeEntrances(levelId, m)) return NULL;

	// RoomLevel monsters
	RandomTable room;
	room.add("goblin", 300).add("troll", 120).add("rat", 400).add("soldier ant", 220).add("soldier", 100);
	m->populate(room, width*height/250);
	m->placeItems(loot, width*height/350);

	return m;
}

class BSPLevelTraversing : public ITCODBspCallback
{
public:
	bool visitNode(TCODBsp* node, void* userData)
	{
		Level* m = static_cast<Level*>(userData);
		if (!node->isLeaf())
		{
			if (node->horizontal)
			{
				m->setTile(Point(node->getRight()->x + node->getRight()->w / 2, node->getRight()->y - 2), TILE_CAVE_FLOOR);
				m->setTile(Point(node->getRight()->x + node->getRight()->w / 2, node->getRight()->y - 1), TILE_CAVE_FLOOR);
				m->setTile(Point(node->getRight()->x + node->getRight()->w / 2, node->getRight()->y + 1), TILE_CAVE_FLOOR);
				m->setTile(Point(node->getRight()->x + node->getRight()->w / 2, node->getRight()->y), TILE_CAVE_FLOOR);
				m->addObject(Object(OBJ_DOOR_CLOSED), Point(node->getRight()->x + node->getRight()->w / 2, node->getRight()->y));
			}
			else
			{
				m->setTile(Point(node->getRight()->x - 2, node->getRight()->y + node->getRight()->h / 2), TILE_CAVE_FLOOR);
				m->setTile(Point(node->getRight()->x - 1, node->getRight()->y + node->getRight()->h / 2), TILE_CAVE_FLOOR);
				m->setTile(Point(node->getRight()->x + 1, node->getRight()->y + node->getRight()->h / 2), TILE_CAVE_FLOOR);
				m->setTile(Point(node->getRight()->x, node->getRight()->y + node->getRight()->h / 2), TILE_CAVE_FLOOR);
				m->addObject(Object(OBJ_DOOR_CLOSED), Point(node->getRight()->x, node->getRight()->y + node->getRight()->h / 2));
			}
		}
		else
		{
			for (int x=1; x<node->w - 1; x++) for (int y=1; y<node->h - 1; y++)
				{
					m->setTile(Point(node->x + x, node->y + y), TILE_CAVE_FLOOR);
				}
		}
		return true;
	}
};

Level* LevelGen::generateBSPLevel(int levelId, int width, int height, int numSplits, float squareness)
{
	Level* m = new Level(width, height);
	for (int x=0; x<width; x++) for (int y=0; y<height; y++)
		{
			m->setTile(Point(x,y), TILE_CAVE_WALL);
		}
	TCODBsp bsp(0, 0, width, height);
	if (numSplits < 0) numSplits = 8;
	bsp.splitRecursive(NULL, numSplits, 7, 7, squareness, squareness);
	BSPLevelTraversing traverse;
	bsp.traverseInvertedLevelOrder(&traverse, static_cast<void*>(m));

	if (!placeEntrances(levelId, m)) return NULL;

	// BSP monsters
	RandomTable bspm;
	bspm.add("soldier", 500).add("sergeant", 80).add("medic", 200);
	m->populate(bspm, width*height/150);
	m->placeItems(loot, width*height/350);

	return m;
}

bool LevelGen::placeEntrances(int levelId, Level* l)
{
	assert((int)world.worldNodes.size() > levelId);
	if (l == NULL || world.worldNodes[levelId].link.size() <= 0) return true;
	// place first entrance
	std::vector<Point> list;
	for (int x=0; x<l->getWidth(); x++) for (int y=0; y<l->getHeight(); y++)
		{
			if (l->isWalkable(Point(x,y))) list.push_back(Point(x,y));
		}
	if (list.size() < 1) return false;
	Point first = list[rng->getInt(0, list.size() - 1)];
	Object firstEntrance(world.worldNodes[levelId].link[0].type);
	l->addObject(firstEntrance, first);
	world.worldNodes[levelId].link[0].pos = first;
	// build distance tree from first entrance
	TCODMap map(l->getWidth(), l->getHeight());
	for (int x=0; x<l->getWidth(); x++) for (int y=0; y<l->getHeight(); y++)
		{
			map.setProperties(x, y, false, l->isWalkable(Point(x,y)));
		}
	TCODDijkstra dtree(&map);
	dtree.compute(first.x, first.y);
	list.clear();
	for (int x=0; x<l->getWidth(); x++) for (int y=0; y<l->getHeight(); y++)
		{
			if (l->isWalkable(Point(x,y)) && dtree.getDistance(x,y) > 0.f) list.push_back(Point(x,y));
		}
	if (list.size() < world.worldNodes[levelId].link.size() - 1) return false;
	// place remaining entrances
	for (int i=1; i<(int)world.worldNodes[levelId].link.size(); i++)
	{
		int id = rng->getInt(0, list.size() - i);
		Point p = list[id];
		world.worldNodes[levelId].link[i].pos = p;
		Object entrance(world.worldNodes[levelId].link[i].type);
		l->addObject(entrance, p);
		list.erase(list.begin() + id);
	}

	return true;
}

Level* LevelGen::generatePlainLevel(int levelId, int width, int height)
{
	Level* m = new Level(width, height);
	for (int x=0; x<m->getWidth(); x++) for (int y=0; y<m->getHeight(); y++)
		{
			m->setTile(Point(x,y), TILE_GRASS);
		}

	placeEntrances(levelId, m);

	// Meadow monsters
	RandomTable meadow;
	meadow.add("goblin", 50).add("snake", 400).add("soldier ant", 200);
	m->populate(meadow, width*height/800 + 2);
	m->placeItems(loot, width*height/800 + 1);

	return m;
}

class ForestPathFinding : public ITCODPathCallback
{
public:
	virtual float getWalkCost( int xFrom, int yFrom, int xTo, int yTo, void *userData) const
	{
		Level* dat = static_cast<Level*>(userData);
		Tile tile = dat->getTile(Point(xTo,yTo));
		if (tile == TILE_DARK_GRASS) return 1.0f;
		return 5000.0f;
	}
};

Level* LevelGen::generateForestLevel(int levelId, int width, int height)
{
	Level* m = new Level(width, height);

	// Heightmap
	TCODHeightMap map(width, height);
	TCODNoise noise(2);
	map.addFbm(&noise, width / 6.f, height / 6.f, 0.f, 0.f, 20.f, 0.f, 0.7f);
	map.addFbm(&noise, width / 2.f, height / 2.f, 0.f, 0.f, 20.f, 0.f, 0.3f);

	// Fill level
	for (int x=0; x<width; x++) for (int y=0; y<height; y++)
		{
			if (map.getValue(x,y) > std::min(0.1, -1.0 + std::min(std::min(x,width-x-1),std::min(y,height-y-1)) * 0.25)
			    || rng->getInt(0,99) < 8)
			{
				m->setTile(Point(x,y), rng->getInt(0,2) == 0 ? TILE_TREE2 : TILE_TREE1);
			}
			else
			{
				m->setTile(Point(x,y), TILE_DARK_GRASS);
			}
		}

	bool* bfs = new bool[width*height];
	std::fill(bfs, bfs+width*height, false);
	Point start = m->getRandomLocation(WALKABLE);
	bool complete;

	do
	{
		std::deque<Point> deq;
		deq.push_back(start);
		bfs[start.x+width*start.y] = true;

		// Flood fill level
		while (!deq.empty())
		{
			Point p = deq.front();
			deq.pop_front();
			Point t = p + Point(-1,0);
			if (p.x > 0 && m->getTile(t) == TILE_DARK_GRASS && !bfs[t.x+width*t.y])
			{
				bfs[t.x+width*t.y] = true;
				deq.push_back(t);
			}
			t = p + Point(+1,0);
			if (p.x < width-1 && m->getTile(t) == TILE_DARK_GRASS && !bfs[t.x+width*t.y])
			{
				bfs[t.x+width*t.y] = true;
				deq.push_back(t);
			}
			t = p + Point(0,-1);
			if (p.y > 0 && m->getTile(t) == TILE_DARK_GRASS && !bfs[t.x+width*t.y])
			{
				bfs[t.x+width*t.y] = true;
				deq.push_back(t);
			}
			t = p + Point(0,+1);
			if (p.y < height-1 && m->getTile(t) == TILE_DARK_GRASS && !bfs[t.x+width*t.y])
			{
				bfs[t.x+width*t.y] = true;
				deq.push_back(t);
			}
		}

		// Check whether whole level is filled
		complete = true;
		Point end;

		for (int x=0; x<width; x++) for (int y=0; y<height; y++)
			{
				if (m->getTile(Point(x,y)) == TILE_DARK_GRASS && !bfs[x+width*y])
				{
					end = Point(x,y);
					complete = false;
					break;
				}
			}

		if (complete) break;

		// Build a path between disconnected areas
		ForestPathFinding pathFinding;
		TCODDijkstra path(width,height,&pathFinding,m,0.0f);
		path.compute(start.x, start.y);
		path.setPath(end.x, end.y);
		while (!path.isEmpty())
		{
			Point p;
			path.walk(&p.x,&p.y);
			m->setTile(p, TILE_DARK_GRASS);
		}

		start = end;
	}
	while (true);

	delete[] bfs;

	placeEntrances(levelId, m);

	// Meadow monsters
	RandomTable forest;
	forest.add("goblin", 50).add("snake", 400).add("soldier ant", 200);
	m->populate(forest, width*height/250 + 1);
	m->placeItems(loot, width*height/250 + 1);

	std::vector<Point> list = m->getMatchingLocations(WALKABLE | NO_OBJECT);
	m->addObject(OBJ_TRAP_FIRE, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_FIRE, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_FIRE, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_FIRE, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_FIRE, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_BEAR, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_BEAR, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_BEAR, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_BEAR, m->chooseRandomPoint(list, true));
	m->addObject(OBJ_TRAP_BEAR, m->chooseRandomPoint(list, true));

	return m;
}