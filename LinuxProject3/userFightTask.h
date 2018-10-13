#pragma once
#include "myTask.h"
#include "UserFight.h"

class UserFightTask : public MyTask
{
private:
	UserFight *userFight;
	
	enum
	{
		MAX_WAIT_TIME = 5000000,
		CHECK_TIME    = 50
	};
public:
	UserFightTask(UserFight *userFight1)
	{
		userFight = userFight1;
	}
	
	~UserFightTask()
	{
		delete userFight;
	}
	
	void run()
	{
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
	}
};