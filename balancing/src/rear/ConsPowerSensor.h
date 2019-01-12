#ifndef CONSPOWERSENSOR_H
#define CONSPOWERSENSOR_H

#include "Sensor.h"

class RearAgent;

class ConsPowerSensor : public Sensor
{
public:
	ConsPowerSensor(const string& name, RearAgent* agent);
	
	virtual double GetRealValue();
	
private:
	RearAgent* agent;
};

#endif
