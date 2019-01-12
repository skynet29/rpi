#include "DSelect.h"

#include <stdlib.h>
#include <stdio.h>
#include "DLogger.h"

static DLogger log("DSelect");

class Data
{
public:
	Data(int fd, DSelectHandler cbk, void* cbkArg)
	{
		this->fd = fd;
		this->cbk =cbk;
		this->cbkArg = cbkArg;
	}
	
	void Fire()
	{
		cbk(fd, cbkArg);
	}
	
	int fd;
	DSelectHandler cbk;
	void* cbkArg;
};
	
uint DSelect::GetFdCount()
{
	return datas.GetCount();
}

int DSelect::GetFd(uint idx)
{
	return datas[idx]->fd;
}
	

bool DSelect::AddInput(int fd, DSelectHandler cbk, void* cbkArg)
{
	log.Write(DLogger::INFO, "AddInput:%d\n", fd);
	return datas.Add(new Data(fd, cbk, cbkArg));
}

bool DSelect::RemoveInput(int fd)
{
	log.Write(DLogger::INFO, "RemoveInput:%d\n", fd);
	for(uint i = 0; i < datas.GetCount(); i++)
	{
		if (datas[i]->fd == fd)
		{
			delete datas[i];
			datas.RemoveAt(i);
			return true;		
		}
	}	
	return false;
}

void DSelect::SetTimeMs(struct timeval& tm, int timeout)
{
	tm.tv_sec = timeout/1000;
	tm.tv_usec = (timeout%1000)*1000;	
}

int DSelect::WaitEvent(uint timeout)
{
	
	fd_set fdmask;
	FD_ZERO(&fdmask);
	
	int maxFd = 0;
	
	
	for(uint i = 0; i < datas.GetCount(); i++)
	{
		int fd = datas[i]->fd;
		if (fd > maxFd)
			maxFd = fd;
		FD_SET(fd, &fdmask);
	}
	
	
	struct timeval* ptm = NULL;
	struct timeval tm;
	if (timeout != 0)
	{
		SetTimeMs(tm, timeout);
		ptm = &tm;
	}
	
	int ret = select(maxFd+1, &fdmask, NULL, NULL, ptm);
	log.Write(DLogger::DEBUG, "select():%d, nbFd=%d", ret, datas.GetCount());
	
	if (ret < 0)
	{
		perror("select()\n");
		return -1;
	}
	
	if (ret == 0)
	{
		//printf("Timeout\n");
		return 0;
	}
	
	
	for(int i = datas.GetCount()-1; i >= 0; i--)
	{
		Data* data = datas[i];
		
		if (FD_ISSET(data->fd, &fdmask))
		{		
			data->Fire();
		}
			
	}	
	
	return 1;	
}
