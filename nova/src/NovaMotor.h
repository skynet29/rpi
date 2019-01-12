#ifndef NOVAMOTOR_H
#define NOVAMOTOR_H

#include <time.h>

#include "DThread.h"
#include "DTimer.h"
#include "ValveController.h"
#include "MonitoringVolume.h"

class NovaMotor : DThread
{
public:
	class MonitorData
	{
	public:
		MonitorData();
		
		float rpm;
		float voltage;
		float current;
		float tempCont;
		float tempMot;
		int power;
		time_t date;
		void Dump();
		
	};
	
	NovaMotor();
	~NovaMotor();
	
	bool Init(const char* deviceName);
	bool Init(); // for test only
	bool SendMsg(const char* msg);
	
	bool StartRev();
	bool StartFwd();
	bool Stop();
	bool IncPower();
	bool DecPower();
	bool SetPower(int power);
	void SetMaxPower(int maxPower);
	void SetTimeoutEcho(int timeoutEcho);
	void SendConfig();
	
	int GetCurPower() {return curPower;}
	void GetMonitorData(MonitorData& theData);
	
	bool IsInit() {return isInit;}
	bool IsBooted() {return isBooted;}
	bool IsAlive();
	void Reset();

     // modif_SH : reference monitoring des volumes
	MonitoringVolume *p_MonitoringVolume;	
	
private:
	bool isBooted;
	bool isInit;
	int fd;
	char* echoMsg;
	int curPower;
	MonitorData data;
	int maxPower;
	int timeoutEcho;
	DTimer aliveTimer;
	
	void OnRecvData(char* msg);
	void DecodeMonitorMsg(char* msg);
	
	virtual void OnRun();
};

#endif
