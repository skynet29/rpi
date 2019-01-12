#include "DServerApp.h"
#include "DLogger.h"
#include "DClient.h"

#include <stdio.h>


static DLogger log("DServerApp");

DServerApp::DServerApp()
{
	maxClient = 0; // no limitation
	timeout = 100; // 100 ms	
	AddInput(server.GetFd(), NewClient, this);
}

bool DServerApp::OnStartup()
{
	return server.Listen(port, 5);
}

void DServerApp::NewClient(int fd, void* arg)
{
	log.Write(DLogger::INFO, "NewClient()");
	((DServerApp*)arg)->OnNewClient();
}

void DServerApp::OnNewClient()
{
	DSocket* client = server.Accept();
	log.Write(DLogger::DEBUG, "ClientCount=%d", server.GetClientCount());
	if (client == NULL)
		return;
				
	if (maxClient != 0 && (int)server.GetClientCount() > maxClient)	
	{
		log.Write(DLogger::WARNING, "Too much client");
		delete client;
		return;		
	}
	
	AddInput(client->GetFd(), RecvData, this);


}



DClient* DServerApp::Subscribe(const char* hostName, int port)
{
	DClient* client = new DClient();
	if (!client->SetServerInfo(hostName, port))
	{
		delete client;
		return NULL;
	}
	clients.push_back(client);
	return client;
}

DSocket* DServerApp::FindSocket(int fd, bool& isFromServer)
{
	isFromServer = false;

	DSocket* client = server.Find(fd);
	if (client != NULL)
	{
		isFromServer = true;
		return client;
	}
	
	for(unsigned i = 0; i < clients.size(); i++)
	{
		if (clients[i]->GetFd() == fd)
			return clients[i];
	}
	return NULL;
}

void DServerApp::OnRecv(int fd)
{
	bool isFromServer;
	DSocket* client = FindSocket(fd, isFromServer);
	
	if (client == NULL)
	{
		log.Write(DLogger::ERROR, "Client not found");
		return;
	}
		
	char buff[1024];
	int byteRead = client->Recv(buff, sizeof(buff));
	if (byteRead <= 0)
	{
		log.Write(DLogger::INFO, "Client disconnected");
		RemoveInput(fd);
		if (isFromServer)
		{
			server.Remove(client);
			delete client;
		}
		else
			client->Close();
	}
	else
	{	
		buff[byteRead] = 0;
		log.Write(DLogger::DEBUG, "Buff=%s", buff);
		client->AppendData(buff);
		
		string msg;
		while (client->ReadLine("\n", msg))
		{
			log.Write(DLogger::DEBUG, "Msg=%s", msg.c_str());
			OnRecvData(client, msg.c_str());
		}
	}
		

}

void DServerApp::RecvData(int fd, void* arg)
{
	log.Write(DLogger::DEBUG, "RecvData()");
		
	((DServerApp*)arg)->OnRecv(fd);
}



void DServerApp::OnIterate()
{
	//printf("OnIterate()\n");	
	int ret = WaitEvent(timeout);
	if (ret < 0)
		Exit();
		
	for(unsigned i = 0; i < clients.size(); i++)
	{
		if (!clients[i]->IsConnected() && clients[i]->Connect(false))
			AddInput(clients[i]->GetFd(), RecvData, this);
	}

}

void DServerApp::OnExit()
{
}

bool DServerApp::OnConfig(Json::Value& root)
{
	log.Write(DLogger::DEBUG, "DServerApp::OnConfig()");
	
	if (!DApplication::OnConfig(root))
		return false;
	
	if (!GetConfigInt(root, "server.port", port))
		return false;

	GetConfigInt(root, "server.maxClient", maxClient);
	GetConfigInt(root, "server.timeout", timeout);
		
	return true;
}
