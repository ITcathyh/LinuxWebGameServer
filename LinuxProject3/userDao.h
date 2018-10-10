#pragma once
#include "sqlTem.h"
#include "baseFightUnit.h"

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

class UserDao
{
private:
	User *mappingUser(MYSQL_ROW sqlrow);
	UserDao()
	{
	}
public:
	User *getUserByUsernameAndPassword(string username, string password);
	bool updateUser(const User *user);
	
	static UserDao* getInstance()
	{
		static UserDao instance;
		return &instance;
	}
};


User *UserDao::mappingUser(MYSQL_ROW sqlrow)
{
	User *user = new User();
	user->id = atol(sqlrow[0]);
	user->username=sqlrow[1];
	user->password=sqlrow[2];
	user->level = atoi(sqlrow[3]);
	user->points = atoi(sqlrow[4]);
	user->agility = atoi(sqlrow[5]);
	user->strength = atoi(sqlrow[6]);
	user->HP = atoi(sqlrow[7]);
	user->MP = atoi(sqlrow[8]);
	user->attachAgility = 0;
	user->attachStrength = 0;
	user->attachHP = 0;
	user->attachMP = 0;
	
	return user;
}

User *UserDao::getUserByUsernameAndPassword(string username, string password)
{
	char sql[150];
	sprintf(sql, "select * from user where username = '%s' and password = '%s'", username.c_str(), password.c_str());
	MYSQL_RES *res = query(sql);
	
	if (res == NULL)
	{
		return NULL;
	}
	
	MYSQL_ROW sqlrow;
	User *user = NULL;
	
	if ((sqlrow = mysql_fetch_row(res)))
	{
		user =  mappingUser(sqlrow);
	}
	
	mysql_free_result(res);
	return user;
}

bool UserDao::updateUser(const User *user)
{
	MYSQL *conn = connectMysql();    
	
	if (!conn)  
	{  
		return false;
	}  

	char sql[250];
	sprintf(sql,
		"update user set level = %d,points = %d, agility=%d,strength=%d,HP=%d,MP=%d where userid = %ld",
		user->level,
		user->points,
		user->agility,
		user->strength,
		user->HP,
		user->MP,
		user->id);
	bool done = !mysql_query(conn, sql);
	
	mysql_close(conn);
	return done;
}
