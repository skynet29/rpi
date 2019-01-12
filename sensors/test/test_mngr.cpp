//#include "HumiditySensor.h"

#include "SensorManager.h"


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <json/reader.h>

int main(int argc, char* argv[])
{
		
	if (argc != 2)
	{
		printf("Usage: %s fileName\n", argv[0]);
		return -1;
	}
	
		ifstream in;
		in.open(argv[1], ifstream::in);
		if (!in.good())
		{
			cout << "Failed to open file " << argv[1] << endl;
			return -1;
		}	
	
		Json::Reader reader;
		Json::Value root;
		
		if (!reader.parse(in, root))
		{
			cout << "Failed  to parse config file: " << reader.getFormatedErrorMessages() << endl;
			return -1;
		}	
			
	
	system("i2cdetect -y 1");
	
	
	//HumiditySensor sensor(addr);
	SensorManager sensorMngr;
	if (!sensorMngr.Init(root))
	{
		printf("init sensors failed\n");
		return -1;
	}
	
	
	while (1)
	{	
		for(int i = 0; i < sensorMngr.GetSensorCount(); i++)
		{
			Sensor* s = sensorMngr.GetSensorAt(i);
			printf("name=%s, val=%f\n", s->GetName().c_str(), s->GetValue());
		}
		
		sleep(1); //wait 1 sec;
	}
	

	return 0;
}
