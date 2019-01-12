#ifndef DSERVERAPP_H
#define DSERVERAPP_H

#include "DApplication.h"

#include "DSelect.h"
#include "DServer.h"

#include <stdlib.h>
#include <vector>


class DClient;

class DServerApp : public DApplication, public DSelect
{
public:
	DServerApp();
	
	
protected:
	virtual bool OnStartup();
	virtual void OnIterate();
	virtual void OnExit();	
	virtual bool OnConfig(Json::Value& root);
	
	DClient* Subscribe(const char* hostName, int port);		
	
	void OnNewClient();
	virtual void OnRecvData(DSocket* client, const char* msg) {}
	
	DServer server;
private:

	int port;
	int maxClient;
	int timeout;
	vector<DClient*> clients;

	void OnRecv(int fd);	
	DSocket* FindSocket(int fd, bool& isFromServer);
	
	static void NewClient(int fd, void* arg);
	static void RecvData(int fd, void* arg);
};

#endif
