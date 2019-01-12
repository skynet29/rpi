#ifndef VALVEEMPTYALTERNATEACTION_H
#define VALVEEMPTYALTERNATEACTION_H


#include "DSequencer.h"
#include "DTimer.h"

class ValveController;

class ValveEmptyAlternateAction : public DAbstractAction
{
public:
	
	ValveEmptyAlternateAction(ValveController* controller, int duration, int period);
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
