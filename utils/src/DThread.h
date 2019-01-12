#ifndef DTHREAD_H
#define DTHREAD_H

#include <pthread.h>


typedef void (*ThreadHandler)(void*);

class DThread
{
public:
	DThread();
	
	bool Start();
	bool Join();
	
	void SetCallback(ThreadHandler handler, void* arg);
	
	static void Sleep(int val);
	static bool WaitCond(bool (*CondFunc)(void*), void* arg, int timeout);
protected:
	pthread_t desc;
	ThreadHandler handler;
	void* arg;

	static void* Handler(void* arg);
	virtual void OnRun();
};


class DMutex
{
public:
	DMutex();
	~DMutex();
	
	void Lock();
	void Unlock();
private:
	pthread_mutex_t desc;
};

#endif
