#ifndef INCMOTORPOWERACTION_H
#define INCMOTORPOWERACTION_H

#include "DSequencer.h"

class NovaMotor;
class RearAgent;

class IncMotorPowerAction : public DAbstractAction
{
public:
	IncMotorPowerAction(RearAgent* agent,NovaMotor* nova);
private:
	NovaMotor* nova;
	RearAgent* agent;
	
protected:
	virtual bool OnStartup();
};

#endif
