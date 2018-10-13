#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "equipmentDao.h"
#include "mapDao.h"
#include "userDao.h"
#include "monsterDao.h"
#include "cJSON.h"
#include "websocketSession.h"
#include "websocketSessionManage.h"

using namespace std;

string getErrorJson(string reason)
{
	cJSON *json = cJSON_CreateObject();
	
	cJSON_AddStringToObject(json, "method", "error");
	cJSON_AddStringToObject(json, "reason", reason.c_str());
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getLoginResultJson(string result)
{
	cJSON *json = cJSON_CreateObject();
	
	cJSON_AddStringToObject(json,"method", "loginResult");
	cJSON_AddStringToObject(json, "result", result.c_str());
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getLogoutResultJson(string result)
{
	cJSON *json = cJSON_CreateObject();
	
	cJSON_AddStringToObject(json, "method", "logoutResult");
	cJSON_AddStringToObject(json, "result", result.c_str());
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getUserInfoJson(const User *user)
{
	cJSON *json = cJSON_CreateObject();
	
	cJSON_AddStringToObject(json, "method", "userInfo");
	cJSON_AddNumberToObject(json, "id", user->id);
	cJSON_AddNumberToObject(json, "level", user->level);
	cJSON_AddStringToObject(json, "username", user->username.c_str());
	cJSON_AddNumberToObject(json, "points", user->points);
	cJSON_AddNumberToObject(json, "agility", user->agility + user->attachAgility);
	cJSON_AddNumberToObject(json, "strength", user->strength + user->attachStrength);
	cJSON_AddNumberToObject(json, "HP", user->HP + user->attachHP);
	cJSON_AddNumberToObject(json, "MP", user->MP + user->attachMP);
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getEquipmentListJson(const vector<Equipment> &v)
{
	cJSON *json = cJSON_CreateObject();
	cJSON  *array = cJSON_CreateArray();
	Equipment equip;
	
	for (int i = 0, len = v.size(); i < len; ++i)
	{
		cJSON *tmp = cJSON_CreateObject();
		equip = v[i];
		
		cJSON_AddItemToArray(array, equip.getJson());
	}
	
	cJSON_AddStringToObject(json, "method", "equipmentList");
	
	if (v.size())
	{
		cJSON_AddItemToObject(json, "equipmentList", array);
	}
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getMapListJson(const vector<MyMap*> &v, string method="mapList")
{
	cJSON *json = cJSON_CreateObject();
	cJSON  *array = cJSON_CreateArray();
	cJSON_AddStringToObject(json, "method", method.c_str());
	
	for (int i = 0, len = v.size(); i < len; ++i)
	{
		cJSON *tmp = cJSON_CreateObject();
		MyMap *myMap = v[i];
		
		cJSON_AddNumberToObject(tmp, "mapid", myMap->mapid);
		cJSON_AddStringToObject(tmp, "detail", myMap->detail.c_str());
		cJSON_AddStringToObject(tmp, "mapname", myMap->mapname.c_str());
		delete(myMap);
		cJSON_AddItemToArray(array, tmp);
	}
	
	if (v.size())
	{
		cJSON_AddItemToObject(json, "mapList", array);
	}
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getMonsterJson(const Monster &monster)
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "mapMonster");
	cJSON_AddNumberToObject(json, "monsterid", monster.monsterid);
	cJSON_AddStringToObject(json, "monstername", monster.monstername.c_str());
	cJSON_AddNumberToObject(json, "agility", monster.agility);
	cJSON_AddNumberToObject(json, "strength", monster.strength);
	cJSON_AddNumberToObject(json, "level", monster.level);
	cJSON_AddNumberToObject(json, "HP", monster.HP);

	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getFightDetailJson(const int &userhp, const int &userAssault, const bool &userMiss,
	const int &opponentHP, const int &opponentAssault, const bool &opponentMiss,
	string userSkill = "普通攻击", string opponentSkill = "普通攻击")
{
	cJSON *json = cJSON_CreateObject();
	
	cJSON_AddStringToObject(json, "method", "fightDetail");
	cJSON_AddBoolToObject(json, "userMiss", userMiss);
	cJSON_AddBoolToObject(json, "opponentMiss", opponentMiss);
	cJSON_AddNumberToObject(json, "userhp", userhp);
	cJSON_AddNumberToObject(json, "opponentHP", opponentHP);

	if (!userMiss)
	{
		cJSON_AddNumberToObject(json, "userAssault", userAssault);
		cJSON_AddStringToObject(json, "userSkill", userSkill.c_str());
	}
	
	if (!opponentMiss)
	{
		cJSON_AddNumberToObject(json, "opponentAssault", opponentAssault);
		cJSON_AddStringToObject(json, "opponentSkill", opponentSkill.c_str());
	}
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getFightResultJson(bool isUserWin, int XP = 0, string getEquip = "未掉落新装备")
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "fightResult");
	cJSON_AddBoolToObject(json, "isUserWin", isUserWin);
	cJSON_AddNumberToObject(json, "XP", XP);
	cJSON_AddStringToObject(json, "getEquip", getEquip.c_str());

	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getMapFightResultJson(bool isUserWin)
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "finalFightResult");
	cJSON_AddBoolToObject(json, "isUserWin", isUserWin);

	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getAddEquipJson(const Equipment &equip)
{
	cJSON *json = equip.getJson();
	cJSON_AddStringToObject(json, "method", "getNewEquip");
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getWearEquipResultJson(bool success, long newid = 0, long oldid = 0)
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "wearEquipResult");
	cJSON_AddBoolToObject(json, "success", success);
	
	if (success)
	{
		cJSON_AddNumberToObject(json, "newid", newid);
		cJSON_AddNumberToObject(json, "oldid", oldid);
	}
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getCreateUserFightResultJson(bool &success, string oppoUsername)
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "createUserFightResult");
	cJSON_AddBoolToObject(json, "success", success);
	cJSON_AddStringToObject(json, "oppoUsername", oppoUsername.c_str());

	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getNoticeUserFightJson(int creatorConnfd, User *opponent)
{
	cJSON *json = cJSON_CreateObject();
	cJSON *userInfo = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "noticeUserFightResult");
	cJSON_AddNumberToObject(json, "creatorConnfd", creatorConnfd);
	cJSON_AddNumberToObject(userInfo, "level", opponent->level);
	cJSON_AddStringToObject(userInfo, "username", opponent->username.c_str());					
	cJSON_AddNumberToObject(userInfo, "agility", opponent->agility);
	cJSON_AddNumberToObject(userInfo, "strength", opponent->strength);
	cJSON_AddNumberToObject(userInfo, "HP", opponent->HP);
	cJSON_AddNumberToObject(userInfo, "MP", opponent->MP);
	cJSON_AddItemToObject(json, "userInfo", userInfo);
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getJoinUserFightResultJson(bool join, string opponentName, string reason = "none")
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "joinUserFightResult");
	cJSON_AddBoolToObject(json, "join", join);
	cJSON_AddStringToObject(json, "opponentName", opponentName.c_str());
	cJSON_AddStringToObject(json, "reason", reason.c_str());

	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getCancelUserFightResultJson(bool success, string oppnentUsername)
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "cancelUserFightResult");
	cJSON_AddBoolToObject(json, "success", success);
	cJSON_AddStringToObject(json, "oppnentUsername", oppnentUsername.c_str());

	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getNoticeNewOnlineUserJson(string &username, short &level)
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "newOnlineUser");
	cJSON_AddStringToObject(json, "username", username.c_str());
	cJSON_AddNumberToObject(json, "level", level);

	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getNoticeNewOfflineUserJson(string &username)
{
	cJSON *json = cJSON_CreateObject();

	cJSON_AddStringToObject(json, "method", "newOfflineUser");
	cJSON_AddStringToObject(json, "username", username.c_str());

	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}

string getNoticeAllOnlineUserJson(string selename)
{
	static WebSocketSessionManage *webSocketSessionManage = WebSocketSessionManage::getInstance();
	hash_map<int, WebsocketSession*>* connfdSessionMap = webSocketSessionManage->getAllOnlineUserMap();
	cJSON *json = cJSON_CreateObject();
	cJSON *array = cJSON_CreateArray();
	
	for (hash_map<int, WebsocketSession*>::iterator it = connfdSessionMap->begin(); it != connfdSessionMap->end();it++)
	{	
		int connfd = it->first;
		
		if (webSocketSessionManage->isSessionOpen(connfd))
		{
			cJSON *tmp = cJSON_CreateObject();
			User *user = it->second->getUser();
			
			if (!user || user->username == selename)
			{
				continue;
			}
		
			cJSON_AddNumberToObject(tmp, "level", user->level);
			cJSON_AddStringToObject(tmp, "username", user->username.c_str());	
			cJSON_AddItemToArray(array, tmp);
		}
	}
	
	cJSON_AddStringToObject(json, "method", "allOnlineUserList");
	cJSON_AddItemToObject(json, "userList", array);
	
	string jsonstr = cJSON_Print(json);
	cJSON_Delete(json);
	
	return jsonstr;
}