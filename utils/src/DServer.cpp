#include "DServer.h"

#include <stdio.h>
#include <stdlib.h>


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <netdb.h>

#include <unistd.h>

#include "DServer.h"
#include "DLogger.h"

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
typedef struct hostent HOSTENT;

static DLogger log("DServer");

DServer::DServer()
{
}

uint DServer::GetClientCount()
{
	return clients.size();
}




void DServer::Broadcast(const char* msg, DSocket* except)
{
	for(uint i = 0; i < clients.size(); i++)
	{
		if (clients[i] != except)
			clients[i]->Send(msg);
	}	
}


bool DServer::Listen(int port, int maxCon)
{
	SOCKADDR_IN sin;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	
	
	const int set = 1;

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                  (char *)&set, sizeof(set)) < 0 )
    {
		perror("setsockopt SO_REUSEADDR");
		return false;
    }	
    
    
	if (bind(sock, (SOCKADDR*)&sin, sizeof(sin)) < 0)
	{
		perror("bind()");
		close(sock);
		return false;
	}

	if (listen(sock, maxCon) < 0)
	{
		perror("listen()");
		close(sock);
		return false;
	}

	return true;
	
}

DSocket* DServer::Accept()
{
	int csock = accept(sock, NULL, NULL);
	if (csock < 0) 
		return NULL;
		
	DSocket* client = new DSocket(csock);
	clients.push_back(client);
	log.Write(DLogger::INFO, "Add client %p\n", client);
	return client;
}


bool DServer::Remove(DSocket* client)
{
	log.Write(DLogger::INFO, "Remove client %p\n", client);

	for(uint i = 0; i < clients.size(); i++)
	{
		if (clients[i] == client)
		{
			clients.erase(clients.begin() + i);
			return true;
		}
	}
	return false;
}

DSocket* DServer::Find(int fd)
{
	for(uint i = 0; i < clients.size(); i++)
	{
		if (clients[i]->GetFd() == fd)
		{
			return clients[i];
		}
	}
	return NULL;	
}
