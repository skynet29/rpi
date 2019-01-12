#ifndef REARAGENT_H
#define REARAGENT_H

#include "BiteServerApp.h"
#include "NovaMotor.h"
#include "DTimer.h"
#include "ValveController.h"
#include <string>

#include <json/value.h>
#include "MonitoringVolume.h"

class DSequencer;


using namespace std;

class RearAgent : public BiteServerApp
{
public:
	RearAgent();
	
protected:	
	virtual bool OnStartup();
	virtual void OnIterate();
	virtual void OnExit();
	virtual bool OnConfig(Json::Value& root);
	
	virtual void OnRecvData(DSocket* client, const char* msg);
private:
	void EmergencyStop();
	void DecodeMsg(Json::Value& root);	
	void DecodeFrontAgentMsg(Json::Value& root);	
	void DecodeNavAgentMsg(Json::Value& root);	
	string EncodeMonitorMsg();
	bool DecodeBuoyCmd(int cmd, Json::Value& root);
	void StopCurrentAction();
	void Stop();
	void ComputeBite();
	void ReadSensors();
	bool IsPressureSensorsOK(int valve);
	void StartMotor(DSequencer* seq, int power);
	double ComputeStateOfCharge(double batteryVoltage, double batteryTemperature);

    // modif_SH : reference monitoring des volumes
	MonitoringVolume *p_MonitoringVolume;	
	
	NovaMotor nova;
	ValveController valveCtrl;

	DSequencer* curSeq;
	int curAction;
	struct timeval curActionStartTime;
	
	string lastMsg;

	DClient* frontClient;
	DClient* navClient;

	DTimer monitoringTimer;
	DTimer stopMotorTimer;
	int stopMotorDelay;
	

	// sensors
	double frontTemp;
	double frontWl;
	double rearTemp;
	double rearWl;
	double rtPressure;
	double ftPressure;
	double pumpPressure;
	double pumpTemp;
	double batteryTemp;
	
	double navCurrent;
	// config parameters
	bool isSimu;
	int monitorPeriod;
	double maxTankPressure;
	double maxPumpPressure;
	double maxPumpTemp;
	double minTankPressure;
	int openValveDelay;
	int motorPowerStepDelay;
	bool safetyActivated;
	
		
	int emergencyStopNovaPower;
	int emergencyStopDrainDuration;
	
	// bite
	SensorBiteNode* thrusterNode;

	friend class ValveAction;
	friend class ValveStateAction;
	friend class ConsPowerSensor;
	friend class IncMotorPowerAction;

};

#endif
