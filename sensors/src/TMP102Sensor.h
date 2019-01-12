#ifndef TMP102SENSOR_H
#define TMP102SENSOR_H

#include "I2CSensor.h"

class TMP102Sensor : public I2CSensor
{
public:
	TMP102Sensor(const string& name);
	
	virtual double GetRealValue();
	
	virtual bool Init(Json::Value& root);
private:
	double k;
	
};

#endif
