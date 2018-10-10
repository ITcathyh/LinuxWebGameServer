#pragma once
#include <iostream>
#include "cJSON.h"
#include "websocketSession.h"
#include "userDao.h"
#include "JsonFactory.h"
#include "equipmentDao.h"
#include "mapDao.h"
#include "fightAction.h"
#include "userAction.h"
#include "websocketSessionManage.h"
#include "noticeOnlineUserAction.h"

using namespace std;

class RequestHanlder
{
private:
	UserDao *userdao;
	EquipmentDao *equipDap;
	MapDao *mapDao;
	NoticeOnlineUserAction *noticeAction;
	
	RequestHanlder()
	{
		userdao = UserDao::getInstance();
		equipDap = EquipmentDao::getInstance();
		mapDao = MapDao::getInstance();
		noticeAction = NoticeOnlineUserAction::getInstance();
	}
	
	void login(string &username, string &password, WebsocketSession &session)
	{
		User *user = userdao->getUserByUsernameAndPassword(username, password);
		int connfd = session.getConnfd();
		static WebSocketSessionManage *webSocketSessionManage = WebSocketSessionManage::getInstance();
	
		if (!user)
		{
			response(connfd, getLoginResultJson("username or password is invalid").c_str());
		}
		else if (webSocketSessionManage->isUserOnline(username))
		{
			response(connfd, getLoginResultJson("login in other device").c_str());
		}
		else
		{
			webSocketSessionManage->updateSession(connfd, username);
			
			session.setUser(user);
			session.setUserEquipList(equipDap->getByUserid(user->id));
			response(session.getConnfd(), getLoginResultJson("success").c_str());
			response(connfd, getUserInfoJson(user).c_str());
			response(connfd, getEquipmentListJson(*session.getUserEquipList()).c_str());
			response(connfd, getMapListJson(mapDao->getAllMap()).c_str());
			noticeAction->noticeAllOnlineUser(connfd, username);
			noticeAction->noticeNewOnlineUser(username, user->level);
		}
	}
	
	void wearEquip(const long &equipid, const string &equipTypeStr, WebsocketSession &session)
	{
		vector<Equipment> *v = session.getUserEquipList();
		Equipment *newEquip, *oldEquip, *tmp;
		newEquip = oldEquip = NULL;
		short equipType = Equipment::convertEquipTypeToNumber(equipTypeStr);
		int connfd = session.getConnfd();
	
		for (int i = 0, len = v->size(); i < len; ++i)
		{	
			if (equipType == (*v)[i].type && (*v)[i].equipmentid != equipid && (*v)[i].wear)
			{
				oldEquip = &(*v)[i];
			}
			
			if (equipid == (*v)[i].equipmentid)
			{
				newEquip = &(*v)[i];
			}
		}
		
		if (!newEquip)
		{
			response(connfd, getWearEquipResultJson(false).c_str());
		}
		else if (newEquip->wear)
		{
			response(connfd, getWearEquipResultJson(false).c_str());
		}
		else
		{
			UserAction::wearEquip(session.getupdateEquipSem(), session.getUser(), newEquip, oldEquip);
			
			if (oldEquip)
			{
				response(connfd, getWearEquipResultJson(true, newEquip->equipmentid, oldEquip->equipmentid).c_str());	
			}
			else
			{
				response(connfd, getWearEquipResultJson(true, newEquip->equipmentid).c_str());	
			}
			
			response(connfd, getUserInfoJson(session.getUser()).c_str());
		}
	}

	void createUserFight(string oppnentUsername, WebsocketSession &session, FightAction *fightAction)
	{
		bool result = fightAction->createUserFight(oppnentUsername, &session);
		
		if (!result)
		{
			session.setFight(false);
		}
		
		response(session.getConnfd(), getCreateUserFightResultJson(result, oppnentUsername).c_str());
	}

	void joinUserFight(int &creatorConnfd, WebsocketSession &session, FightAction *fightAction)
	{
		if (!fightAction->joinUserFight(creatorConnfd, &session))
		{
			session.setFight(false);
		}
	}
public:
	void requestHanlder(string &method, cJSON *json, WebsocketSession &session)
	{
		int connfd = session.getConnfd(); 
		static FightAction *fightAction = FightAction::getInstance();
		bool fighting = session.isFighting();
		
		if (fighting)
		{
			if (method == "userFightReady")
			{
				session.setReadyToFight(true);
			}
			else if (method == "cancelUserFight")
			{
				string oppnentUsername(cJSON_GetObjectItem(json, "oppnentUsername")->valuestring);
				fightAction->cancelUserFight(oppnentUsername, &session);
			}
			else
			{
				response(connfd, getErrorJson("fighting").c_str());
			}
		}
		else
		{
			if (method == "beginMapFight")
			{
				long mapid = atol(cJSON_GetObjectItem(json, "mapid")->valuestring);
				session.setFight(true);
				fightAction->mapFight(mapid, session);
				session.setFight(false);
			}
			else if (method == "wearEquip")
			{
				long equipid = atol(cJSON_GetObjectItem(json, "equipid")->valuestring);
				string equipType(cJSON_GetObjectItem(json, "equipType")->valuestring);
				wearEquip(equipid, equipType, session);
			}
			else if (method == "joinUserFight")
			{
				session.setFight(true);
				int creatorConnfd = atol(cJSON_GetObjectItem(json, "creatorConnfd")->valuestring);
				joinUserFight(creatorConnfd, session, fightAction);
			}
			else if (method == "refuseUserFight")
			{
				int creatorConnfd = atol(cJSON_GetObjectItem(json, "creatorConnfd")->valuestring);
				fightAction->refuseUserFight(creatorConnfd, &session);
			}
			else if (method == "createUserFight")
			{
				session.setFight(true);
				string oppnentUsername(cJSON_GetObjectItem(json, "username")->valuestring);
				createUserFight(oppnentUsername, session, fightAction);
			}
			else if (method == "login")
			{
				string username(cJSON_GetObjectItem(json, "username")->valuestring);
				string password(cJSON_GetObjectItem(json, "password")->valuestring);
		
				login(username, password, session);
			}
			else if (method == "logout")
			{
				session.setUser(NULL);
				response(connfd, getLogoutResultJson("success").c_str());
			}
			else
			{
				response(connfd, getErrorJson("error method").c_str());
			}
		}
	}
	
	static RequestHanlder* getInstance()
	{
		static RequestHanlder instance;
		return &instance;
	}
};
