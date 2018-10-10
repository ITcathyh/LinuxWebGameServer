#pragma once
#include "websocketSessionManage.h"
#include "JsonFactory.h"
#include "websocketSendAction.h"
#include <pthread.h>

class NoticeOnlineUserAction
{
private:
	WebSocketSessionManage *webSocketSessionManage;
	NoticeOnlineUserAction()
	{
		webSocketSessionManage = WebSocketSessionManage::getInstance();
	}
	
	void notice(string &username, const char *str)
	{
		hash_map<int, WebsocketSession*>* connfdSessionMap = webSocketSessionManage->getAllOnlineUserMap();
		
		for (hash_map<int, WebsocketSession*>::iterator it = connfdSessionMap->begin(); it != connfdSessionMap->end(); it++)
		{	
			int connfd = it->first;
			
			if (webSocketSessionManage->isSessionOpen(connfd))
			{
				User *user = it->second->getUser();
				
				if (user && user->username != username)
				{
					response(connfd, str);
				}
			}
		}
	}
public:
	static NoticeOnlineUserAction* getInstance()
	{
		static NoticeOnlineUserAction instance;
		return &instance;
	}
	
	void noticeAllOnlineUser(int &connfd, string &selfname)
	{
		response(connfd, getNoticeAllOnlineUserJson(selfname).c_str());
	}

	void noticeNewOnlineUser(string &username, short &level)
	{
		notice(username, getNoticeNewOnlineUserJson(username, level).c_str());
	}
	
	void noticeNewOfflineUser(string &username)
	{
		notice(username, getNoticeNewOfflineUserJson(username).c_str());
	}
};