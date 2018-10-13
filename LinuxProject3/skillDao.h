#pragma once
#include <string>
#include <vector>
#include "mysql.h"
#include "sqlTem.h"
#include "skill.h"

class SkillDao
{
private:
	Skill *mappingSkill(MYSQL_ROW sqlrow);
	SkillDao()
	{
	}
public:
	vector<Skill*> getAll();
	
	static SkillDao* getInstance()
	{
		static SkillDao instance;
		return &instance;
	}
};


Skill *SkillDao::mappingSkill(MYSQL_ROW sqlrow)
{
	Skill *skill = new Skill();
	skill->skillid = atol(sqlrow[0]);
	skill->name = sqlrow[1];
	skill->ratio = atof(sqlrow[2]);
	skill->requiredLevel = atol(sqlrow[3]);
	skill->probability = atol(sqlrow[4]);
	
	return skill;
}

vector<Skill*> SkillDao::getAll()
{
	char *sql = "select * from skill order by requiredLevel";
	MYSQL_RES *res = query(sql);
	vector<Skill*> v;
	
	if (res == NULL)
	{
		return v;
	}
	
	MYSQL_ROW sqlrow;
	
	while (sqlrow = mysql_fetch_row(res))
	{
		v.push_back(mappingSkill(sqlrow));
	}
	
	mysql_free_result(res);
	return v;
}