#pragma once
#include "myTask.h"
#include "requestReceiver.h"

class SessionTask : public MyTask
{
protected:
	int connfd;
public:
	SessionTask(int connfd1)
	{
		connfd = connfd1;
	}
	
	~SessionTask()
	{
	}
	
	void run()
	{
		static RqeuestReceiver *rqeuestReceiver = RqeuestReceiver::getInstance();
	
		rqeuestReceiver->receiveRequest(connfd);
	}
};