#include "requestReceiver.h"
#include "equipmentDao.h"
#include "JsonFactory.h"
#include <cstdlib>
#include <unistd.h>

#define PORT 8000

void *newThread(void *arg) {
	static RqeuestReceiver *rqeuestReceiver = RqeuestReceiver::getInstance();
	
	rqeuestReceiver->receiveRequest(*(int *)arg);
	pthread_exit(NULL);
}

/*
/tmp/VisualGDB/d/Code/C/project/LinuxProject3
g++ cJSON.c server.cpp -I/usr/include/mysql -L/usr/include/mysql -lmysqlclient -lpthread
 */
int main() {
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd < 0)
	{
		printf("socket error\n");
		return 1;
	}
	
	int reuse = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	
	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("bind error\n");
		return 1;
	}
	
	if (listen(sockfd, 10) < 0)
	{
		printf("listen error\n");
		return 1;
	}
	
	int connfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	
	while (1) {
		pthread_t thread;
		connfd = accept(sockfd, (struct sockaddr *)&client, &len);
		int err1 = pthread_create(&thread, NULL, newThread, &connfd);
		
		if (!err1)
		{
			pthread_detach(thread);	
		}
	}


	return 0;
}
