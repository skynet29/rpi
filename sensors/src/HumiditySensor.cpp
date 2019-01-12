#include "HumiditySensor.h"


HumiditySensor::HumiditySensor(int i2cAddr) : ADConverter(i2cAddr)
{
}


float HumiditySensor::GetValue()
{
	ushort conf = ADS1015_DEFAULT_CONF;
	ADS1015_REG_SET(conf, ADS1015_MUX, A1GND); // between A1 and GND
	//REG_SET(conf, ADS1015_PGA, 4_096);	
				
	return GetConvValue(conf);
	
}
