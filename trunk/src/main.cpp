#include <libtcod.hpp>

int main()
{
  TCODConsole::initRoot(80,50,"deadmeat",false);
  while(!TCODConsole::isWindowClosed())
  {
    TCODConsole::flush();
    TCOD_key_t key = TCODConsole::waitForKeypress(true);
    if(key.vk == TCODK_ENTER && key.lalt) TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
  }
  return 0;
}
