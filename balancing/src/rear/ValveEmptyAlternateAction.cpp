#include "ValveEmptyAlternateAction.h"

#include "ValveController.h"

ValveEmptyAlternateAction::ValveEmptyAlternateAction(ValveController* controller, int duration, int period)
{
	this->controller = controller;
	this->duration = duration;
	this->period = period;
}

bool ValveEmptyAlternateAction::OnStartup()
{
	 controller->SetState(ValveController::WEV | ValveController::RTDV | ValveController::THSV);
	 tDuration.Start(duration);
	 tPeriod.Start(period);
	 return true;
}

DAction::Status ValveEmptyAlternateAction::OnRun()
{
	if (tDuration.IsExpired())
	{
		 controller->SetState(0); // close all valve
		 return DAction::COMPLETED;		 
	}
	
	if (tPeriod.IsExpired())
	{
		controller->InvertState(ValveController::RTDV | ValveController::FTDV);
		tPeriod.Start(period);
	}
	
	
	return DAction::IN_PROG;		 
	
}
