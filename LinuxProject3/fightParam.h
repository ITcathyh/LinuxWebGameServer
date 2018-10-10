#pragma once
class FightParam
{
protected:
	static const double ESCAPE_RATE_PER_AGILITY;
	
	enum
	{
		AGILITY_PER_LEVEL = 5,
		STRENGTH_PER_LEVEL = 5,
		ATN_PER_STRENGTH  = 1,
		XP_PER_MONSTER_LEVEL =  10
	};
	
	void delay()
	{
	}
};

const double FightParam::ESCAPE_RATE_PER_AGILITY = 0.001;