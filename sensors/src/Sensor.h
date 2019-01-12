#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <json/value.h>
#include <vector>
#include <math.h>

using namespace std;

#define TRUNK(val, digit) \
	((int)((val)*pow(10, digit)))/pow(10, digit)
	
class Sensor
{
public:
	Sensor(const string& name);
	string GetName() {return name;}
	string GetType() {return type;}
	string GetNode() {return node;}
	
	double GetValue();
	virtual double GetRealValue() {return 0.0;}
	virtual bool Init(Json::Value& root);
	virtual bool IsInRange();
	double GetMaxValue() {return maxValue;}
	double GetMinValue() {return minValue;}
	bool GetStatus() {return status;}
	void SetStatus(bool status);
	void SetMaxValue(double maxValue);
protected:
	string name;
	string type;
	string node;
	bool hasMaxValue;
	bool hasMinValue;
	double minValue;
	double maxValue;
	bool status;
	vector<double> samples;
	unsigned nbSamples;
	unsigned nbDecimal;
};

#endif
