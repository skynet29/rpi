#include "IncMotorPowerAction.h"

#include "NovaMotor.h"
#include "RearAgent.h"

IncMotorPowerAction::IncMotorPowerAction(RearAgent* agent,NovaMotor* nova)
{
	this->nova = nova;
	this->agent = agent;
}

bool IncMotorPowerAction::OnStartup()
{
    int valve = agent->valveCtrl.GetState();
	if (!agent->IsPressureSensorsOK(valve)) return false;
	
	return nova->IncPower();		
}
