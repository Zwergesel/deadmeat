#include "levelgen.hpp"
#include <algorithm>
#include <libtcod.hpp>

Level* LevelGen::generateCaveLevel(int width, int height)
{
  if(width <= 2 || height <= 2) return NULL;
  Level* m = new Level(width,height);
  TCODRandom rng;
  int* swap1 = new int[width*height];
  int* swap2 = new int[width*height];

  // 0=solid, 1=open
  std::fill(swap1,swap1+width*height,0);
  std::fill(swap2,swap2+width*height,0);
  for(int x=2;x<width-2;x++) for(int y=2;y<height-2;y++)
      swap1[x + y * width] = (rng.getInt(0,100) <= 40) ? 0 : 1;
  
  for(int i=0;i<4;i++)
  {
    for(int x=2;x<width-2;x++) for(int y=2;y<height-2;y++)
    {
      // count solid spaces in radius one and two
      int R1 = 9;
      int R2 = 21;
      for(int offx=-1;offx<=1;offx++) for(int offy=-1;offy<=1;offy++)
        R1 -= swap1[x + offx + (y + offy) * width];
      for(int offx=-2;offx<=2;offx++) for(int offy=-2;offy<=2;offy++)
      {
        if(abs(offx) == 2 && abs(offy) == 2) continue;
        R2 -= swap1[x + offx + (y + offy) * width];
      }
      // CA rule #1
      if(R1 >= 5 || R2 <= 2) swap2[x + y * width] = 0;
      else swap2[x + y * width] = 1;
    }
    for(int i=0;i<width*height;i++) swap1[i] = swap2[i];
  }

  for(int i=0;i<3;i++)
  {
    for(int x=2;x<width-2;x++) for(int y=2;y<height-2;y++)
    {
      int R1 = 9;
      for(int offx=-1;offx<=1;offx++) for(int offy=-1;offy<=1;offy++)
        R1 -= swap1[x + offx + (y + offy) * width];
      // CA rule #2
      if(R1 >=5) swap2[x + y * width] = 0;
      else swap2[x + y * width] = 1;
    }
    for(int i=0;i<width*height;i++) swap1[i] = swap2[i];
  }

  for(int x=0;x<width;x++) for(int y=0;y<height;y++)
  {
    if(swap1[x + y * width] == 0) m->setTile(x, y, Tile(TCOD_CHAR_BLOCK1, false));
    else if(swap1[x + y * width] == 1) m->setTile(x, y, Tile('.', true));
  }

  delete[] swap1;
  delete[] swap2;

  return m;
}