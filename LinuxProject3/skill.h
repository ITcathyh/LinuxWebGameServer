#pragma once
#include <string>
using namespace std;

class Skill
{
public:
	long skillid;
	string name;
	float ratio;
	int requiredLevel;
	short probability;
	
	Skill(const Skill &skill)
	{
		skillid = skill.skillid;
		name = skill.name;
		ratio = skill.ratio;
		requiredLevel = skill.requiredLevel;
		probability = skill.probability;
	}
	
	Skill()
	{
	}
	
	Skill& operator=(const Skill &skill)
	{
		this->skillid = skill.skillid;
		this->name = skill.name;
		this->ratio = skill.ratio;
		this->requiredLevel = skill.requiredLevel;
		this->probability = skill.probability;
		
		return *this;
	}
};