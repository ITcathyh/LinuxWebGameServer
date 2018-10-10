#pragma once
#include "sqlTem.h"
#include <vector>
#include "cJSON.h"

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
		else if(type == "裤子")
		{
			return 2;
		}
		else if(type == "鞋子")
		{
			return 3;
		}
		else if(type == "帽子")
		{
			return 4;
		}
		else if(type == "披风")
		{
			return 5;
		}
		else if(type == "武器")
		{
			return 6;
		}
		else if(type == "其他")
		{
			return 0;
		}
		
		return -1;
	}
};

class EquipmentDao
{
private:
	Equipment mappingEquipment(MYSQL_ROW sqlrow);
	Equipment mappingUserEquipment(MYSQL_ROW sqlrow);
	EquipmentDao()
	{
	}
public:
	Equipment getEquipmentById(long id);
	vector<Equipment> getByUserid(long usaerid);
	vector<Equipment> getUsedByUserid(long usaerid);
	bool addUserEquip(const long &equipid, const long &userid);
	bool updateUserUsedEquip(const long &userid, const long &oldEquipid, const long &newEquipid);
	int getMaxEquipId();
	
	static EquipmentDao* getInstance()
	{
		static EquipmentDao instance;
		return &instance;
	}
};

Equipment EquipmentDao::mappingEquipment(MYSQL_ROW sqlrow)
{
	Equipment equipment;
	equipment.equipmentid = atol(sqlrow[0]);
	equipment.name = sqlrow[1];
	equipment.detail = sqlrow[2];
	equipment.agility = atoi(sqlrow[3]);
	equipment.strength =  atoi(sqlrow[4]);
	equipment.HP =  atoi(sqlrow[5]);
	equipment.MP =  atoi(sqlrow[6]);
	equipment.type = atoi(sqlrow[7]);
		
	return equipment;
}


Equipment EquipmentDao::mappingUserEquipment(MYSQL_ROW sqlrow)
{
	Equipment equipment;
	equipment.equipmentid = atol(sqlrow[0]);
	equipment.name = sqlrow[1];
	equipment.detail = sqlrow[2];
	equipment.agility = atoi(sqlrow[3]);
	equipment.strength =  atoi(sqlrow[4]);
	equipment.HP =  atoi(sqlrow[5]);
	equipment.MP =  atoi(sqlrow[6]);
	equipment.type = atoi(sqlrow[7]);
	equipment.wear = atoi(sqlrow[8]);
		
	return equipment;
}

Equipment EquipmentDao::getEquipmentById(long id)
{
	string sqlstr = "select * from equipment where equipid  = " + getLongStr(id);
	char sql[sqlstr.length()];
	strcpy(sql, sqlstr.c_str());
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		Equipment e;
		e.equipmentid = 0;
		return e;
	}
	
	MYSQL_ROW sqlrow;
	Equipment e;
	e.equipmentid = 0;
	
	if ((sqlrow = mysql_fetch_row(res)))
	{
		e = mappingEquipment(sqlrow);
	}
	
	mysql_free_result(res);
	return e;
}

vector<Equipment> EquipmentDao::getByUserid(long userid)
{
	vector<Equipment> v;
	string sqlstr = "select equipment.*,wear from equipment natural join user_equip where userid = " + getLongStr(userid);
	char sql[sqlstr.length()];
	strcpy(sql, sqlstr.c_str());
	MYSQL_RES *res = query(sql);
		
	if (res == NULL)
	{
		return v;
	}
	
	MYSQL_ROW sqlrow;	
	
	while (sqlrow = mysql_fetch_row(res))
	{
		v.push_back(mappingUserEquipment(sqlrow));
	}
	
	mysql_free_result(res);
	return v;
}

vector<Equipment> EquipmentDao::getUsedByUserid(long userid)
{
	vector<Equipment> v;
	string sqlstr = "select equipment.*,wear from equipment natural join user_equip where wear = 1 and userid = " + getLongStr(userid);
	char sql[sqlstr.length()];
	strcpy(sql, sqlstr.c_str());
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		return v;
	}
	
	MYSQL_ROW sqlrow;	
	
	while ((sqlrow = mysql_fetch_row(res)))
	{
		v.push_back(mappingUserEquipment(sqlrow));
	}
	
	mysql_free_result(res);
	return v;
}

bool EquipmentDao::addUserEquip(const long &equipid, const long &userid)
{
	MYSQL *conn = connectMysql();    
	
	if (!conn)  
	{  
		return false;
	}  

	char sql[80];
	sprintf(sql, "insert into user_equip(userid, equipid) values(%ld, %ld)", userid, equipid);
	bool done =  !mysql_query(conn, sql);
	mysql_close(conn);
	
	return done; 
}

bool EquipmentDao::updateUserUsedEquip(const long &userid, const long &oldEquipid, const long &newEquipid)
{
	MYSQL *conn = connectMysql();    
	
	if (!conn)  
	{  
		return false;
	}  

	char sql[150];
	
	if (oldEquipid)
	{
		sprintf(sql,
			"update user_equip set wear = 0 where userid = %ld and equipid = %ld",
			userid,
			oldEquipid);
	
		if (!mysql_real_query(conn, sql, strlen(sql)))
		{
			long id = mysql_insert_id(conn);
		
			sprintf(sql,
				"update user_equip set wear = 1 where userid = %ld and equipid = %ld",
				userid,
				newEquipid);
			return !mysql_real_query(conn, sql, strlen(sql));
		}
	}
	else
	{
		sprintf(sql,
			"update user_equip set wear = 1 where userid = %ld and equipid = %ld",
			userid,
			newEquipid);
		
		return !mysql_real_query(conn, sql, strlen(sql));
	}
	
	
	return false;
}

int EquipmentDao::getMaxEquipId()
{
	string sqlstr = "select max(equipid) from equipment";
	char sql[sqlstr.length()];
	strcpy(sql, sqlstr.c_str());
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		return 0;
	}
	
	MYSQL_ROW sqlrow;	
	long result;
	
	if ((sqlrow = mysql_fetch_row(res)))
	{
		result =  atol(sqlrow[0]);
	}
	
	mysql_free_result(res);
	return result;
}