#include "BuoySeq.h"
#include "StartMotorAction.h"
#include "ValveAction.h"

BuoySeq::BuoySeq(ValveController* valveCtrl, NovaMotor* nova, int valves, int power, int duration)
{
	AddAction(new StartMotorAction(nova, power));
	AddAction(new ValveAction(valveCtrl, valves));
	AddWaitAction(duration);
	AddAction(new ValveAction(valveCtrl, 0));
	AddAction(new StartMotorAction(nova, 0));

}
