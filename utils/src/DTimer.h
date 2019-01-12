#ifndef DTIMER_H
#define DTIMER_H

#include <sys/time.h>

class DTimer
{
public:
	DTimer(int Timeout);
	DTimer();
	
	void Start(int Timeout);
	void Stop();
	
	int GetRemainingTime();
	bool IsExpired();
	
	static void GetTimeOfDay(struct timeval* now);
	static int TimeSub(struct timeval* T0, struct timeval* T1);
private:
	struct timeval Time;
	bool isStarted;
};



#endif
