#pragma once
#include <vector>
#include <pthread.h>
#include "myTask.h"
using namespace std;

class MyThreadPool
{
private:
	static vector<MyTask*> taskList;
	static pthread_mutex_t pthreadMutex;
	static pthread_mutex_t sizeMutex;
	static pthread_cond_t pthreadCond;  
	static int size;
	pthread_t *pthreadIds;
	pthread_mutex_t dilatationMutex;
	int capacity;
	
	enum
	{
		THREAD_NUM = 10
	};
	
	MyThreadPool()
	{
		capacity = THREAD_NUM;
		pthreadIds = new pthread_t[THREAD_NUM];
		dilatationMutex = PTHREAD_MUTEX_INITIALIZER;
		
		create(pthreadIds, 0, THREAD_NUM);
	}
	
	static void* threadWork(void* info) {
		while (1)
		{
			pthread_mutex_lock(&pthreadMutex);
			
			while(!taskList.size())
			{
				pthread_cond_wait(&pthreadCond, &pthreadMutex);
			}
        
			vector<MyTask*>::iterator iter = taskList.begin();
			MyTask* task = *iter;
			
			if (iter != taskList.end())
			{
				task = *iter;
				taskList.erase(iter);
			}
        
			pthread_mutex_unlock(&pthreadMutex);
			
			pthread_mutex_lock(&sizeMutex);
			++size;
			pthread_mutex_unlock(&sizeMutex);
			
			task->run();
		
			pthread_mutex_lock(&sizeMutex);
			--size;
			pthread_mutex_unlock(&sizeMutex);
			
			delete task;
		}
    
		return (void*)0;
	}
	
	bool dilatation()
	{
		int len = capacity;
		capacity += THREAD_NUM;
		pthread_t *tmp = new pthread_t[capacity];
		pthread_t *old = pthreadIds;
		
		for (int i = 0; i < len; ++i)
		{
			tmp[i] = pthreadIds[i];
		}
		
		create(tmp, len, capacity);
		pthreadIds = tmp;
		
		delete[] old;
		return true;
	}
	
	void create(pthread_t *threads,int begin, int end)
	{
		for (int i = begin; i < end; ++i)
		{
			if (!pthread_create(&threads[i], NULL, threadWork, NULL))
			{
				pthread_detach(threads[i]);	
			}
		}
	}
public:
	static MyThreadPool* getInstance()
	{
		static MyThreadPool instance;
		return &instance;
	}
	
	int addTask(MyTask *task) { 
		pthread_mutex_lock(&dilatationMutex);
		
		pthread_mutex_lock(&sizeMutex);
		int &curSize = size;
		pthread_mutex_unlock(&sizeMutex);
		
		if (curSize >= (capacity - 4))
		{
			if (!dilatation())
			{
				return -1;
			}
		}
		
		pthread_mutex_unlock(&dilatationMutex);
		
		pthread_mutex_lock(&pthreadMutex);    
		taskList.push_back(task);  
		pthread_mutex_unlock(&pthreadMutex);  
		pthread_cond_signal(&pthreadCond);    
		
		return 1;
	}
	
	~MyThreadPool() { 
		delete[] pthreadIds;
	}
};

vector<MyTask*> MyThreadPool::taskList;
pthread_mutex_t MyThreadPool::pthreadMutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t MyThreadPool::sizeMutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t MyThreadPool::pthreadCond = PTHREAD_COND_INITIALIZER;  
int MyThreadPool::size = 0;