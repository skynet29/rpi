
#include "BiteNode.h"
#include "BiteMessage.h"
#include "DClient.h"
#include <iostream>
#include "DThread.h"

using namespace std;


int main(int argc, char* argv[])
{
		
	BiteNode biteNode("balancing");
	
	biteNode.SetStatus(BiteNode::ERROR);
	biteNode.AddMessage("balancing.conx_eng");
	BiteMessage* msg = biteNode.AddMessage("balancing.temp_eng");
	msg->AddParam(10);
	
	DClient sock;
	sock.SetServerInfo("192.168.1.100", 5984);
	
	if (!sock.Connect())
	{
		cout << "Connect failed" << endl;
		return -1;
	}
	
	while (1)
	{
		cout << "sendBite\n";
		biteNode.Send(sock);
		DThread::Sleep(10000);
	}
		
	sock.Close();	

	
	return 0;
}
