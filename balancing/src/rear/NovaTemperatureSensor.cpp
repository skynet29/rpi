#include "NovaTemperatureSensor.h"

NovaTemperatureSensor::NovaTemperatureSensor(const string& name, NovaMotor& nova) :
	Sensor(name)
{
	this->nova = &nova;
	this->type = "temperature";
}

double NovaTemperatureSensor::GetRealValue()
{
	NovaMotor::MonitorData data;
	nova->GetMonitorData(data);
	
	return data.tempMot;
}
