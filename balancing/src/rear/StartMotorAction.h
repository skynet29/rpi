#ifndef STARTMOTORACTION_H
#define STARTMOTORACTION_H

#include "DSequencer.h"

class NovaMotor;

class StartMotorAction : public DAbstractAction
{
public:
	StartMotorAction(NovaMotor* nova);
private:
	NovaMotor* nova;

protected:
	virtual bool OnStartup();
};

#endif
