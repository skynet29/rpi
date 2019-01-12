#ifndef VALVESTATEACTION_H
#define VALVESTATEACTION_H

#include "DSequencer.h"

class RearAgent;

class ValveStateAction : public DAbstractAction
{
public:
	
	ValveStateAction(RearAgent* agent, int valve);
private:
	RearAgent* agent;
	int valve;

	virtual bool OnStartup();	
};

#endif
