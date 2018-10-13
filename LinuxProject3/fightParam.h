#pragma once
#include <unistd.h>
#include "skillDao.h"
#include "fightAction.h"
class FightParam
{
protected:
	static const double ESCAPE_RATE_PER_AGILITY;
	//static FightAction *fightAction;
	
	enum
	{
		AGILITY_PER_LEVEL = 5,
		STRENGTH_PER_LEVEL = 5,
		ATN_PER_STRENGTH  = 1,
		XP_PER_MONSTER_LEVEL =  10,
		WAIT_TIME_PER_ATTACK = 100000
	};
	
	static void delay()
	{
		usleep(WAIT_TIME_PER_ATTACK);
	}

};

const double FightParam::ESCAPE_RATE_PER_AGILITY = 0.001;
//FightAction* FightParam::fightAction = FightAction::getInstance();