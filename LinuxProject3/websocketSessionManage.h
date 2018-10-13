#pragma once
#include <iostream>
#include <ext/hash_map>
#include <string>
#include "websocketSession.h"
#include <sys/stat.h>
#include <fcntl.h> 
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
	
	bool checkFd(int &connfd) {  
		struct stat _stat;  
		
		if (!fcntl(connfd, F_GETFL)) {  
			if (!fstat(connfd, &_stat)) {  
				if (_stat.st_nlink >= 1)
				{
					return true;
				}
				else
				{
					return false;
				}
			}  
		}  
		
		return false;  
	}  
	
	WebSocketSessionManage()
	{
	}
public:
	bool isSessionOpen(int &connfd)
	{
		return connfdSessionMap.count(connfd) == 1 &&
			!connfdSessionMap[connfd]->isClose();
	}
	
	bool isSessionOpen(WebsocketSession* session)
	{
		if (!session || session->isClose())
		{
			return false;
		}
		
		int connfd = session->getConnfd();
		
		return connfdSessionMap.count(connfd) == 1;
	}
	
	bool isSessionValid(int &connfd)
	{
		WebsocketSession* session = connfdSessionMap[connfd];
		
		return session && !session->isClose() &&
			session->isLogin() && checkFd(connfd);
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