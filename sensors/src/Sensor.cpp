#include "Sensor.h"

Sensor::Sensor(const string& name)
{
	this->name = name;
	hasMaxValue = false;
	hasMinValue = false;
	minValue = 0.0;
	maxValue = 0.0;	
	nbSamples = 1;
	nbDecimal = 2;
}

bool Sensor::Init(Json::Value& root)
{
	if (root["minValue"].isDouble())
	{
		hasMinValue = true;
		minValue = root["minValue"].asDouble();
	}
		
	if (root["maxValue"].isDouble())
	{
		hasMaxValue = true;
		maxValue = root["maxValue"].asDouble();
	}

	if (root["type"].isString())
	{
		type = root["type"].asString();
	}

	if (root["node"].isString())
	{
		node = root["node"].asString();
	}
	
	if (root["nbSamples"].isInt())
	{
		nbSamples = root["nbSamples"].asInt();
	}
		
	if (root["nbDecimal"].isInt())
	{
		nbDecimal = root["nbDecimal"].asInt();
	}	
	
	return true;
}

bool Sensor::IsInRange()
{
	if (hasMinValue && GetValue() < minValue)
		return false;

	if (hasMaxValue && GetValue() > maxValue)
		return false;

	return true;
}

void Sensor::SetStatus(bool status)
{
	this->status = status;
}

void Sensor::SetMaxValue(double maxValue)
{
	this->maxValue = maxValue;
	hasMaxValue = true;
}

double Sensor::GetValue()
{
	if (samples.size() >= nbSamples)
		samples.erase(samples.begin()); // remove the first (older) sample
		
	samples.push_back(TRUNK(GetRealValue(), nbDecimal));
	
	// compute average on last nbSamples value
	double average = 0.0;	
	for(unsigned i = 0; i < samples.size(); i++)
		average += samples[i];
		
	average /= samples.size();	
	return average;
}
