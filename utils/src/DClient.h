#ifndef DCLIENT_H
#define DCLIENT_H

#include "DSocket.h"



class DClient : public DSocket
{
public:
	DClient();
	
	bool Connect(bool isBlocking = true);
	bool IsConnected() {return isConnected;}
	bool SetServerInfo(const char* hostName, int port);
	
	virtual void Close();
protected:
	bool isConnected;
	struct sockaddr_in sin;
	
};

#endif
