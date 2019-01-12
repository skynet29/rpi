#ifndef BITESERVERAPP_H
#define BITESERVERAPP_H

#include "DServerApp.h"
#include "DTimer.h"
#include "DSocket.h"
#include "SensorBiteNode.h"

#include "SensorManager.h"



class BiteServerApp : public DServerApp
{
public:
	BiteServerApp();
protected:
	virtual bool OnStartup();
	virtual void OnIterate();
	virtual bool OnConfig(Json::Value& root);
	virtual bool ComputeBite(bool forceSending);
	
	SensorBiteNode* AddBiteNode(const string& nodeName);
	SensorBiteNode* GetBiteNode(const string& name);
	
	SensorManager sensorMngr;
	
	DClient* biteClient;
	DTimer biteTimer;
	
	int bitePeriod;
private:	
	vector<SensorBiteNode*> biteNodes;
};

#endif
