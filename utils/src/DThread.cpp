
#include "DThread.h"

#include "DTimer.h"

#include <unistd.h>


DThread::DThread()
{
	handler = NULL;
	arg =NULL;
}

void DThread::SetCallback(ThreadHandler handler, void* arg)
{
	this->handler = handler;
	this->arg = arg;
}

bool DThread::Start()
{
	return (pthread_create(&desc, NULL, Handler, this) == 0);
  	
}


void* DThread::Handler(void* arg)
{
	((DThread*)arg)->OnRun();
	return NULL;
}

void DThread::OnRun()
{
	if (handler)
		handler(arg);
}

bool DThread::Join()
{
	return (pthread_join(desc, NULL) == 0);
}

void DThread::Sleep(int val)
{
	sleep(val/1000);
	usleep((val%1000)*1000);	
}

bool DThread::WaitCond(bool (*CondFunc)(void*), void* arg, int timeout)
{
	DTimer timer(timeout);

	while (!CondFunc(arg))
	{
		if (timer.GetRemainingTime() < 0)
		{
			return false;
		}		
		DThread::Sleep(10);
	}		
	return true;
}

DMutex::DMutex()
{
	pthread_mutex_init(&desc, NULL);
}

DMutex::~DMutex()
{
	pthread_mutex_destroy(&desc);
}

	
void DMutex::Lock()
{
	pthread_mutex_lock(&desc);
}

void DMutex::Unlock()
{
	pthread_mutex_unlock(&desc);
}
