#include "FrontAgent.h"

#include "DLogger.h"

#include <stdio.h>
#include <json/writer.h>


static DLogger logger("FrontAgent");

int main(int argc, char* argv[])
{
	FrontAgent agent;	
	if (!agent.ParseArg(argc, argv))
		return -1;
	
	return agent.Run();
}


FrontAgent::FrontAgent()
{
	AddBiteNode("sensors");	
}

bool FrontAgent::OnStartup()
{
	if (!BiteServerApp::OnStartup())
		return false;
		
	timer.Start(monitorPeriod);
	
	return true;
}

void FrontAgent::OnIterate()
{
	BiteServerApp::OnIterate();
	
				
	if (timer.IsExpired())
	{
		Json::Value root;
		struct timeval tv;
		DTimer::GetTimeOfDay(&tv);
		
		root["t"] = (Json::UInt)tv.tv_sec;

		for(int i = 0; i < sensorMngr.GetSensorCount(); i++)
		{
			Sensor* s = sensorMngr.GetSensorAt(i);
			root[s->GetName()] =  s->GetValue();
		}
				
		Json::FastWriter writer;
		string msg = writer.write(root);

	
		logger.Write(DLogger::DEBUG, "Msg=%s", msg.c_str());
		server.Broadcast(msg.c_str());
		timer.Start(monitorPeriod);
	}
		
}


bool FrontAgent::OnConfig(Json::Value& root)
{
	if (!BiteServerApp::OnConfig(root))
		return false;
	
	if (!root["monitorPeriod"].isInt())
		monitorPeriod = 1000; // 1sec
	else
		monitorPeriod = root["monitorPeriod"].asInt();
	
	return true;
}

