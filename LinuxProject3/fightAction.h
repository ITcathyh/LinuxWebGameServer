#pragma once
#include "skillDao.h"
#include <iostream>
#include <sys/timeb.h>
#include <cstdlib>

class FightAction
{
private:
	enum
	{
		RANGE        = 1000,
		NORMAL_RATIO = 4
	};
	const double ATN_PER_STRENGTH;
	vector<Skill*> skillList;
	int skillArray[RANGE];
	
	void initSkillArray()
	{
		struct timeb timeSeed;
		
		ftime(&timeSeed);
		srand(timeSeed.time * 1000 + timeSeed.millitm);
		memset(skillArray, -1, sizeof(skillArray));

		int randomNum = rand() % RANGE;
		
		for (int i = 0, len = skillList.size(); i < len; ++i)
		{
			for (int j = 0, pro = skillList[i]->probability; j < pro; ++j)
			{
				while (skillArray[randomNum] != -1)
				{
					randomNum = rand() % RANGE;
				}
			
				skillArray[randomNum] = i;
			}
		}
	}
	
	FightAction() : ATN_PER_STRENGTH(1.0)
	{
		SkillDao *dao = SkillDao::getInstance();
		skillList = dao->getAll();
		initSkillArray();
	}
public:
	static FightAction *getInstance()
	{
		static FightAction instance;
		return &instance;
	}
	
	~FightAction()
	{
		//delete[] skillArray;
	}
	
	int getAttackMode(string &skillStr, const short &userLevel, const short &userStrength)
	{
		struct timeb timeSeed;
		//ftime(&timeSeed);
		//srand(timeSeed.time * 1000 + timeSeed.millitm);
		int randomNum;
		Skill *skill;
		
		while (1)
		{
			randomNum = rand() % RANGE;
			const int &skillIndex = skillArray[randomNum];
			
			if (skillIndex == -1)
			{
				skillStr = "普通攻击";
				return userStrength * ATN_PER_STRENGTH;
			}
			else
			{
				skill = skillList[skillIndex];
				
				if (skill->requiredLevel > userLevel)
				{
					continue;
				}
				
				skillStr = skill->name;
				return userStrength * skill->ratio * ATN_PER_STRENGTH;
			}
		}
	}
};