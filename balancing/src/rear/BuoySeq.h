#ifndef BUOYSEQ_H
#define BUOYSEQ_H

#include "DSequencer.h"

class ValveController;
class NovaMotor;

class BuoySeq : public DSequencer
{
public:
	BuoySeq(ValveController* valveCtrl, NovaMotor* nova, int valves, int power, int duration);
};

#endif
