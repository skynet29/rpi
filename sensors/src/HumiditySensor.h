#ifndef HUMIDITYSENSOR_H
#define HUMIDITYSENSOR_H

#include "ads1015.h"

class HumiditySensor : public ADConverter
{
public:
	HumiditySensor(int i2cAddr);
	
	float GetValue();
};

#endif
