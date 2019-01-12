#include "ConsPowerSensor.h"
#include "RearAgent.h"

ConsPowerSensor::ConsPowerSensor(const string& name, RearAgent* agent) :
	Sensor(name)
{
	this->agent = agent;
	this->type = "power";
}

double ConsPowerSensor::GetRealValue()
{
	NovaMotor::MonitorData data;
	agent->nova.GetMonitorData(data);
	
	return (data.voltage * agent->navCurrent);
}
