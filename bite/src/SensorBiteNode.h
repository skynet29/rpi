#ifndef SENSORBITENODE_H
#define SENSORBITENODE_H

#include "BiteNode.h"
#include "Sensor.h"

class SensorBiteNode : public BiteNode
{
public:
	SensorBiteNode(const string& nodeName);
	void AddSensor(Sensor* s);
	void Compute(DSocket& biteClient, bool forceSending);
	Sensor* GetSensorByName(const string& name);
private:
	vector<Sensor*> sensors;
	bool CheckSensor(Sensor* s);
};

#endif
