#include "ValveFillAlternateAction.h"

#include "ValveController.h"

ValveFillAlternateAction::ValveFillAlternateAction(ValveController* controller, int duration, int period)
{
	this->controller = controller;
	this->duration = duration;
	this->period = period;
}

bool ValveFillAlternateAction::OnStartup()
{
	 controller->SetState(ValveController::WIV | ValveController::RTFV | ValveController::THSV);
	 tDuration.Start(duration);
	 tPeriod.Start(period);
	 return true;
}

DAction::Status ValveFillAlternateAction::OnRun()
{
	if (tDuration.GetRemainingTime() <= 0)
	{
		 controller->SetState(0); // close all valve
		 return DAction::COMPLETED;		 
	}
	
	if (tPeriod.GetRemainingTime() <= 0)
	{
		controller->InvertState(ValveController::RTFV | ValveController::FTFV);
		tPeriod.Start(period);
	}
	
	
	return DAction::IN_PROG;		 
	
}
