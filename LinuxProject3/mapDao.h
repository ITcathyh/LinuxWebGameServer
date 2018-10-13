#pragma once
#include "sqlTem.h"
#include <vector>
#include "myMap.h"


class MapDao
{
private:
	MyMap *mappingMap(MYSQL_ROW sqlrow);
	MapDao()
	{
	}
public:
	MyMap *getMapById(long id);
	vector<MyMap*> getAllMap();
	vector<MyMap*> getMapByMinRequiredLevel(short level);
	vector<MyMap*> getMapByRequiredLevel(short level);
	static MapDao* getInstance()
	{
		static MapDao instance;
		return &instance;
	}
};

MyMap *MapDao::mappingMap(MYSQL_ROW sqlrow)
{
	MyMap *map = new MyMap();
	map->mapid = atol(sqlrow[0]);
	map->mapname = sqlrow[1];
	map->detail = sqlrow[2];
		
	return map;
}

MyMap *MapDao::getMapById(long id)
{
	char sql[150];
	sprintf(sql, "select * from map where mapid = %ld", id);
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		return NULL;
	}
	
	MYSQL_ROW sqlrow;
	
	if ((sqlrow = mysql_fetch_row(res)))
	{
		return mappingMap(sqlrow);
	}
	
	return NULL;
}

vector<MyMap*> MapDao::getAllMap()
{
	vector<MyMap*> v;
	MYSQL_RES *res = query("select * from map");
	
	if (res == NULL)
	{
		return v;
	}
	
	MYSQL_ROW sqlrow;	
	
	while ((sqlrow = mysql_fetch_row(res)))
	{
		v.push_back(mappingMap(sqlrow));
	}
	
	return v;
}

vector<MyMap*> MapDao::getMapByMinRequiredLevel(short level)
{
	vector<MyMap*> v;
	char sql[100];
	sprintf(sql, "select * from map where requiredLevel<=%d", level);
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		return v;
	}
	
	MYSQL_ROW sqlrow;	
	
	while ((sqlrow = mysql_fetch_row(res)))
	{
		v.push_back(mappingMap(sqlrow));
	}
	
	return v;
}

vector<MyMap*> MapDao::getMapByRequiredLevel(short level)
{
	vector<MyMap*> v;
	char sql[100];
	sprintf(sql, "select * from map where requiredLevel=%d", level);
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		return v;
	}
	
	MYSQL_ROW sqlrow;	
	
	while ((sqlrow = mysql_fetch_row(res)))
	{
		v.push_back(mappingMap(sqlrow));
	}
	
	return v;
}