#include "SensorBiteNode.h"
#include "DLogger.h"

static DLogger logger("SensorBiteNode");

SensorBiteNode::SensorBiteNode(const string& nodeName) :
	BiteNode(nodeName)
{
}


void SensorBiteNode::AddSensor(Sensor* s)
{
	if (s != NULL)
		sensors.push_back(s);
}

bool SensorBiteNode::CheckSensor(Sensor* s)
{
	bool oldStatus = s->GetStatus();

	if (s->IsInRange())
	{
		s->SetStatus(true);

		if (s->GetType() == "temperature")
			AddMessage("sensors.temp")->AddParam(s->GetValue());
			
		else if (s->GetType() == "pressure")
			AddMessage("sensors.pressure")->AddParam(s->GetValue());
			
		else if (s->GetType() == "waterleakage")
			AddMessage("sensors.waterleakage");
			
		else if (s->GetType() == "voltage")
			AddMessage("thruster.v")->AddParam(s->GetValue());
			
		else if (s->GetType() == "current")
			AddMessage("thruster.i")->AddParam(s->GetValue());
			
		else if (s->GetType() == "power")
			AddMessage("battery.w")->AddParam(s->GetValue());
			
		else if (s->GetType() == "cnx")
			AddMessage("thruster.conx");
	}
	else
	{
		s->SetStatus(false);

		if (s->GetType() == "temperature")
			AddMessage("sensors.temp_error")->AddParam(s->GetValue())->AddParam(s->GetMaxValue());
			
		else if (s->GetType() == "pressure")		
			AddMessage("sensors.pressure_error")->AddParam(s->GetValue())->AddParam(s->GetMaxValue())->AddParam(s->GetMinValue());
			
		else if (s->GetType() == "waterleakage")
			AddMessage("sensors.waterleakage_error");
			
		else if (s->GetType() == "voltage")
			AddMessage("thruster.v_error")->AddParam(s->GetValue())->AddParam(s->GetMaxValue());
			
		else if (s->GetType() == "current")
			AddMessage("thruster.i_error")->AddParam(s->GetValue())->AddParam(s->GetMaxValue());
			
		else if (s->GetType() == "power")
			AddMessage("battery.w_error")->AddParam(s->GetValue())->AddParam(s->GetMaxValue());
			
		else if (s->GetType() == "cnx")
			AddMessage("thruster.conx_error");
	}
	
	return (oldStatus != s->GetStatus());
}


void SensorBiteNode::Compute(DSocket& biteClient, bool forceSending)
{
	ClearAllMessage();
	SetStatus(OK);
	
	bool needSending = forceSending;
	
	for(unsigned i = 0; i < sensors.size(); i++)
	{
		if (CheckSensor(sensors[i])) // if sensor status has changed force sending
			needSending = true;
			
		logger.Write(DLogger::DEBUG, "sensor: name=%s, value=%f\n", sensors[i]->GetName().c_str(),
			sensors[i]->GetValue());
			
		if (!sensors[i]->GetStatus()) // sensor KO
			SetStatus(ERROR); // modify node status
		
	}
	
	if (needSending)
		Send(biteClient);
}


Sensor* SensorBiteNode::GetSensorByName(const string& name)
{
	for(unsigned i = 0; i < sensors.size(); i++)
	{
		if (sensors[i]->GetName() == name)
			return sensors[i];
	}
	return NULL;
}

