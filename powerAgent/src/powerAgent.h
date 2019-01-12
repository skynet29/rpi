#ifndef POWERAGENT_H
#define POWERAGENT_H

#include "DServerApp.h"
#include "DTimer.h"
#include <string>

#include <json/value.h>



using namespace std;

class PowerAgent : public DServerApp
{
public:
	PowerAgent();
	
protected:	
	virtual bool OnStartup();
	virtual void OnIterate();
	virtual void OnExit();
	virtual bool OnConfig(Json::Value& root);
	
	virtual void OnRecvData(DSocket* client, const char* msg);
private:
	void DecodeMsg(Json::Value& root);
	string EncodeMonitorMsg();
	
	DTimer monitoringTimer;
	DTimer pulseTimer;
	

	// config parameters
	int monitorPeriod;
	int pulsePeriod;
};

#endif
