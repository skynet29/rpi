#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <vector>
#include <json/value.h>
#include "Sensor.h"

using namespace std;


class SensorManager
{
public:
	bool Init(Json::Value& root);
	Sensor* GetSensorAt(int idx);
	Sensor* GetSensorByName(const string& name);
	bool GetSensorValue(const string& sensorName, double& value);
	int GetSensorCount();
private:
	vector<Sensor*> sensors;
};

#endif
