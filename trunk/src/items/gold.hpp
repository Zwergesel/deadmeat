#ifndef _GOLD_HPP
#define _GOLD_HPP

#include "../item.hpp"

class Savegame;
class LoadBlock;

class Gold : public Item
{
public:
	Gold();
	Gold(int amount);
	~Gold();
	Item* clone();
	void randomize(int level);

	virtual unsigned int save(Savegame& sg);
	virtual void load(LoadBlock& load);
};

#endif