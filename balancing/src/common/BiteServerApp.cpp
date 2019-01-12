#include "BiteServerApp.h"

#include "DLogger.h"
#include "DClient.h"

#include <stdio.h>
#include <json/writer.h>



static DLogger logger("BiteServerApp");


BiteServerApp::BiteServerApp()
{
	bitePeriod = 30000; //30 sec
}

bool BiteServerApp::OnStartup()
{
	if (!DServerApp::OnStartup())
		return false;
		
		
	system("i2cdetect -y 1 > /dev/null"); // reset I2C bus	
		
	
	biteTimer.Start(bitePeriod);	
	
	return true;
}

void BiteServerApp::OnIterate()
{
	DServerApp::OnIterate();
	

	ComputeBite(false);
	
	if (biteTimer.IsExpired())
	{
		ComputeBite(true);
		biteTimer.Start(bitePeriod);
	}
						
}


bool BiteServerApp::OnConfig(Json::Value& root)
{
	if (!DServerApp::OnConfig(root))
		return false;
	
	
	// bite node
	
	string biteHost;
	int bitePort;
	
	if (GetConfigString(root, "bite.host", biteHost) && GetConfigInt(root, "bite.port", bitePort))
	{
		biteClient = Subscribe(biteHost.c_str(), bitePort);
		if (biteClient == NULL)
		{
			logger.Write(DLogger::ERROR, "biteClient subscribtion fail");
			return false;
		}
	}
	
	GetConfigInt(root, "bite.period", bitePeriod);
		
	// sensors	
	if (!sensorMngr.Init(root["sensors"]))
		return false;
		
	for(int i = 0; i < sensorMngr.GetSensorCount(); i++)
	{
		Sensor* s = sensorMngr.GetSensorAt(i);
		logger.Write(DLogger::DEBUG, "sensorName=%s", s->GetName().c_str());
		
		
		SensorBiteNode* biteNode = GetBiteNode(s->GetNode());
		if (biteNode != NULL)
			biteNode->AddSensor(s);
	}
		
	return true;
}

SensorBiteNode*  BiteServerApp::GetBiteNode(const string& name)
{
	for(unsigned i = 0; i < biteNodes.size(); i++)
	{
		if (biteNodes[i]->GetName() == name)
			return biteNodes[i];
	}
	logger.Write(DLogger::WARNING, "biteNode not found:%s", name.c_str());
	return NULL;	
}

bool BiteServerApp::ComputeBite(bool forceSending)
{
	logger.Write(DLogger::DEBUG, "ComputeBite()");
	
	if (!biteClient->IsConnected())
		return false;
		
	for(unsigned i = 0; i < biteNodes.size(); i++)
	{
		biteNodes[i]->Compute(*biteClient, forceSending);	
	}
	
	return true;	
}

SensorBiteNode* BiteServerApp::AddBiteNode(const string& nodeName)
{
	SensorBiteNode* biteNode = new SensorBiteNode(nodeName);
	biteNodes.push_back(biteNode);
	return biteNode;
}
