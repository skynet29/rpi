#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "DServerApp.h"
#include "DTimer.h"
#include <json/value.h>
#include <json/reader.h>



class App : public DServerApp
{
		
protected:

	bool OnRecvData(DSocket& client)
	{
		char buff[1024];
		int byteRead = client.Recv(buff, sizeof(buff));
		if (byteRead <= 0)
		{
			printf("Client disconnected\n");
			return false;
		}
		
		buff[byteRead] = 0;
		printf("buff=%s\n", buff);

		server.Broadcast(buff, &client);
			
		return true;
	}
		
};

int main(int argc, char* argv[])
{	
	App app;
	
	if (!app.ParseArg(argc, argv))
		return -1;
	
	return app.Run();
}


