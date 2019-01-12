#include <stdio.h>

#include "DSequencer.h"
#include "DThread.h"

static DAction::Status PrintString(DSequencer*seq, void* arg)
{
	printf("%s\n", (char*)arg);
	return DAction::COMPLETED;
}

int main(int argc, char* arv[])
{
	DSequencer seq;
	seq.AddAction(PrintString, (void*)"Hello World");
	seq.AddWaitAction(10000); // 10 sec
	seq.AddAction(PrintString, (void*)"Bye bye");
	seq.AddWaitAction(10000); // 10 sec
	seq.AddAction(PrintString, (void*)"Me revoioul");
	
	seq.Start();
	
	while (seq.Iterate() == DSequencer::IN_PROG)
	{
		DThread::Sleep(10); // 10 millisec
	}
	
	return 0;
}
