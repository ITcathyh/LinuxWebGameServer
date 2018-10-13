#pragma once
#include <memory.h> 
#include "my.h"
#include "myRPC.h"

#define RANDOM_RPC_HOST "127.0.0.1"

class GetRandomNum : public MyRPC
{
private:
	
	GetRandomNum()
		: MyRPC(MY_RPC_PROG, MY_RPC_VERS1, RANDOM_RPC_HOST)
	{
	}
public:
	int getRandomnum(void *argp = NULL)
	{
		int clnt_res;
		static struct timeval TIMEOUT = { 25, 0 };
		
		memset((char *)&clnt_res, 0, sizeof(clnt_res));
		run(getRandomNum, &clnt_res);
		
		return clnt_res;
	}
	
	static GetRandomNum* getInstance()
	{
		static GetRandomNum instance;
		return &instance;
	}
};