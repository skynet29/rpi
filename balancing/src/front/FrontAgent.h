#ifndef FRONTAGENT_H
#define FRONTAGENT_H

#include "BiteServerApp.h"


class FrontAgent : public BiteServerApp
{
public:
	FrontAgent();
protected:
	virtual bool OnStartup();
	virtual void OnIterate();
	virtual bool OnConfig(Json::Value& root);
private:
	
	DTimer timer;	
	int monitorPeriod;
		
};

#endif
