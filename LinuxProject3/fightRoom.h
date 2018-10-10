#pragma once
#include "websocketSessionManage.h"
#include <semaphore.h>

class FightRoom
{
private:
	WebsocketSession *creator;
	int id;
	sem_t fightRoomSem;
public:
	FightRoom(WebsocketSession *c, int id1)
	{
		creator = c;
		id = id1;
		
		sem_init(&fightRoomSem, 0, 1);
	}
	
	~FightRoom()
	{
		while (sem_destroy(&fightRoomSem) == -1) ;
	}
	
	WebsocketSession* getSession()
	{
		return creator;
	}
	
	int& getId()
	{
		return id;
	}
	
	sem_t getSem()
	{
		return fightRoomSem;
	}
};