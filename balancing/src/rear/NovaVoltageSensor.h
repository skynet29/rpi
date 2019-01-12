#ifndef NOVAVOLTAGESENSOR_H
#define NOVAVOLTAGESENSOR_H

#include "Sensor.h"
#include "NovaMotor.h"

class NovaVoltageSensor : public Sensor
{
public:
	NovaVoltageSensor(const string& name, NovaMotor& nova);
	
	virtual double GetRealValue();
	
private:
	NovaMotor* nova;
};

#endif
