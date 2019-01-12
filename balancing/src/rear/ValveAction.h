#ifndef VALVEACTION_H
#define VALVEACTION_H

#include "DSequencer.h"
#include "DTimer.h"

class RearAgent;

class ValveAction : public DAbstractAction
{
public:
	
	ValveAction(RearAgent* agent, int valve, int duration);
	ValveAction(RearAgent* agent, int valve, int duration, float vol);
private:
	RearAgent* agent;
	int valve;

	DTimer tDuration;
	int duration;
	double vol;

	virtual bool OnStartup();	
	virtual DAction::Status OnRun();
};

#endif
