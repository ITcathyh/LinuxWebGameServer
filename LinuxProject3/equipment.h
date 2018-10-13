#pragma once
#include <string>
#include "cJSON.h"
using namespace std;

class Equipment
{
private:
	static string convertEquipTypeToString(const int &type)
	{
		switch (type)
		{
		case 1:return "上衣";
		case 2:return "裤子";
		case 3:return "鞋子";
		case 4:return "帽子";
		case 5:return "披风";
		case 6:return "武器";
		default:return "其他";
		}
	}
	
public:
	long equipmentid;
	string name;
	string detail;
	short agility;
	short strength;
	int HP;
	int MP;
	int type;
	bool wear;
	
	Equipment(const Equipment &equip)
	{
		equipmentid = equip.equipmentid;
		name = equip.name;
		detail = equip.detail;
		agility = equip.agility;
		strength = equip.strength;
		HP = equip.HP;
		MP = equip.MP;
		type = equip.type;
		wear = equip.wear;
	}
	
	Equipment()
	{
	}
	
	Equipment& operator=(const Equipment &equip)
	{
		this->equipmentid = equip.equipmentid;
		this->name = equip.name;
		this->detail = equip.detail;
		this->agility = equip.agility;
		this->strength = equip.strength;
		this->HP = equip.HP;
		this->MP = equip.MP;
		this->type = equip.type;
		this->wear = equip.wear;
		
		return *this;
	}
	
	cJSON *getJson()const
	{
		cJSON *tmp = cJSON_CreateObject();
		
		cJSON_AddNumberToObject(tmp, "equipmentid", this->equipmentid);
		cJSON_AddStringToObject(tmp, "name", this->name.c_str());
		cJSON_AddStringToObject(tmp, "detail", this->detail.c_str());
		cJSON_AddNumberToObject(tmp, "agility", this->agility);
		cJSON_AddNumberToObject(tmp, "strength", this->strength);
		cJSON_AddNumberToObject(tmp, "HP", this->HP);
		cJSON_AddNumberToObject(tmp, "MP", this->MP);
		cJSON_AddStringToObject(tmp, "type", convertEquipTypeToString(this->type).c_str());
		cJSON_AddNumberToObject(tmp, "wear", this->wear);
		
		return tmp;
	}
	
	static short convertEquipTypeToNumber(const string &type)
	{
		if (type == "上衣")
		{
			return 1;
		}
		else if (type == "裤子")
		{
			return 2;
		}
		else if (type == "鞋子")
		{
			return 3;
		}
		else if (type == "帽子")
		{
			return 4;
		}
		else if (type == "披风")
		{
			return 5;
		}
		else if (type == "武器")
		{
			return 6;
		}
		else if (type == "其他")
		{
			return 0;
		}
		
		return -1;
	}
};