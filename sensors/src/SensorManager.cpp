#include "SensorManager.h"
#include "ads1015.h"
#include "TMP102Sensor.h"
#include "DLogger.h"

static DLogger logger("SensorManager");

bool SensorManager::Init(Json::Value& root)
{
	if (!root.isArray())
		return false;
		
	for(unsigned i = 0; i < root.size(); i++)
	{
		Json::Value node = root[i];
		if (!node.isObject())
			return false;
			
		if (!node["name"].isString())
			return false;			
		string name = node["name"].asString();
		
		if (!node["device"].isString())
			return false;			
		string device = node["device"].asString();
		
		Sensor* sensor = NULL;
		
		if (device == "TMP102")
			sensor = new TMP102Sensor(name);

		else if (device == "ADS1015")
			sensor = new ADConverter(name);

		else
		{
			logger.Write(DLogger::ERROR, "Unknown sensor device %s", device.c_str());
			continue;
		}
			
		
		if (!sensor->Init(node))
		{
			logger.Write(DLogger::ERROR, "Fail to init sensor: %s", name.c_str());
			delete sensor;
			return false;
		}
			
		sensors.push_back(sensor);
	}
	
	return true;
}

Sensor* SensorManager::GetSensorAt(int idx)
{
	return sensors[idx];
}


int SensorManager::GetSensorCount()
{
	return sensors.size();
}

Sensor* SensorManager::GetSensorByName(const string& name)
{
	for(unsigned i = 0; i < sensors.size(); i++)
	{
		if (sensors[i]->GetName() == name)
			return sensors[i];
	}
	return NULL;
}

bool SensorManager::GetSensorValue(const string& sensorName, double& value)
{
	Sensor* s = GetSensorByName(sensorName);
	if (s)
	{
		value = s->GetValue();
		return true;
	}
	
	return false;
}
