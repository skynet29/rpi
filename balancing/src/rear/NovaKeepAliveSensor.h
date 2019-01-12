#ifndef NOVAKEEPALIVESENSOR_H
#define NOVAKEEPALIVESENSOR_H

#include "Sensor.h"
#include "NovaMotor.h"

class NovaKeepAliveSensor : public Sensor
{
public:
	NovaKeepAliveSensor(const string& name, NovaMotor& nova);
	
	virtual bool IsInRange();
private:
	NovaMotor* nova;
};

#endif
