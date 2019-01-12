#include "ValveStateAction.h"

#include "RearAgent.h"



ValveStateAction::ValveStateAction(RearAgent* agent, int valve)
{
	this->agent = agent;
	this->valve = valve;
}

bool ValveStateAction::OnStartup()
{
	 agent->valveCtrl.SetState(valve);
	 
	 return true;
}

