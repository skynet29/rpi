#ifndef NOVACURRENTSENSOR_H
#define NOVACURRENTSENSOR_H

#include "Sensor.h"
#include "NovaMotor.h"

class NovaCurrentSensor : public Sensor
{
public:
	NovaCurrentSensor(const string& name, NovaMotor& nova);
	
	virtual double GetRealValue();
	
private:
	NovaMotor* nova;
};

#endif
