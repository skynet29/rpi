#ifndef STOPMOTORACTION_H
#define STOPMOTORACTION_H

#include "DSequencer.h"

class NovaMotor;

class StopMotorAction : public DAbstractAction
{
public:
	StopMotorAction(NovaMotor* nova);
private:
	NovaMotor* nova;
protected:
	virtual bool OnStartup();
};

#endif
