#include "StartMotorAction.h"

#include "NovaMotor.h"

StartMotorAction::StartMotorAction(NovaMotor* nova)
{
	this->nova = nova;
}

bool StartMotorAction::OnStartup()
{		
	return nova->StartFwd();
}
