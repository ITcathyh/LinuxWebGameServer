#pragma once
#include "userDao.h"
#include "equipmentDao.h"
#include <semaphore.h>
#include "websocketSessionManage.h"
#include "mapDao.h"
#include "JsonFactory.h"
#include "websocketSendAction.h"

class UserAction
{
private:
	enum {
		POINST_PER_LEVEl = 50,
		ADD_HP_PER_LEVEL = 20,
		ADD_MP_PER_LEVEL = 20,
		ADD_AGILITY_PER_LEVEL = 2,
		ADD_STRENGTH_PER_LEVEL = 5
	};
	static bool isUpdate(const int &points)
	{
		return points >= POINST_PER_LEVEl;
	}

	static int computeAddLevel(int &points)
	{
		int addLevel = points / POINST_PER_LEVEl;
		points %= POINST_PER_LEVEl;
	
		return addLevel;
	}

	static void update(User *user, const int &addLevel)
	{
		user->level += addLevel;
		user->agility += addLevel * ADD_AGILITY_PER_LEVEL;
		user->strength += addLevel * ADD_STRENGTH_PER_LEVEL;
		user->HP += addLevel * ADD_HP_PER_LEVEL;
		user->MP += addLevel * ADD_MP_PER_LEVEL;
	}
public:
	static void checkUserLevel(User *user)
	{
		if (isUpdate(user->points))
		{
			int addLevel = computeAddLevel(user->points);
			WebSocketSessionManage *manage = WebSocketSessionManage::getInstance();
			int connfd = manage->getConnfdByUsername(user->username);
			update(user, addLevel);
			
			if (connfd && manage->isSessionOpen(connfd))
			{
				MapDao* mapDao = MapDao::getInstance();
				response(connfd, getMapListJson(mapDao->getMapByRequiredLevel(user->level), "newMapList").c_str());
			}
		}
	}
	
	static bool isUserHaveEquip(long &equipid, long &userid)
	{
		if (equipid <= 0)
		{
			return true;
		}
		
		static EquipmentDao *dao = EquipmentDao::getInstance();
		
		return dao->isUserHaveEquip(equipid, userid);
	}

	static void wearEquip(sem_t *updateEquipSem, User *user, Equipment *newEquip, Equipment *oldEquip = NULL)
	{
		EquipmentDao *dao = EquipmentDao::getInstance();
		
		sem_wait(updateEquipSem);
		
		if (newEquip->wear || (oldEquip && !oldEquip->wear))
		{
			sem_post(updateEquipSem);
			return;
		}
		
		newEquip->wear = true;
		
		if (oldEquip)
		{
			dao->updateUserUsedEquip(user->id, oldEquip->equipmentid, newEquip->equipmentid);
			user->attachAgility += newEquip->agility - oldEquip->agility;
			user->attachStrength += newEquip->strength - oldEquip->strength;
			user->attachHP += newEquip->HP - oldEquip->HP;
			user->attachMP += newEquip->MP - oldEquip->MP;
			oldEquip->wear = false;
		}
		else
		{
			dao->updateUserUsedEquip(user->id, 0, newEquip->equipmentid);
			user->attachAgility += newEquip->agility;
			user->attachStrength += newEquip->strength;
			user->attachHP += newEquip->HP;
			user->attachMP += newEquip->MP;
		}
	
		sem_post(updateEquipSem);
	}
};