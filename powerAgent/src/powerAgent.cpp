#include "powerAgent.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <errno.h>

#include <json/reader.h>
#include <json/writer.h>

#include "DLogger.h"

#define GPIO_POWER 6
#define GPIO_PULSE 7

#define POWER_ON 0
#define POWER_OFF 1


static DLogger logger("PowerAgent");


int main(int argc, char* argv[])
{
	PowerAgent agent;	
	
	if (!agent.ParseArg(argc, argv))
		return -1;	
		
	return agent.Run();
}
	
PowerAgent::PowerAgent()
{
	monitorPeriod = 1000; 	// 1 sec
	pulsePeriod = 500; 		// 0.5 sec
}


void PowerAgent::DecodeMsg(Json::Value& root)
{
	string cmd;
		
	if (!GetConfigString(root, "cmd", cmd))
	{
		logger.Write(DLogger::ERROR, "Cmd parse error");
		return;
	}
	
	if (cmd == "POWER_ON")
	{
		digitalWrite(GPIO_POWER, POWER_ON);
	}
	else if (cmd == "POWER_OFF")
	{
		digitalWrite(GPIO_POWER, POWER_OFF);
	}
	else
	{
		logger.Write(DLogger::ERROR, "Unknown command");
	}
	
	
}




void PowerAgent::OnRecvData(DSocket* client, const char* msg)
{

	logger.Write(DLogger::INFO, "Msg:%s", msg);
	
	Json::Value root;
	Json::Reader reader;

	if (!reader.parse(msg, root))		
	{
		logger.Write(DLogger::ERROR, "JSON Parse error: Msg:%s", msg);
		return;			
	}
	
	DecodeMsg(root);
}

bool PowerAgent::OnStartup()
{
	if (!DServerApp::OnStartup())
		return false;
			
	if (wiringPiSetup() < 0)
	{
		logger.Write(DLogger::ERROR, "wiringPiSetup():%s", strerror(errno));
		return false;
	}
	
	digitalWrite(GPIO_POWER, POWER_OFF);
	
	// set all GPIO to OUTPUT mode
	pinMode(GPIO_POWER, OUTPUT);
	pinMode(GPIO_PULSE, OUTPUT);
			
	monitoringTimer.Start(monitorPeriod);
	
	pulseTimer.Start(pulsePeriod);
	
	return true;
}


void PowerAgent::OnIterate()
{
	DServerApp::OnIterate();
	
				
	if (monitoringTimer.IsExpired())
	{
			
		string msg = EncodeMonitorMsg();
		logger.Write(DLogger::DEBUG, "MonitorMsg=%s", msg.c_str());
			
		server.Broadcast(msg.c_str());
		monitoringTimer.Start(monitorPeriod);
	}
	
	if (pulseTimer.IsExpired())
	{
		int ret = digitalRead(GPIO_PULSE);
		
		digitalWrite(GPIO_PULSE, (ret == 0) ? 1 : 0);
		
		pulseTimer.Start(pulsePeriod);
	}
		
}


string PowerAgent::EncodeMonitorMsg()
{
		
	time_t now;
	time(&now);

	int powerState = digitalRead(GPIO_POWER);
	
	Json::Value root;
	root["t"] = (Json::UInt)now;
	root["state"] = (powerState == POWER_ON) ? "POWER_ON" : "POWER_OFF";
	
	
	Json::FastWriter writer;
	
	string msg = writer.write(root);	
	size_t idx = msg.rfind("}");
	return  msg.substr(0, idx) + "000" + msg.substr(idx); // force date in ms

}


void PowerAgent::OnExit()
{
	DServerApp::OnExit();
	
}



bool PowerAgent::OnConfig(Json::Value& root)
{
	if (!DServerApp::OnConfig(root))
		return false;
		
	GetConfigInt(root, "monitorPeriod", monitorPeriod);			
	GetConfigInt(root, "pulsePeriod", pulsePeriod);			
			
										
	return true;
}


