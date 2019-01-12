#include "NovaKeepAliveSensor.h"

NovaKeepAliveSensor::NovaKeepAliveSensor(const string& name, NovaMotor& nova) :
	Sensor(name)
{
	this->nova = &nova;
	this->type = "cnx";
}

bool NovaKeepAliveSensor::IsInRange()
{
	return nova->IsAlive();
}
 
