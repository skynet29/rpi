#include "ValveAction.h"
#include "RearAgent.h"
#include "MonitoringVolume.h"

#include "ValveController.h"


ValveAction::ValveAction(RearAgent* agent, int valve, int duration)
{
	ValveAction(agent,valve,duration,0.0);
}

ValveAction::ValveAction(RearAgent* agent, int valve, int duration, float vol)
{
	this->agent = agent;
	this->valve = valve;
	this->duration = duration;	
	this->vol = vol;
}

bool ValveAction::OnStartup()
{
	 agent->valveCtrl.SetState(valve);
	 if (duration > 0) tDuration.Start(duration);
	 
	 return true;
}


DAction::Status ValveAction::OnRun()
{
	bool bEnd = false;
	
	bEnd = !agent->IsPressureSensorsOK(valve);
	if (duration > 0) {
		bEnd = bEnd || tDuration.IsExpired();
	}
	
	if (vol > 0.0)
	{
	    MonitoringVolume *p_MonitoringVolume = MonitoringVolume::instance();
	    bEnd = bEnd || (p_MonitoringVolume->getVol() >= vol);
	}
    
    
	if (bEnd)
	{
		agent->valveCtrl.SetState(0); // close all valve
		return DAction::COMPLETED;		 		
	}
		
	return DAction::IN_PROG;		 
	
}
