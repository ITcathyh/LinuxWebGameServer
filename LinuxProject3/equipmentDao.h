#pragma once
#include "sqlTem.h"
#include <vector>
#include "cJSON.h"
#include "equipment.h"

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
	bool isUserHaveEquip(long &equipid, long &userid);
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

bool EquipmentDao::isUserHaveEquip(long &equipid, long &userid)
{
	char sql[100];
	sprintf(sql, "select count(*) from user_equip where equipid=%ld and userid=%ld", equipid, userid);
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		return 0;
	}
	
	MYSQL_ROW sqlrow;	
	int result = 0;
	
	if ((sqlrow = mysql_fetch_row(res)))
	{
		result =  atoi(sqlrow[0]);
	}
	
	mysql_free_result(res);
	return result;
}
