#pragma once
#include "websocketSendAction.h"
#include "requestHanlder.h"
#include "websocketSession.h"
#include "websocketSessionManage.h"
#include <errno.h> 

#define LINE_MAX 256 
#define BUFF_SIZE 1024
#define WEB_SOCKET_KEY_LEN_MAX 256  
#define RESPONSE_HEADER_LEN_MAX 1024  

char *http_res_tmpl = "HTTP/1.1 200 OK\r\n"
"Server: Cleey's Server V1.0\r\n"
"Accept-Ranges: bytes\r\n"
"Content-Length: %d\r\n"
"Connection: close\r\n"
"Content-Type: %s\r\n\r\n";
	
void shakeHand(int &connfd, char *buffer);  
char * fetchSecKey(char * buf);  
char * computeAcceptKey(char * buf);  
char * analyData(char * buf, int bufLen); 

class RqeuestReceiver
{
private:
	char * fetchSecKey(char * buf)  
	{  
		char *key;  
		char *keyBegin;  
		char *flag = "Sec-WebSocket-Key: ";  
		int i = 0, bufLen = 0;  

		key = (char *)malloc(WEB_SOCKET_KEY_LEN_MAX);  
		memset(key, 0, WEB_SOCKET_KEY_LEN_MAX);  
		if (!buf)  
		{  
			return NULL;  
		}  

		keyBegin = strstr(buf, flag);  
	
		if (!keyBegin)  
		{  
			return NULL;  
		}  
		keyBegin += strlen(flag);  

		bufLen = strlen(buf);  
		for (i = 0; i < bufLen; i++)  
		{  
			if (keyBegin[i] == 0x0A || keyBegin[i] == 0x0D)  
			{  
				break;  
			}  
			key[i] = keyBegin[i];  
		}  

		return key;  
	}  

	char * computeAcceptKey(char * buf)  
	{  
		char * clientKey;  
		char * serverKey;   
		char * sha1DataTemp;  
		char * sha1Data;  
		short temp;  
		int i, n;  
		const char * GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";  


		if (!buf)  
		{  
			return NULL;  
		}  
	
		clientKey = (char *)malloc(LINE_MAX);  
		memset(clientKey, 0, LINE_MAX);  
		clientKey = fetchSecKey(buf);  

		if (!clientKey)  
		{  
			return NULL;  
		}  

		strcat(clientKey, GUID);  

		sha1DataTemp = sha1_hash(clientKey);  
		n = strlen(sha1DataTemp);  


		sha1Data = (char *)malloc(n / 2 + 1);  
		memset(sha1Data, 0, n / 2 + 1);  

		for (i = 0; i < n; i += 2)  
		{        
			sha1Data[i / 2] = htoi(sha1DataTemp, i, 2);      
		}   

		serverKey = base64_encode(sha1Data, strlen(sha1Data));   

		return serverKey;  
	}  

	void shakeHand(int &connfd, char *buffer)  
	{  
		char *serverKey = computeAcceptKey(buffer);
		char responseHeader[RESPONSE_HEADER_LEN_MAX];  

		if (!connfd)  
		{  
			return;  
		}  

		if (!serverKey)  
		{  
			return;  
		}  

		memset(responseHeader, '\0', RESPONSE_HEADER_LEN_MAX);  

		sprintf(responseHeader, "HTTP/1.1 101 Switching Protocols\r\n");  
		sprintf(responseHeader, "%sUpgrade: websocket\r\n", responseHeader);  
		sprintf(responseHeader, "%sConnection: Upgrade\r\n", responseHeader);  
		sprintf(responseHeader, "%sSec-WebSocket-Accept: %s\r\n\r\n", responseHeader, serverKey);  
		write(connfd, responseHeader, strlen(responseHeader));  
	} 

