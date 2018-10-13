#pragma once
#include "my.h"
#include <iostream>
#include <string>

using namespace std;

class MyRPC
{
protected:
	CLIENT *clnt;
	
	MyRPC( unsigned long proc,
		unsigned long vers,
		string host = "127.0.0.1",
		string method = "tcp")
	{
		clnt = clnt_create(host.c_str(), proc, vers, method.c_str());
		
		if (clnt == NULL) {
			clnt_pcreateerror(host.c_str());
		}
	}
	
	void run(u_long method, void* result = NULL, void *argp = NULL)
	{
		static struct timeval TIMEOUT = { 25, 0 };
		
		if (clnt_call(clnt,
			method,
			(xdrproc_t) xdr_void,
			(caddr_t) argp,
			(xdrproc_t) xdr_int,
			(caddr_t) result,
			TIMEOUT) != RPC_SUCCESS) {
				result = NULL;
		}
	
		result = NULL;
	}
public:
	bool isConnect()
	{
		return clnt != NULL;
	}
};