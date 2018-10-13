#pragma once
#include "monsterDao.h"
#include "userDao.h"
#include "monsterDao.h"
#include <sys/timeb.h>
#include "JsonFactory.h"
#include "equipmentDao.h"
#include "websocketSendAction.h"
#include "userAction.h"
#include "fightParam.h"
#include "fightAction.h"
#include "getRandomNum.h"

class Fight : public FightParam
{
private:
	User user;
	Monster *monster;
	int connfd;
	int userAgility;
	int userMP;
	int userHP;
	
	void endFight(bool &isUserWin, short &monsterLevel)
	{
		int XP = 0;
		Equipment equip;
		bool getEquip = false;
		static WebSocketSessionManage *manage = WebSocketSessionManage::getInstance();
		WebsocketSession *session = manage->getSessionByUsername(user.username);
		
		if (isUserWin)
		{
			XP = XP_PER_MONSTER_LEVEL * monsterLevel;
			user.points += XP;
			EquipmentDao *dao = EquipmentDao::getInstance();
			UserDao *userDao = UserDao::getInstance();
			int range = dao->getMaxEquipId() * 2;
			struct timeb timeSeed;
			ftime(&timeSeed);
			srand(timeSeed.time * 1000 + timeSeed.millitm);
			int randomNum = (rand() % range) + 1;
			equip = dao->getEquipmentById(randomNum);
			
			UserAction::checkUserLevel(&user);
			
			if (userDao->updateUser(&user) && 
				!UserAction::isUserHaveEquip(equip.equipmentid, user.id))
			{
				dao->addUserEquip(equip.equipmentid, user.id);
				getEquip = true;
				equip.wear = false;
				session->getUserEquipList()->push_back(equip);
			}
		}
		
		if (manage->isSessionOpen(connfd))
		{
			session->setUser(user);
		}
		
		if (getEquip)
		{
			string get = "获得新装备:" + equip.name;
			response(connfd, getFightResultJson(isUserWin, XP, get).c_str());
			response(connfd, getAddEquipJson(equip).c_str());
		}
		else
		{
			response(connfd, getFightResultJson(isUserWin, XP).c_str());
		}
	}
public:
	Fight(User *user1, Monster *monster1, const int &confd)
	{
		connfd = confd;
		user = *user1;
		monster = monster1;
		userAgility = ESCAPE_RATE_PER_AGILITY * 1000 * (user.agility + user.attachAgility);
		userHP = user.HP;
		userMP = user.MP;
		monster->agility *= ESCAPE_RATE_PER_AGILITY * 1000;
		monster->strength *= ATN_PER_STRENGTH;
		response(connfd, getMonsterJson(*monster1).c_str());
	}
	
	bool fighting(short &monsterLevel)
	{
		bool end = false;
		bool userWin = false;
		//struct timeb timeSeed;
	//	ftime(&timeSeed);
		//srand(timeSeed.time * 1000 + timeSeed.millitm);
		int range = 1001;
		int userAssault = 0;
		int userStrength = user.strength + user.attachStrength;
		string skill;
		static FightAction *fightAction = FightAction::getInstance();
		static GetRandomNum *randomNum = GetRandomNum::getInstance();
		
		while (!end)
		{
			//int randomRate = rand() % range;
			int randomRate = randomNum->getRandomnum();
			bool userMiss = false, monsterMiss = false;
		
			if (randomRate > monster->agility)
			{
				userAssault = fightAction->getAttackMode(skill, user.level, user.strength);
				monster->HP -= userAssault;
		
				if (monster->HP <= 0)
				{
					monster->HP = 0;
					userWin = true;
					end = true;
				}
			}
			else
			{
				userMiss = true;
			}
		
			if (!end && randomRate > userAgility)
			{
				userHP -= monster->strength;
		
				if (userHP <= 0)
				{
					userHP = 0;
					end = true;
				}
			}
			else
			{
				monsterMiss = true;
			}
		
			string jsonstr = getFightDetailJson(userHP,
				userAssault,
				userMiss,
				monster->HP,
				monster->strength,
				monsterMiss,skill);
			
			response(connfd, jsonstr.c_str());
			delay();
		}
		
		endFight(userWin, monsterLevel);
		return userWin;
	}
};

class MapFight
{
private:
	vector<Monster*> monsterList;
	int connfd;
	User *user;
public:
	MapFight(const long &mapid, const int &connfd1, User *user1)
	{
		MonsterDao *monsterDao = MonsterDao::getInstance();
		monsterList = monsterDao->getByMapid(mapid);
		connfd = connfd1;
		user = user1;
	}

	string beginFight()
	{
		int i = 0;
		int len = monsterList.size();
		bool userWin = true;
		
		for (; i < len; ++i)
		{
			Fight fight(user, monsterList[i], connfd);
			
			if (!fight.fighting(monsterList[i]->level))
			{
				userWin = false;
				break;
			}
		}
		
		return getMapFightResultJson(userWin);
	}
};