	char * analyData(char * buf, int bufLen)  
	{  
		char * data;  
		char fin, maskFlag, masks[4];  
		char * payloadData;  
		char temp[8];  
		unsigned long n, payloadLen = 0;  
		unsigned short usLen = 0;  
		int i = 0;   


		if (bufLen < 2)   
		{  
			return NULL;  
		}  

		fin = (buf[0] & 0x80) == 0x80; 
		if (!fin)  
		{  
			return NULL;  
		}  

		maskFlag = (buf[1] & 0x80) == 0x80; 
		if (!maskFlag)  
		{  
			return NULL; 
		}  

		payloadLen = buf[1] & 0x7F;  
		if (payloadLen == 126)  
		{        
			memcpy(masks, buf + 4, 4);        
			payloadLen = (buf[2] & 0xFF) << 8 | (buf[3] & 0xFF);    
			payloadData = (char *)malloc(payloadLen);  
			memset(payloadData, 0, payloadLen);  
			memcpy(payloadData, buf + 8, payloadLen);  
		}  
		else if (payloadLen == 127)  
		{  
			memcpy(masks, buf + 10, 4);    
			for (i = 0; i < 8; i++)  
			{  
				temp[i] = buf[9 - i];  
			}   

			memcpy(&n, temp, 8);    
			payloadData = (char *)malloc(n);   
			memset(payloadData, 0, n);   
			memcpy(payloadData, buf + 14, n);
			payloadLen = n;      
		}  
		else  
		{     
			memcpy(masks, buf + 2, 4);      
			payloadData = (char *)malloc(payloadLen);  
			memset(payloadData, 0, payloadLen);  
			memcpy(payloadData, buf + 6, payloadLen);   
		}  

		for (i = 0; i < payloadLen; i++)  
		{  
			payloadData[i] = (char)(payloadData[i] ^ masks[i % 4]);  
		}  
 
		return payloadData;  
	}  
	
	RqeuestReceiver()
	{
	}
public:
	void receiveRequest(int connfd)
	{
		fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFL, 0) | O_NONBLOCK);
		static RequestHanlder* requestHanlder = RequestHanlder::getInstance();
		static WebSocketSessionManage *webSocketSessionManage = WebSocketSessionManage::getInstance();
		char buffer[BUFF_SIZE];
		int size = read(connfd, buffer, BUFF_SIZE - 1);
		shakeHand(connfd, buffer);
		WebsocketSession session(connfd);
		webSocketSessionManage->addSession(connfd, &session);

		while (1)
		{
			memset(buffer, 0, BUFF_SIZE);
			//size = read(connfd, buffer, BUFF_SIZE - 1);
			size = recv(connfd, buffer, BUFF_SIZE - 1, MSG_DONTWAIT);
		
			if (size == 0)
			{
				printf("close\n");
				break;
			}
			else if (size < 0)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
				{
					usleep(100);
					continue;
				}
				else
				{
					printf("close\n");
					break;
				}
			}
		
			char *data = analyData(buffer, size);
			printf("Receive data(%d, size%d) :%s\n", connfd, size, data);
			cJSON *json = cJSON_Parse(data);
			
			if (!json)
			{
				continue;
			}
			
			string method(cJSON_GetObjectItem(json, "method")->valuestring);
		
			if (!session.isLogin() && method != "login")
			{
				response(connfd, getErrorJson("please login first").c_str());
			}
			else if (session.isLogin() && method == "login")
			{
				response(connfd, getErrorJson("already login").c_str());
			}
			else
			{
				requestHanlder->requestHanlder(method, json, session);
			}
		
			cJSON_Delete(json);
		}
		
		static NoticeOnlineUserAction* noticeOnlineUserAction = NoticeOnlineUserAction::getInstance();
		
		if (session.getUser())
		{
			noticeOnlineUserAction->noticeNewOfflineUser(session.getUser()->username);
		}
	
		webSocketSessionManage->removeSession(connfd);
		close(connfd);
	}
	
	static RqeuestReceiver* getInstance()
	{
		static RqeuestReceiver instance;
		return &instance;
	}
};
