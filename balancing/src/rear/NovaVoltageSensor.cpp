#include "NovaVoltageSensor.h"

NovaVoltageSensor::NovaVoltageSensor(const string& name, NovaMotor& nova) :
	Sensor(name)
{
	this->nova = &nova;
	this->type = "voltage";
}

double NovaVoltageSensor::GetRealValue()
{
	NovaMotor::MonitorData data;
	nova->GetMonitorData(data);
	
	return data.voltage;
}
