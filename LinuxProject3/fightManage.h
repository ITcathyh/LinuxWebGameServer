#pragma once
#include "mapFight.h"
#include "UserFight.h"
#include <pthread.h>
#include <ext/hash_map>
#include "websocketSessionManage.h"
#include "fightRoom.h"
#include "myThreadPool.h"
#include "userFightTask.h"

class FightManage
{
private:
	enum
	{
		MAX_WAIT_TIME = 5000000,
		CHECK_TIME = 50
	};
	hash_map<int, FightRoom*> fightRoomMap;
	WebSocketSessionManage *webSocketSessionManage;
	sem_t removeSem;
	sem_t idSem;
	int id;

	static void *userFightBegin(void *arg) {
		UserFight *userFight = (UserFight *)arg;
		short status;
		static const int waitTime = MAX_WAIT_TIME / CHECK_TIME;
		
		for (int i = 0; i < CHECK_TIME; ++i)
		{
			status = userFight->checkFightReadyStatus();
			
			if (status != 0)
			{
				break;
			}
			
			usleep(waitTime);
		}
		
		if (status > 0)
		{
			userFight->beginFight();
		}
		else
		{
			WebsocketSession *session1 = userFight->getSession1();
			WebsocketSession *session2 = userFight->getSession2();
			static WebSocketSessionManage *manage = WebSocketSessionManage::getInstance();
			
			if (manage->isSessionOpen(session1))
			{
				session1->setFight(false);
				response(session1->getConnfd(), getErrorJson("挑战异常").c_str());
			}

			if (manage->isSessionOpen(session2))
			{
				session2->setFight(false);
				response(session2->getConnfd(), getErrorJson("挑战异常").c_str());
			}
		}
		
		delete(userFight);
		pthread_exit(NULL);
	}
	
	void removeFightRoom(int &connfd)
	{
		sem_wait(&removeSem);
		
		FightRoom* room = fightRoomMap[connfd]; 
		
		if (room)
		{
			fightRoomMap.erase(connfd);
			//delete(room);
		}
		
		sem_post(&removeSem);
	}
	
	FightManage()
	{
		id = 0;
		sem_init(&idSem, 0, 1);
		sem_init(&removeSem, 0, 1);
		webSocketSessionManage = WebSocketSessionManage::getInstance();
	}
public:
	~FightManage()
	{
		while (sem_destroy(&idSem) == -1) ;
		while (sem_destroy(&removeSem) == -1) ;
	}
	
	void mapFight(long &mapid, WebsocketSession &session)
	{
		MapFight mapFight(mapid, session.getConnfd(), session.getUser());
		string result = mapFight.beginFight();
		int connfd = session.getConnfd();
	
		response(connfd, result.c_str());
		response(connfd, getUserInfoJson(session.getUser()).c_str());
	}
	
	bool createUserFight(string username, WebsocketSession *creator)
	{
		int connfd = creator->getConnfd();
		
		if (fightRoomMap[connfd])
		{
			return false;
		}
		
		WebsocketSession *oppoSession = webSocketSessionManage->getSessionByUsername(username);
		
		if (!oppoSession || oppoSession->isFighting() || !webSocketSessionManage->isSessionOpen(oppoSession))
		{
			return false;
		}
		
		sem_wait(&idSem);
		FightRoom *fightRoom = new FightRoom(creator, ++id);
		sem_post(&idSem);
		fightRoomMap[connfd] = fightRoom;

		response(oppoSession->getConnfd(), getNoticeUserFightJson(creator->getConnfd(), creator->getUser()).c_str());
		return true;
	}
	
