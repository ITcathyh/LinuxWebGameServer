#pragma once
#include <iostream>
#include "userDao.h"
#include "equipmentDao.h"
#include <semaphore.h>
#include <unistd.h>

class WebsocketSession
{
private:
	bool fighting;
    int connfd;
	User *user;
	vector<Equipment> userEquipList;
	sem_t setUserSem;
	sem_t updateEquipSem;
	bool readyToFight;
public:
	WebsocketSession(int &confd)
	{
		connfd = confd;
		user = NULL;
		fighting = false;
		
		sem_init(&setUserSem, 0, 1);
		sem_init(&updateEquipSem, 0, 1);
	}
	
	~WebsocketSession()
	{
		if (user)
		{
			delete(user);
			user = NULL;
		}
		
		while (sem_destroy(&setUserSem) == -1)
		{
			usleep(100);
		}
		
		while (sem_destroy(&updateEquipSem) == -1)
		{
			usleep(100);
		}
	}
	
	void close()
	{
		connfd = 0;
	}
	
	int getConnfd()
	{
		return connfd;
	}
	
	bool isClose()
	{
		return !connfd;
	}
	
	bool isLogin()const
	{
		return user;
	}
	
	User *getUser()
	{
		sem_wait(&setUserSem);
		User *u = user;
		sem_post(&setUserSem);
		return u;
	}
	
	void setUser(User *user1)
	{
		sem_wait(&setUserSem);
		
		if (user)
		{
			delete(user);
		}
		
		user = user1;
		sem_post(&setUserSem);
	}
	
	void setUser(User user1)
	{
		sem_wait(&setUserSem);
		
		if (user)
		{
			delete(user);
		}
		
		user = new User(user1);
		sem_post(&setUserSem);
	}
	
	vector<Equipment> *getUserEquipList()
	{
		return &userEquipList;
	}
	
	void setUserEquipList(vector<Equipment> list)
	{
		userEquipList = list;
		
		for (int i = 0, len = list.size(); i < len; ++i)
		{
			if (list[i].wear)
			{
				user->attachAgility += list[i].agility;
				user->attachStrength += list[i].strength;
				user->attachHP += list[i].HP;
				user->attachMP += list[i].MP;
			}
		}
	}
	
	void setFight(bool f)
	{
		fighting = f;
	}
	
	bool isFighting()
	{
		return fighting;
	}
	
	bool isReadyToFight()
	{
		return readyToFight;
	}

	void setReadyToFight(bool ready)
	{
		readyToFight = ready;
	}
	
	sem_t* getupdateEquipSem()
	{
		return &updateEquipSem;
	}
};