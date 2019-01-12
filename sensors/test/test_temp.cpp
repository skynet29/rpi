#include "TMP102Sensor.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <json/reader.h>
#include <fstream>

int main(int argc, char* argv[])
{
	/*
	if (argc != 2)
	{
		printf("Usage: %s i2cAddr\n", argv[0]);
		return -1;
	}
	int addr = strtol(argv[1], NULL, 16);
	*/

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
	
	
	TMP102Sensor sensor("toto");
	sensor.Init(root);
	
	while (1)
	{	
		printf("temp=%lf\n", sensor.GetValue());
		
		sleep(1); //wait 1 sec;
	}
	

	return 0;
}
