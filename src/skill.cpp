#include "skill.hpp"

Skill::Skill()
{
}

Skill::Skill(std::string n, int v, int mv, int ml, int e):
	name(n),value(v),maxValue(mv),maxLevel(ml),exp(e)
{
	req.resize(maxLevel + 1);
}

int Skill::expNeeded(int currentValue)
{
	return 250 * (currentValue+2) * (currentValue+1);
}

void Skill::setDefaults(Skill* skills)
{
	skills[SKILL_WEAPON_SPEED] = Skill("Swiftness", 0, 0, 10, 0);
	skills[SKILL_ATTACK] = Skill("Accuracy", 0, 0, 20, 0);
	skills[SKILL_DAMAGE] = Skill("Power", 0, 0, 20, 0);
	skills[SKILL_DEFENSE] = Skill("Evasion", 0, 0, 20, 0);
	skills[SKILL_ARMOR] = Skill("Armor", 0, 0, 10, 0);
	skills[SKILL_MOVE_SPEED] = Skill("Running", 0, 0, 10, 0);
	skills[SKILL_RANGED_ATTACK] = Skill("Aiming", 0, 0, 20, 0);
	skills[SKILL_HEALTH] = Skill("Endurance", 0, 0, 20, 0);
	skills[SKILL_MANA] = Skill("Arcana", 0, 0, 20, 0);
	skills[SKILL_MANA_REGEN] = Skill("Channeling", 0, 0, 20, 0);
	skills[SKILL_NEG_EFFECT] = Skill("Toughness", 0, 0, 10, 0);
	skills[SKILL_TRAPS] = Skill("Caution", 0, 0, 15, 0);
	skills[SKILL_ALCHEMY] = Skill("Alchemy", 0, 0, 15, 0);
	skills[SKILL_COOKING] = Skill("Cooking", 0, 0, 15, 0);
	skills[SKILL_BLACKSMITH] = Skill("Blacksmithing", 0, 0, 15, 0);
	
	// Requirements
	for (int i=1; i<=20; i++)
	{
		skills[SKILL_ATTACK].req[i].push_back(std::make_pair(ATTR_DEX, i));
		skills[SKILL_DAMAGE].req[i].push_back(std::make_pair(ATTR_STR, i));
		skills[SKILL_DEFENSE].req[i].push_back(std::make_pair(ATTR_DEX, i));
		skills[SKILL_RANGED_ATTACK].req[i].push_back(std::make_pair(ATTR_DEX, i));
		skills[SKILL_HEALTH].req[i].push_back(std::make_pair(ATTR_CON, i));
		skills[SKILL_MANA].req[i].push_back(std::make_pair(ATTR_INT, i));
		skills[SKILL_MANA_REGEN].req[i].push_back(std::make_pair(ATTR_INT, i));
	}
	for (int i=1; i<=10; i++)
	{
		skills[SKILL_ARMOR].req[i].push_back(std::make_pair(ATTR_CON, 2*i-1));
		skills[SKILL_WEAPON_SPEED].req[i].push_back(std::make_pair(ATTR_DEX, 2*i));
		skills[SKILL_MOVE_SPEED].req[i].push_back(std::make_pair(ATTR_CON, 2*i));
		skills[SKILL_NEG_EFFECT].req[i].push_back(std::make_pair(ATTR_CON, 2*i-1));
	}
	for (int i=1; i<=15; i++)
	{
		skills[SKILL_TRAPS].req[i].push_back(std::make_pair(ATTR_INT, i));
		skills[SKILL_TRAPS].req[i].push_back(std::make_pair(ATTR_DEX, i));
		skills[SKILL_ALCHEMY].req[i].push_back(std::make_pair(ATTR_INT, 4+i));
		skills[SKILL_COOKING].req[i].push_back(std::make_pair(ATTR_DEX, 4+i));
		skills[SKILL_BLACKSMITH].req[i].push_back(std::make_pair(ATTR_STR, 4+i));
	}
}