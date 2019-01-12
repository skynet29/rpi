#ifndef BITENODE_H
#define BITENODE_H

#include <string>
#include <vector>

#include "BiteMessage.h"

using namespace std;

class DSocket;

class BiteNode
{
public:
	enum Status {OBSOLETE, OK, DEBUG, INFO, WARNING, ERROR, CRITICAL};
	
	BiteNode(const string& nodeName);
	
	string GetName() {return nodeName;}
	void SetStatus(Status status);
	BiteMessage* AddMessage(const char* identifier);
	
	bool Send(DSocket& sock);	
	void ClearAllMessage();
private:
	string ToJson();
	
	string nodeName;
	Status status;
	vector<BiteMessage*> msgs;
};

#endif
