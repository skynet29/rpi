#ifndef DSEQUENCER_H
#define DSEQUENCER_H

#include "DVector.h"
#include "DTimer.h"

class DSequencer;


class DAbstractAction
{
public:
	enum Status {ERROR, IN_PROG, COMPLETED};
	
	DAbstractAction();
	
	virtual ~DAbstractAction() {}
		
	Status Run();
	void Init(DSequencer* seq);
	
	
protected:
	virtual Status OnRun() {return COMPLETED;}
	virtual bool OnStartup() {return true;}

	DSequencer* seq;
private:
	bool isFirstCall;
};

class DAction : public DAbstractAction
{
public:
	typedef Status (*Handler)(DSequencer*, void*);
	
	DAction(Handler handler, void * arg);
	
	virtual Status OnRun();
private:
	Handler handler;
	void* arg;

};

class DSequencer
{
public:
	enum State {ERROR, IN_PROG, COMPLETED, NOT_RUNNING};
	
	DSequencer();
	~DSequencer();
	
	void Start();
	void Stop();
	
	void AddAction(DAction::Handler handler, void* arg = NULL);
	void AddAction(DAbstractAction* action);
	void AddWaitAction(int delay);
	
	State Iterate();
	State GetState() {return state;}
private:
	DVector<DAbstractAction*> actions;
	int curAction;
	State state;
};


#endif
