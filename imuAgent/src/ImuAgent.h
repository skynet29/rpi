#ifndef IMUAGENT_H
#define IMUAGENT_H

#include "DServerApp.h"
#include "DTimer.h"

class RTIMU;

class ImuAgent : public DServerApp
{
public:
	ImuAgent();
protected:
	virtual bool OnStartup();
	virtual void OnIterate();
	virtual bool OnConfig(Json::Value& root);
private:
	
	DTimer timer;	
	int monitorPeriod;
	RTIMU *imu;
		
};

#endif
