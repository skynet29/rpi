#ifndef BITEMESSAGE_H
#define BITEMESSAGE_H

#include <json/value.h>
#include <string>

using namespace std;

class BiteMessage
{
public:
	BiteMessage(const char* identifier);
	
	BiteMessage* AddParam(const char* val);
	BiteMessage* AddParam(int val);
	BiteMessage* AddParam(double val);
	Json::Value ToJson();
private:
	Json::Value args;
	string identifier;
	int nbArgs;
};

#endif
