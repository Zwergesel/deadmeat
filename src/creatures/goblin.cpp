#include "../creature.hpp"
#include "../world.hpp"
#include "../player.hpp"
#include "../tileset.hpp"
#include "../level.hpp"
#include "../pathfinding.hpp"

Goblin::Goblin() : Creature(Point(20, 20), "goblin", 'g', TCODColor::green, 20)
{
	baseWeapon = Weapon("hands", (unsigned char)'¤', TCODColor::pink, 15, 10, 10, 0, 0, 2, SKILL_UNARMED, 2);
	attackSkill = 10;
}

int Goblin::action()
{
	Point ppos = world.player->getCreature()->getPos();
	TCODPath path = TCODPath(level->getWidth(), level->getHeight(), new PathFindingCallback(), level);
	path.compute(position.x, position.y, ppos.x, ppos.y);

	int tx, ty;
	if (!path.isEmpty() && path.walk(&tx, &ty, true))
	{
		Point target = Point(tx, ty);
		Creature* c = level->creatureAt(target);
		if (c != NULL && c->isControlled())
		{
			return attack(c);
		}
		else
		{
			position = target;
			return 10;
		}
	}
	return 10;
}

/*--------------------- SAVING AND LOADING ---------------------*/

unsigned int Goblin::save(Savegame& sg)
{
	unsigned int id;
	if (sg.saved(this, &id)) return id;
	SaveBlock store("Goblin", id);
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

// Note: Loading should work fine through Creature::load