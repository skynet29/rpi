#ifndef DSOCKET_H
#define DSOCKET_H

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

class DSocket
{
public:
	DSocket(int sock);

	DSocket();

	~DSocket();

	virtual void Close();

	int Recv(char* buff, int bufLen);
	bool Send(const char* buff, int bufLen);
	bool Send(const char* msg);
	
	bool ReadLine(const char* separator, string& ret);	
	void AppendData(const char* data);


	bool SetNonBlocking(bool state);
	
	int GetFd() {return sock;}

protected:
	int sock;
	char buff[2048];
};



#endif
