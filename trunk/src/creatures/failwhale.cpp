#include "../creature.hpp"
#include "../world.hpp"
#include "../player.hpp"
#include "../tileset.hpp"
#include "../level.hpp"

FailWhale::FailWhale() : Creature(Point(60, 35), "fail whale", 'w', TCODColor::blue, 10000)
{
	baseWeapon = Weapon("teeth maybe?", '§', TCODColor::pink, 5, 10, 1, 5, 10, 2, SKILL_UNARMED, 2);
	attackSkill = 1;
}

int FailWhale::action()
{
	Point ppos = world.player->getCreature()->getPos();
	int dx = util::sign(ppos.x - position.x);
	int dy = util::sign(ppos.y - position.y);
	Point target(position.x+dx, position.y+dy);

	if (target.x == ppos.x && target.y == ppos.y)
	{
		return attack(world.player->getCreature());
	}
	else if (world.tileSet->isPassable(level->getTile(target)) && level->creatureAt(target) == NULL)
	{
		position.x += dx;
		position.y += dy;
		return 21;
	}
	else
	{
		return 8;
	}
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int FailWhale::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("FailWhale", id);
	store ("name", name) ("symbol", symbol) ("position", position);
	store ("color", color) ("health", health) ("maxHealth", maxHealth);
	store ("controlled", controlled);
	store.ptr("mainWeapon", mainWeapon == NULL ? 0 : mainWeapon->save(sg));
	store.ptr("armor", armor == NULL ? 0 : armor->save(sg));
	store ("attackSkill", attackSkill) ("armorSkill", armorSkill);
	store.ptr("baseWeapon", baseWeapon.save(sg)).ptr("baseArmor", baseArmor.save(sg));
	sg << store;
	return id;
}

// Note: Creature::load should work fine for loading