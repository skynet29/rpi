#include "ads1015.h"
#include <wiringPiI2C.h>
#include <stdio.h>
#include "DThread.h"

static float gains[] = {6.144, 4.096, 2.048, 1.024, 0.512, 0.256};

/*
ushort SwapShort(ushort val)
{
	char* p = (char*)&val;

	return (p[0] << 8) | p[1];	
}
*/


ADConverter::ADConverter(const string& name) :
	I2CSensor(name)
{
	conf = ADS1015_DEFAULT_CONF;
	a = 1.0;
	b = 0.0;
}

bool ADConverter::Init(Json::Value& root)
{
	if (!I2CSensor::Init(root))
		return false;
		
	if (root["mux"].isInt())
	{
		ADS1015_BIT_SET(conf, root["mux"].asInt(), ADS1015_MUX_OFF, ADS1015_MUX_BIT);
	}

	if (root["gain"].isInt())
	{
		ADS1015_BIT_SET(conf, root["gain"].asInt(), ADS1015_PGA_OFF, ADS1015_PGA_BIT);
	}

	if (root["a"].isDouble())
		a = root["a"].asDouble();

	if (root["b"].isDouble())
		b = root["b"].asDouble();
	
	return true;
}


double ADConverter::GetRealValue()
{	
	wiringPiI2CWriteReg16(fd, 0x01, SwapShort(conf));
	DThread::Sleep(10); // 10ms

	int val = wiringPiI2CReadReg16(fd, 0x00);

	//printf("val=%d, 0x%X\n", val, val);


	ushort val2 = SwapShort(val) >> 4;
	//printf("val2=%d, 0x%X\n", val2, val2);
	if (val2 & 0x800)
	{
		//printf("Valeur negative\n");
		val2 |= 0xF000;
	}
	
	//printf("gain=%f\n", gains[ADS1015_REG_GET(conf, ADS1015_PGA)]);
	
	double val3 = ((short)val2) * gains[ADS1015_REG_GET(conf, ADS1015_PGA)]/2048.0;
	return (val3 * a) + b;
	
}
