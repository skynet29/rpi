#include "StopMotorAction.h"

#include "NovaMotor.h"

StopMotorAction::StopMotorAction(NovaMotor* nova)
{
	this->nova = nova;
}

bool StopMotorAction::OnStartup()
{
	return nova->Stop();		
}
