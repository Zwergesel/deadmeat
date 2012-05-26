#include <libtcod.hpp>
#include <algorithm>
#include "level.hpp"

Player player;

int main()
{
  TCODConsole::initRoot(80,51,"deadmeat",false);
  TCODSystem::setFps(30);

	Level nethack(80,50);
  player.x = 40; player.y = 25;

  while(!TCODConsole::isWindowClosed())
  {   
    TCODConsole::root->clear();
    TCODConsole::root->setChar(player.x,player.y,'@');
    TCODConsole::root->print(0, 50, "Player.x = %d, Player.y = %d", player.x, player.y);
    TCODConsole::root->flush();    

    TCOD_key_t key = TCODConsole::root->waitForKeypress(true);
    // toogle fullscreen
    if(key.vk == TCODK_ENTER && key.lalt) TCODConsole::root->setFullscreen(!TCODConsole::isFullscreen());
    // numpad player movement
    else if(key.vk >= TCODK_KP1 && key.vk <= TCODK_KP9)
    {
      int newx = player.x + Player::dx[key.vk - TCODK_KP1];
      int newy = player.y + Player::dy[key.vk - TCODK_KP1];
      if(newx >= 0 && newx < 80 && newy >= 0 && newy < 50)
      {
        player.x = newx;
        player.y = newy;
      }
    }
  }
  return 0;
}
