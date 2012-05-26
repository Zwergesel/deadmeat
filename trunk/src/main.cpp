#include <libtcod.hpp>
#include "level.hpp"

int main()
{
	TCODConsole::initRoot(80,50,"deadmeat",false);
	TCODSystem::setFps(30);
	TCODConsole::root->setDefaultBackground(TCODColor::white);
	TCODConsole::root->setDefaultForeground(TCODColor::black);

	Level nethack(80,50);

	while(!TCODConsole::isWindowClosed())
	{
		TCODConsole::root->clear();
		nethack.display(0,0);
		TCODConsole::root->flush();
		TCOD_key_t key = TCODConsole::root->waitForKeypress(true);
		if (key.vk == TCODK_ENTER && key.lalt) {
			TCODConsole::root->setFullscreen(!TCODConsole::isFullscreen());
		}
	}
	return 0;
}
