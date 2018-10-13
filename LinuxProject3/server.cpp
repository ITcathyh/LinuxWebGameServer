#include "requestReceiver.h"
#include "equipmentDao.h"
#include "JsonFactory.h"
#include <cstdlib>
#include <unistd.h>
#include "myThreadPool.h"
#include "sessionTask.h"
#include "getRandomNum.h"

#define PORT 8000
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
	
	if (listen(sockfd, 100) < 0)
	{
		printf("listen error\n");
		return 1;
	}
	
	int connfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	MyThreadPool *myThreadPool = MyThreadPool::getInstance();
	
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);
	printf("ready\n");
	
	while (1) {
		connfd = accept(sockfd, (struct sockaddr *)&client, &len);
		MyTask *task = new SessionTask(connfd);
		myThreadPool->addTask(task);
	}

	return 0;
}

