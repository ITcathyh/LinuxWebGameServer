#pragma once
#include "sqlTem.h"
#include <vector>
#include "baseFightUnit.h"

class Monster : public BaseFightUnit
{
public:
	long monsterid;
	string monstername;
};

class MonsterDao
{
private:
	Monster *mappingMonster(MYSQL_ROW sqlrow);
	MonsterDao()
	{
	}
public:
	Monster *getMonsterById(long id);
	vector<Monster*> getByMapid(long mapid);
	
	static MonsterDao* getInstance()
	{
		static MonsterDao instance;
		return &instance;
	}
};

Monster *MonsterDao::mappingMonster(MYSQL_ROW sqlrow)
{
	Monster *monster = new Monster();
	monster->monsterid = atol(sqlrow[0]);
	monster->monstername = sqlrow[1];
	monster->agility = atoi(sqlrow[2]);
	monster->strength = atoi(sqlrow[3]);
	monster->level = atoi(sqlrow[4]);
	monster->HP = atoi(sqlrow[5]);
		
	return monster;
}

Monster *MonsterDao::getMonsterById(long id)
{
	string sqlstr = "select * from monster where monsterid = " + getLongStr(id);
	char sql[sqlstr.length()];
	strcpy(sql, sqlstr.c_str());
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		return NULL;
	}
	
	MYSQL_ROW sqlrow;
	
	if ((sqlrow = mysql_fetch_row(res)))
	{
		return mappingMonster(sqlrow);
	}
	
	return NULL;
}

vector<Monster*> MonsterDao::getByMapid(long mapid)
{
	vector<Monster*> v;
	string sqlstr = "select monster.* from monster natural join map_monster where mapid = " + getLongStr(mapid);
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
		v.push_back(mappingMonster(sqlrow));
	}
	
	return v;
}