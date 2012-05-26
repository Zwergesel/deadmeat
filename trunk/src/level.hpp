#ifndef LEVEL_HPP
#define LEVEL_HPP

struct Tile 
{
	unsigned char symbol;
	bool passable;
	Tile():symbol('?'),passable(false){};
	Tile(unsigned char s, bool p):symbol(s),passable(p){};
};

class Level 
{
private:
	int width, height;
	Tile* map;
	
	inline int coord(int x, int y);	// x, y => array index
	
public:
	Level(int width,int height);				// width, height
	~Level();
	void display(int offsetX, int offsetY);		// offsetX, offsetY	
  void setTile(int x, int y, Tile t);
  Tile getTile(int x, int y);
  void setSize(int width, int height);
  int getWidth();
  int getHeight();
};

#endif