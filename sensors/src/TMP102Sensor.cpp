#include "TMP102Sensor.h"

#include <wiringPiI2C.h>
#include <stdio.h>

TMP102Sensor::TMP102Sensor(const string& name) :
	I2CSensor(name)
{
	k = 0.0625;
}


double TMP102Sensor::GetRealValue()
{
	int val = wiringPiI2CReadReg16(fd, 0x00);
	//printf("val=%d\n", val);
	return (SwapShort(val) >> 4) * k;
	
}

bool TMP102Sensor::Init(Json::Value& root)
{
	if (!I2CSensor::Init(root))
		return false;
		
	if (root["k"].isDouble())
	{
		k = root["k"].asDouble();
		printf("k=%f\n", k);
	}
	return true;
}
