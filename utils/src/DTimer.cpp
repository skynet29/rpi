#include "DTimer.h"


DTimer::DTimer()
{
	gettimeofday(&Time, 0);
	isStarted = false;
}

DTimer::DTimer(int Timeout)
{
	Start(Timeout);
}

void DTimer::Start(int Timeout)
{
	gettimeofday(&Time, 0);
	
	Time.tv_sec += Timeout/1000;
	Time.tv_usec += (Timeout%1000)*1000;
	Time.tv_sec += (Time.tv_usec/1000000)%1000000;
	Time.tv_usec %= 1000000;	
	isStarted = true;
}

void DTimer::GetTimeOfDay(struct timeval* now)
{
	gettimeofday(now, 0);
	
}

int DTimer::TimeSub(struct timeval* T0, struct timeval* T1)
{
	int sec;
	struct timeval TR;
	TR.tv_sec = T0->tv_sec - 1 - T1->tv_sec;
	TR.tv_usec = T0->tv_usec + 1000000 - T1->tv_usec;
	sec = TR.tv_usec/1000000;
	TR.tv_sec += sec;
	TR.tv_usec -= sec*1000000;
	
	return TR.tv_sec*1000 + TR.tv_usec/1000;
}

void DTimer::Stop()
{
	isStarted = false;
}

int DTimer::GetRemainingTime()
{
	struct timeval now;
	
	gettimeofday(&now, 0);
		
	return TimeSub(&Time, &now);
}

bool DTimer::IsExpired()
{
	return (isStarted && GetRemainingTime() <= 0);
}
