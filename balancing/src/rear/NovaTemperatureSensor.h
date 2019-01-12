#ifndef NOVATEMPERATURESENSOR_H
#define NOVATEMPERATURESENSOR_H

#include "Sensor.h"
#include "NovaMotor.h"

class NovaTemperatureSensor : public Sensor
{
public:
	NovaTemperatureSensor(const string& name, NovaMotor& nova);
	
	virtual double GetRealValue();
	
private:
	NovaMotor* nova;
};

#endif