	bool joinUserFight(int &creatorConnfd, WebsocketSession *session)
	{
		FightRoom *fightRoom = fightRoomMap[creatorConnfd];
		
		if (!fightRoom || !fightRoomMap[creatorConnfd])
		{
			response(session->getConnfd(), getErrorJson("挑战异常").c_str());
			return false;
		}
		
		//sem_t fightRoomSem = fightRoomSemMap[fightRoom->getId()];
		sem_t fightRoomSem = fightRoom->getSem();
		
		if (sem_wait(&fightRoomSem))
		{
			response(session->getConnfd(), getErrorJson("挑战异常").c_str());
			return false;
		}
		
		if (!fightRoomMap[creatorConnfd])
		{
			response(session->getConnfd(), getErrorJson("挑战异常").c_str());
			return false;
		}
		
		removeFightRoom(creatorConnfd);
		sem_post(&fightRoomSem);
		WebsocketSession *creatorSession = fightRoom->getSession();
		
		if (!webSocketSessionManage->isSessionOpen(creatorConnfd))
		{	
			response(session->getConnfd(), getJoinUserFightResultJson(false, creatorSession->getUser()->username).c_str());
			return false;
		}
		
		response(creatorConnfd, getJoinUserFightResultJson(true, session->getUser()->username).c_str());
		response(session->getConnfd(), getJoinUserFightResultJson(true, creatorSession->getUser()->username).c_str());
		return userFight(creatorSession, session);
	}
	
	void refuseUserFight(int &creatorConnfd, WebsocketSession *session)
	{
		FightRoom *fightRoom = fightRoomMap[creatorConnfd];
		string username = "none";
		
		if (!fightRoom || !fightRoomMap[creatorConnfd])
		{
			response(session->getConnfd(), getErrorJson("无效请求").c_str());
			return;
		}
		
		sem_t fightRoomSem = fightRoom->getSem(); 
		
		if (sem_wait(&fightRoomSem))
		{
			response(session->getConnfd(), getErrorJson("无效请求").c_str());
			return;
		}
		
		if (!fightRoomMap[creatorConnfd])
		{
			response(session->getConnfd(), getErrorJson("无效请求").c_str());
			return;
		}
		
		removeFightRoom(creatorConnfd);
		sem_post(&fightRoomSem);
		
		if (webSocketSessionManage->isSessionOpen(creatorConnfd))
		{
			WebsocketSession *creatorSession = fightRoom->getSession();
			username = creatorSession->getUser()->username;
			
			creatorSession->setFight(false);
			response(creatorConnfd, getJoinUserFightResultJson(false, session->getUser()->username, "refuse").c_str());
		}
	
		response(session->getConnfd(), getJoinUserFightResultJson(false, username, "refuse").c_str());
	}
	
	void cancelUserFight(string oppoUsernmae, WebsocketSession *session)
	{
		int creatorConnfd = session->getConnfd();
		FightRoom *fightRoom = fightRoomMap[creatorConnfd];
		WebsocketSession *oppoSession = webSocketSessionManage->getSessionByUsername(oppoUsernmae);
		
		if (!fightRoom || !fightRoomMap[creatorConnfd])
		{
			response(session->getConnfd(), getErrorJson("无效请求").c_str());
			return;
		}
		
		sem_t fightRoomSem = fightRoom->getSem();
		
		if (sem_wait(&fightRoomSem))
		{
			response(session->getConnfd(), getErrorJson("无效请求").c_str());
			return;
		}
		
		if (!fightRoomMap[creatorConnfd])
		{
			response(session->getConnfd(), getErrorJson("无效请求").c_str());
			return;
		}
		
		removeFightRoom(creatorConnfd);
		sem_post(&fightRoomSem);
		session->setFight(false);
		response(creatorConnfd, getCancelUserFightResultJson(true, oppoUsernmae).c_str());
	}

	bool userFight(WebsocketSession *session1, WebsocketSession *session2)
	{
		session1->setReadyToFight(false);
		session2->setReadyToFight(false);
		pthread_t thread;
		UserFight *userFight = new UserFight(session1, session2);
		MyThreadPool *myThreadPool = MyThreadPool::getInstance();
		MyTask *task = new UserFightTask(userFight);
		return myThreadPool->addTask(task) == 1;
		
		/*int err1 = pthread_create(&thread, NULL, userFightBegin, userFight);
	
		if (!err1)
		{
			pthread_detach(thread);	
			return true;
		}
		else
		{
			delete(userFight);
			return false;
		}
		*/
	}
	
	static FightManage* getInstance()
	{
		static FightManage instance;
		return &instance;
	}
};
