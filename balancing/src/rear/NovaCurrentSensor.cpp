#include "NovaCurrentSensor.h"

NovaCurrentSensor::NovaCurrentSensor(const string& name, NovaMotor& nova) :
	Sensor(name)
{
	this->nova = &nova;
	this->type = "current";
}

double NovaCurrentSensor::GetRealValue()
{
	NovaMotor::MonitorData data;
	nova->GetMonitorData(data);
	
	return data.current;
}
