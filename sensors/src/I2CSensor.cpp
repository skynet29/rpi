#include "I2CSensor.h"

#include <wiringPiI2C.h>
#include <stdio.h>
#include "DLogger.h"

static DLogger logger("I2CSensor");

I2CSensor::I2CSensor(const string& name) :
	Sensor(name)
{
	//fd = wiringPiI2CSetup(i2cAddr);		
	//printf("fd=%d\n", fd);
}

ushort I2CSensor::SwapShort(ushort val)
{
	char* p = (char*)&val;

	return (p[0] << 8) | p[1];	
}

bool I2CSensor::Init(Json::Value& root)
{
	if (!Sensor::Init(root))
		return false;
		
	if (!root["i2cAddr"].isInt())
		return false;
		
	int i2cAddr = root["i2cAddr"].asInt();
	
	fd = wiringPiI2CSetup(i2cAddr);		
	if (fd < 0)
	{
		logger.Write(DLogger::ERROR, "cannot open I2C device %s, at address %d", GetName().c_str(), i2cAddr);
		return false;
	}

	return true;
}
