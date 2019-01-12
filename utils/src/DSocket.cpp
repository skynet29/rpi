#include "DSocket.h"


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "DServer.h"
#include "DLogger.h"
#include "DSelect.h"



static DLogger log("DSocket");


DSocket::DSocket(int sock)
{
	this->sock = sock;

	buff[0] = 0;
}

DSocket::DSocket()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	buff[0] = 0;

}

DSocket::~DSocket()
{
	Close();
}

void DSocket::Close()
{
	close(sock);
	sock = -1;
	buff[0] = 0;
}

int DSocket::Recv(char* buff, int bufLen)
{	
	int ret = recv(sock, buff, bufLen, 0);
	return ret;

}

bool DSocket::Send(const char* buff, int bufLen)
{
	struct timeval tm;
	DSelect::SetTimeMs(tm, 10); // 10 ms
	
	int rc = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tm, sizeof(tm));
	if ( rc < 0 )
	{	
		perror("setsockopt() SO_SNDTIMEO");
		return false;
	}

	while( bufLen > 0 )
	{
		int nbrOfBytesWritten = send(sock, buff, bufLen, 0);
		if ( nbrOfBytesWritten < 0 )
			return false;

		bufLen -= nbrOfBytesWritten;
		buff += nbrOfBytesWritten;
	}
    
  return true;
}

bool DSocket::Send(const char* msg)
{
	return Send(msg, strlen(msg));
}








bool DSocket::ReadLine(const char* separator, string& ret)
{
	char* p = strstr(buff, separator);
	if (p == NULL)
		return false;
		
	*p++ = 0;

	ret = buff;
	
	memmove(buff, p, strlen(p)+1);
	return true;
}

void DSocket::AppendData(const char* data)
{
	if (strlen(buff) + strlen(data) > sizeof(buff))
	{
		log.Write(DLogger::ERROR, "buffer overflow");
		buff[0] = 0;
		return;
	}
	strcat(buff, data);
}

bool DSocket::SetNonBlocking(bool state)
{
	int flags = fcntl(sock, F_GETFL, NULL);
	
	if (flags < 0)
	{ 
		perror("SetNonBlocking: fcntl(..., F_GETFL)"); 
		return false;
	} 	
	
	if (state)
		flags |= O_NONBLOCK; 
	else
		flags &= ~O_NONBLOCK;
		 
	
	if (fcntl(sock, F_SETFL, flags) < 0)
	{ 
		perror("SetNonBlocking: fcntl(..., F_SETFL)"); 
		return false;
	} 	
	
	return true;
}
