#include <libtcod.hpp>

int main()
{
  TCODConsole::initRoot(80,50,"deadmeat",false);
  TCODSystem::setFps(30);
  TCODConsole::root->credits();
  TCODConsole::root->clear();
  TCODConsole::root->setDefaultBackground(TCODColor::black);
  TCODConsole::root->setDefaultForeground(TCODColor::white);

  while(!TCODConsole::isWindowClosed())
  {
    TCODConsole::root->print(0,0,"%d",TCODSystem::getFps());
    TCODConsole::root->setDefaultBackground(TCODColor::azure);
    TCODConsole::root->setDefaultForeground(TCODColor::amber);
    TCODConsole::root->printFrame(10,10,60,10,true,TCOD_BKGND_DEFAULT,"hello");
    TCODConsole::root->setChar(40,25,TCOD_CHAR_CHECKBOX_SET);

    TCODConsole::root->flush();
    TCOD_key_t key = TCODConsole::root->waitForKeypress(true);
    if(key.vk == TCODK_ENTER && key.lalt) TCODConsole::root->setFullscreen(!TCODConsole::isFullscreen());
  }
  return 0;
}
