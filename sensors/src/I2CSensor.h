#ifndef I2CSENSOR_H
#define I2CSENSOR_H

#include "Sensor.h"
#include <stdlib.h>


class I2CSensor : public Sensor
{
public:
	I2CSensor(const string& name);
	
	static ushort SwapShort(ushort val);
	
	virtual bool Init(Json::Value& root);
protected:
	int fd;	
};

#endif
