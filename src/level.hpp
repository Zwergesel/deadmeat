#ifndef LEVEL_HPP
#define LEVEL_HPP

struct Tile {
	char symbol;
	bool passable;
	Tile():symbol('?'),passable(false){};
	Tile(char s, bool p):symbol(s),passable(p){};
};

class Level {

	private:
	int width, height;
	Tile* map;
	
	inline int coord(int,int);	// x, y => array index
	
	public:
	Level(int,int);				// width, height
	~Level();
	void display(int, int);		// offsetX, offsetY
	
};

#endif