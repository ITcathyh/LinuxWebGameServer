#pragma once
#include <string>
#include "baseFightUnit.h"
using namespace std;

class User : public BaseFightUnit
{
public:
	long id;
	string username;
	string password;
	int points;
	short attachAgility;
	short attachStrength;
	int attachHP;
	int attachMP;
	
	User(const User &user)
	{
		id = user.id;
		username = user.username;
		password = user.password;
		level = user.level;
		points = user.points;
		agility = user.agility;
		strength = user.strength;
		HP = user.HP;
		MP = user.MP;
		attachAgility = user.attachAgility;
		attachStrength = user.attachStrength;
		attachHP = user.attachHP;
		attachMP = user.attachMP;
	}
	
	User()
	{
	}
	
	User& operator=(const User &user)
	{
		this->id = user.id;
		this->username = user.username;
		this->password = user.password;
		this->level = user.level;
		this->points = user.points;
		this->agility = user.agility;
		this->strength = user.strength;
		this->HP = user.HP;
		this->MP = user.MP;
		this->attachAgility = user.attachAgility;
		this->attachStrength = user.attachStrength;
		this->attachHP = user.attachHP;
		this->attachMP = user.attachMP;
		
		return *this;
	}
};