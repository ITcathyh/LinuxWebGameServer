#pragma once
#include <iostream>
#include <ext/hash_map>
#include <string>
#include "websocketSession.h"
using namespace std;
using namespace __gnu_cxx;

namespace __gnu_cxx{
	template<> struct hash<string>
	{
		size_t operator()(const string& s) const
		{
			return __stl_hash_string(s.c_str());
		}
	};
}

class WebSocketSessionManage
{
private:
	hash_map<string, int> userConnfdMap;
	hash_map<int, WebsocketSession*> connfdSessionMap;
	
	WebSocketSessionManage()
	{
	}
public:
	bool isSessionOpen(const int &connfd)
	{
		return connfdSessionMap.count(connfd) == 1 && !connfdSessionMap[connfd]->isClose();
	}
	
	bool isSessionOpen(WebsocketSession* session)
	{
		if (!session || session->isClose())
		{
			return false;
		}
		
		return connfdSessionMap.count(session->getConnfd()) == 1;
	}
	
	void addSession(const int connfd, WebsocketSession* session)
	{
		connfdSessionMap[connfd] = session;
	}
	
	void updateSession(const int connfd, const string username)
	{
		userConnfdMap[username] = connfd;
	}
	
	void removeSession(const int &connfd)
	{
		connfdSessionMap.erase(connfd);
		
		for (hash_map<string, int>::iterator it = userConnfdMap.begin(); it != userConnfdMap.end();it++)
		{
			if (it->second == connfd)
			{
				userConnfdMap.erase(it);
				break;
			}
		}
	}
	
	static WebSocketSessionManage* getInstance()
	{
		static WebSocketSessionManage instance;
		return &instance;
	}
	
	WebsocketSession* getSessionByUsername(string username)
	{
		return connfdSessionMap[userConnfdMap[username]];
	}
	
	int getConnfdByUsername(string username)
	{
		return userConnfdMap[username];
	}
	
	hash_map<int, WebsocketSession*>* getAllOnlineUserMap()
	{
		return &connfdSessionMap;
	}
	
	bool isUserOnline(string &username)
	{
		return userConnfdMap[username];
	}
};