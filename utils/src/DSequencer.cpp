#include "DSequencer.h"


DAbstractAction::DAbstractAction()
{
	seq = NULL;
}

void DAbstractAction::Init(DSequencer* seq)
{
	this->seq = seq;
	isFirstCall = true;	
}

DAbstractAction::Status DAbstractAction::Run()
{
	if (isFirstCall)
	{
		if (!OnStartup())
			return DAbstractAction::ERROR;
		isFirstCall = false;
	}
	return OnRun();
}

////////////////////////////////////////////////////////////////////////

DAction::DAction(Handler handler, void * arg)
{
	this->handler = handler;
	this->arg = arg;
}

DAction::Status DAction::OnRun()
{
	return handler(seq, arg);
}

////////////////////////////////////////////////////////////////////////
class WaitAction : public DAbstractAction
{
public:
	WaitAction(int delay)
	{
		this->delay = delay;
	}
protected:
	virtual bool OnStartup()
	{
		timer.Start(delay);
		return true;
	}
	
	virtual Status OnRun()
	{
		return (timer.GetRemainingTime() <= 0) ? DAbstractAction::COMPLETED : DAbstractAction::IN_PROG;	
	}
	
private:
	DTimer timer;
	int delay;
};



////////////////////////////////////////////////////////////////////////

DSequencer::DSequencer()
{
	Stop();
}

DSequencer::~DSequencer()
{
	for(uint i = 0; i < actions.GetCount(); i++)
		delete actions[i];
}

void DSequencer::AddAction(DAbstractAction* action)
{
	actions.Add(action);
}

void DSequencer::AddAction(DAction::Handler handler, void* arg)
{
	AddAction(new DAction(handler, arg));
}

void DSequencer::AddWaitAction(int delay)
{
	AddAction(new WaitAction(delay));
}



void DSequencer::Start()
{
	curAction = 0;
	state = IN_PROG;
	
	for(uint i = 0; i < actions.GetCount(); i++)
		actions[i]->Init(this);

}

void DSequencer::Stop()
{
	curAction = -1;
	state = NOT_RUNNING;
}


DSequencer::State DSequencer::Iterate()
{
	if (state != IN_PROG)
		return state;
		
	DAction::Status status = actions[curAction]->Run();
	
	if (status == DAction::ERROR)
	{
		state = ERROR;
	}
	
	else if (status == DAction::COMPLETED)
	{
		curAction++;
		if (curAction == (int)actions.GetCount()) // it was the last action ?
			state = COMPLETED;
	}
	
	return state;
}
