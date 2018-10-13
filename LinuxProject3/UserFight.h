#pragma once
#include "userDao.h"
#include "JsonFactory.h"
#include "userAction.h"
#include "websocketSendAction.h"
#include "websocketSession.h"
#include <cstdlib>
#include <ctime>
#include <sys/timeb.h>
#include <semaphore.h>
#include "websocketSessionManage.h"
#include "fightParam.h"
#include "fightAction.h"

class UserFight : public FightParam
{
private:
	WebsocketSession *session1;
	WebsocketSession *session2;
	User user1;
	User user2;
	
	void endFight(const bool &firstUserWin)
	{
		int XP1 = 0, XP2 = 0;
		static UserDao *userDao = UserDao::getInstance();
		static WebSocketSessionManage *sessionManage = WebSocketSessionManage::getInstance();
		
		if (firstUserWin)
		{
			XP1 = XP_PER_MONSTER_LEVEL * user2.level;
			user1.points += XP1;
			
			UserAction::checkUserLevel(&user1);
			userDao->updateUser(&user1);
		}
		else
		{
			XP2 = XP_PER_MONSTER_LEVEL * user1.level;
			user2.points += XP2;
			
			UserAction::checkUserLevel(&user2);
			userDao->updateUser(&user2);
		}
		
		if (sessionManage->isSessionOpen(session1))
		{
			response(session1->getConnfd(), getFightResultJson(firstUserWin, XP1).c_str());
			session1->setUser(user1);
			session1->setFight(false);
			response(session1->getConnfd(), getUserInfoJson(&user1).c_str());
		}
		
		if (sessionManage->isSessionOpen(session2))
		{
			response(session2->getConnfd(), getFightResultJson(!firstUserWin, XP2).c_str());
			session2->setUser(user2);
			session2->setFight(false);
			response(session2->getConnfd(), getUserInfoJson(&user2).c_str());
		}
	}
	
	void fighting()
	{
		static FightAction *fightAction = FightAction::getInstance();
		bool end = false;
		bool firstWin = false;
	//	struct timeb timeSeed;
		//ftime(&timeSeed);
		//srand(timeSeed.time * 1000 + timeSeed.millitm);
		int range = 1001;
		int hp1 = user1.HP + user1.attachHP;
		int hp2 = user2.HP + user2.attachHP;
		int userAgility1 = ESCAPE_RATE_PER_AGILITY * 1000 * (user1.agility + user1.attachAgility);
		short userStrength1 = user1.strength + user1.attachStrength;
		int userAgility2 = ESCAPE_RATE_PER_AGILITY * 1000 * (user2.agility + user2.attachAgility);
		short userStrength2 = user2.strength + user2.attachStrength;
		int userAssault1, userAssault2;
		string skill1, skill2;
		static GetRandomNum *randomNum = GetRandomNum::getInstance();
		
		while (!end)
		{
			int randomRate = randomNum->getRandomnum();
			bool firstMiss = false, nextMiss = false;
		
			if (randomRate > userAgility2)
			{
				userAssault1 = fightAction->getAttackMode(skill1, user1.level, userStrength1);
				hp2 -= userAssault1;
		
				if (hp2 <= 0)
				{
					hp2 = 0;
					firstWin = true;
					end = true;
				}
			}
			else
			{
				firstMiss = true;
			}
		
			if (!end && randomRate > userAgility1)
			{
				
				userAssault2 = fightAction->getAttackMode(skill2, user2.level, userStrength2);
				hp1 -= userAssault2;
		
				if (hp1 <= 0)
				{
					hp1 = 0;
					end = true;
				}
			}
			else
			{
				nextMiss = true;
			}
		
			noticeFightDetail(firstMiss, nextMiss, 
				hp1, hp2,
				userAssault1,userAssault2,
				skill1,skill2);
			delay();
		}
		
		endFight(firstWin);
	}
	
	void noticeFightDetail(bool &firstMiss, bool &nextMiss,
		int &hp1, int &hp2,
		int &userAssault1, int &userAssault2,
		string skill1, string skill2)
	{
		const int &connfd1 = session1->getConnfd();
		const int &connfd2 = session2->getConnfd();
		string jsonstr = getFightDetailJson(hp1,
			userAssault1,
			firstMiss,
			hp2,
			userAssault2,
			nextMiss,
			skill1,
			skill2);
		response(connfd1, jsonstr.c_str());
		jsonstr = getFightDetailJson(hp2,
			userAssault2,
			nextMiss,
			hp1,
			userAssault1,
			firstMiss,
			skill2,
			skill1);
		response(connfd2, jsonstr.c_str());
	}
public:
	UserFight(WebsocketSession *s1,
		WebsocketSession *s2)
	{
		if (s1->getUser()->agility >= s2->getUser()->agility)
		{
			session1 = s1;
			user1 = *(s1->getUser());
			session2 = s2;
			user2 = *(s2->getUser());
		}
		else
		{
			session1 = s2;
			user1 = *(s2->getUser());
			session2 = s1;
			user2 = *(s1->getUser());
		}
	}
	
	void beginFight()
	{
		fighting();
	}
	
	WebsocketSession *getSession1()
	{
		return session1;
	}
	
	WebsocketSession *getSession2()
	{
		return session2;
	}
	
	short checkFightReadyStatus()
	{
		static WebSocketSessionManage *manage = WebSocketSessionManage::getInstance();
		
		if (manage->isSessionOpen(session1))
		{
			if (!session1->isReadyToFight())
			{
				return 0;
			}
		}
		else
		{
			return -1;
		}
		
		if (manage->isSessionOpen(session2))
		{
			if (!session2->isReadyToFight())
			{
				return 0;
			}
		}
		else
		{
			return -1;
		}
		
		return 1;
	}
};