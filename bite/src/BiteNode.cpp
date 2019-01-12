#include "BiteNode.h"

#include <json/value.h>
#include <json/writer.h>
#include <string.h>

#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include "DSocket.h"

#include "DLogger.h"

static DLogger logger("BiteNode");

static string GetHostName()
{
	char* hostName = getenv("HOSTNAME_EXTERNAL");
	if (hostName != NULL)
	{
		logger.Write(DLogger::DEBUG, "hostName=%s", hostName);
		return hostName;
	}	
	
	char buff[32];
	gethostname(buff, sizeof(buff));
	char* p = strchr(buff, '.');
	if (p)
		*p = 0;
		
	return buff;
}

BiteNode::BiteNode(const string& nodeName)
{	
	this->nodeName = nodeName;
	this->status = OK;
}


void BiteNode::SetStatus(Status status)
{
	this->status = status;
}

BiteMessage* BiteNode::AddMessage(const char* identifier)
{
	BiteMessage* ret = new BiteMessage(identifier);
	msgs.push_back(ret);
	return ret;
}

void BiteNode::ClearAllMessage()
{
	for(unsigned i = 0; i < msgs.size(); i++)
		delete msgs[i];
		
	msgs.clear();
}

string BiteNode::ToJson()
{
	
	Json::Value node;
	node["type"] = "bite";
	node["node"] = GetHostName() + "." + nodeName;
	node["status"] = status;
	
	time_t now;
	time(&now);
		
	node["date"] = (Json::UInt)now;
	 
	 
	Json::Value messages(Json::arrayValue);
	for(unsigned i = 0; i < msgs.size(); i++)
		messages.append(msgs[i]->ToJson());
	
	node["messages"] = messages;

	
	Json::Value docs;
	docs.append(node);

	Json::Value root;
	root["docs"] = docs;
	
	Json::FastWriter writer;
	string msg = writer.write(root);
	//cout << msg << endl;	
	
	size_t idx = msg.find(",");
	//cout << "idx=" <<  idx << endl;
	return  msg.substr(0, idx) + "000" + msg.substr(idx);
	
}



bool BiteNode::Send(DSocket& sock)
{
	string msg = ToJson();
	//cout << msg << endl;
	logger.Write(DLogger::DEBUG, "msg=%s", msg.c_str());
	
	stringstream s;
	s << "POST /bites_info/_bulk_docs HTTP/1.1" << endl;	
	s << "Content-Length: " << msg.length() << endl;
    s << "Content-Type: application/json" << endl;
    s << "Accept: application/json" << endl << endl;
    s << msg;

	string request = s.str();
	
	//cout << request << endl;	
	
	if (!sock.Send(request.c_str()))
	{
		logger.Write(DLogger::ERROR, "bite send failed");
		sock.Close();
	}

	/*
	
	char buff[2048];
	int len = sock.Recv(buff, sizeof(buff));
	if (len > 0)
	{
		buff[len] = 0;
		//cout << "buff=" << buff << endl;
	}
	*/
	
	
	return true;
}
