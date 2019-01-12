#ifndef VALVEFILLALTERNATEACTION_H
#define VALVEFILLALTERNATEACTION_H

#include "DSequencer.h"
#include "DTimer.h"

class ValveController;

class ValveFillAlternateAction : public DAbstractAction
{
public:
	
	ValveFillAlternateAction(ValveController* controller, int duration, int period);
private:
	ValveController* controller;
	int duration;
	int period;
	DTimer tDuration;
	DTimer tPeriod;

	virtual bool OnStartup();	
	virtual DAction::Status OnRun();
};

#endif
