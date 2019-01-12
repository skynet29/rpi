#ifndef DSERVER_H
#define DSERVER_H

#include "DSocket.h"
#include <stdlib.h>
#include <vector>

using namespace std;

class DServer : public DSocket
{
public:	
	DServer();
	
	bool Listen(int port, int maxCon);
	DSocket* Accept();
	
	void Broadcast(const char* msg, DSocket* except = NULL);
	uint GetClientCount();
	DSocket* Find(int fd);
	bool Remove(DSocket* client);

	
private:
	vector<DSocket*> clients;
	
};


#endif
