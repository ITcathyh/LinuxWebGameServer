#pragma once
#include <sys/types.h>  
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include "base64.h"  
#include "sha1.h"  
#include "intLib.h"  
#include "cJSON.h"
#include "websocketSessionManage.h"

char *packData(const char * message, unsigned long * len)  
{  
	char * data = NULL;  
	unsigned long n;  

	n = strlen(message);  
	if (n < 126)  
	{  
		data = (char *)malloc(n + 2);  
		memset(data, 0, n + 2);      
		data[0] = 0x81;  
		data[1] = n;  
		memcpy(data + 2, message, n);  
		*len = n + 2;  
	}  
	else if (n < 0xFFFF) {  
		data = (char *)malloc(n + 4);  
		memset(data, 0, n + 4);  
		data[0] = 0x81;  
		data[1] = 126;  
		data[2] = (n >> 8 & 0xFF);  
		data[3] = (n & 0xFF);  
		memcpy(data + 4, message, n);      
		*len = n + 4;  
	}  
	else {  
		*len = 0;  
	}  


	return data;  
}  

void response(int connfd, const char * message)  
{  
	static WebSocketSessionManage *webSocketSessionManage = WebSocketSessionManage::getInstance();
	
	if (!webSocketSessionManage->isSessionOpen(connfd))
	{
		return;
	}
	
	unsigned long n = 0;  
	int i;  
	
	char *data = packData(message, &n);   

	if (!data || n <= 0)  
	{  
		printf("data is empty!\n");  
		return;  
	}   
	
	if (!webSocketSessionManage->isSessionOpen(connfd))
	{
		return;
	}

	send(connfd, data, n, MSG_DONTWAIT);
	//write(connfd, data, n);
}