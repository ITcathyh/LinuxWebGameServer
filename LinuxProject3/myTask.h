#pragma once
#include <iostream>

class MyTask
{
public:
	MyTask() = default;
	virtual void run() = 0;
	virtual ~MyTask() {}
};