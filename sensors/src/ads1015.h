#ifndef ADS1015_H
#define ADS1015_H

#include "I2CSensor.h"

// Input multiplexer config
#define ADS1015_MUX_OFF		12
#define ADS1015_MUX_BIT		3

#define ADS1015_MUX_A0GND 	0b100
#define ADS1015_MUX_A1GND 	0b101
#define ADS1015_MUX_A2GND 	0b110
#define ADS1015_MUX_A3GND 	0b111
#define ADS1015_MUX_A0A1 	0b000
#define ADS1015_MUX_A0A3 	0b001
#define ADS1015_MUX_A1A3 	0b010
#define ADS1015_MUX_A2A3 	0b011

// Data rate
#define ADS1015_DR_OFF		5
#define ADS1015_DR_BIT		3

#define ADS1015_DR_128		0b000
#define ADS1015_DR_250		0b001
#define ADS1015_DR_490		0b010
#define ADS1015_DR_920		0b011
#define ADS1015_DR_1600		0b100 // default
#define ADS1015_DR_2400		0b101
#define ADS1015_DR_3300		0b110


// Programmable amplifier gain
#define ADS1015_PGA_OFF		9
#define ADS1015_PGA_BIT		3

#define ADS1015_PGA_6_144	0b000
#define ADS1015_PGA_4_096	0b001
#define ADS1015_PGA_2_048	0b010 // default
#define ADS1015_PGA_1_024	0b011
#define ADS1015_PGA_0_512	0b100
#define ADS1015_PGA_0_256	0b101

#define ADS1015_MODE_OFF		8
#define ADS1015_MODE_BIT		1
#define ADS1015_MODE_CONTINUOUS		0
#define ADS1015_MODE_SINGLE_SHOT	1

#define ADS1015_COMP_MODE_OFF		4
#define ADS1015_COMP_MODE_BIT		1
#define ADS1015_COMP_MODE_HYSTERESIS	0 // default
#define ADS1015_COMP_MODE_WINDOW		1

#define ADS1015_COMP_POL_OFF		3
#define ADS1015_COMP_POL_BIT		1
#define ADS1015_COMP_POL_ACTIVE_LOW		0 // default
#define ADS1015_COMP_POL_ACTIVE_HIGH	1

#define ADS1015_COMP_LAT_OFF		2
#define ADS1015_COMP_LAT_BIT		1
#define ADS1015_COMP_LAT_NO_LATCHING	0 // default
#define ADS1015_COMP_LAT_LATCHING		1

#define ADS1015_COMP_QUE_OFF		0
#define ADS1015_COMP_QUE_BIT		2
#define ADS1015_COMP_QUE_1_CONV			0b00
#define ADS1015_COMP_QUE_2_CONV			0b01
#define ADS1015_COMP_QUE_4_CONV			0b10
#define ADS1015_COMP_QUE_DISABLE		0b11 //default

#define ADS1015_MASK(nbit) \
	((1 << nbit)-1)

#define ADS1015_BIT_SET(var, val, off, nbit) \
	var &= ~(ADS1015_MASK(nbit)<< off); \
	var |= val << off;	

#define ADS1015_BIT_GET(var, off, nbit) \
	((var >> off) & ADS1015_MASK(nbit))

#define ADS1015_DEFAULT_CONF	0x8583

#define ADS1015_REG_SET(var, prefix, val) \
	ADS1015_BIT_SET(var, prefix##_##val, prefix##_OFF, prefix##_BIT);

#define ADS1015_REG_GET(var, prefix) \
	ADS1015_BIT_GET(var, prefix##_OFF, prefix##_BIT)



class ADConverter : public I2CSensor
{
public:
	ADConverter(const string& name);
	
	virtual double GetRealValue();
	virtual bool Init(Json::Value& root);

private:
	int conf;
	double a;
	double b;  // coeff transformation linear ax + b
};

#endif
