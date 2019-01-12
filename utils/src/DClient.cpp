#include "DClient.h"

//typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
typedef struct hostent HOSTENT;

DClient::DClient()
{
	isConnected = false;	
}

void DClient::Close()
{
	DSocket::Close();
	isConnected = false;
}

bool DClient::SetServerInfo(const char* hostName, int port)
{
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	in_addr_t addr = inet_addr(hostName);

	if (addr == INADDR_NONE)
	{
		HOSTENT* phe = gethostbyname(hostName);
		if (phe == NULL)
			return false;

		sin.sin_addr = *(IN_ADDR*)phe->h_addr;
	}
	else
	{
		sin.sin_addr.s_addr = addr;
	}
	
	return true;
}

bool DClient::Connect(bool isBlocking)
{

	if (sock < 0)
		sock = socket(AF_INET, SOCK_STREAM, 0);
		
	if (!isBlocking)
		SetNonBlocking(true);
		
	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) < 0)
	{
		//perror("Connect()");
		return false;
	}
	isConnected = true;
	
	SetNonBlocking(false);
	
	return true;

}
