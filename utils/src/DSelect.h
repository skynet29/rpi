#ifndef DSELECT_H
#define DSELECT_H

#include "DVector.h"

typedef void (*DSelectHandler)(int, void*);

class Data;

class DSelect
{
public:
	bool AddInput(int fd, DSelectHandler cbk, void* cbkArg);
	bool RemoveInput(int fd);
	
	int WaitEvent(uint timeout);
	uint GetFdCount();
	int GetFd(uint idx);
	
	static void SetTimeMs(struct timeval& tm, int timeout);
private:

	
	DVector<Data*> datas;
};

#endif
