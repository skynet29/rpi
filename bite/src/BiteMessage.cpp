#include "BiteMessage.h"
#include <stdio.h>

BiteMessage::BiteMessage(const char* identifier) : args(Json::objectValue)
{
	this->identifier = identifier;
	nbArgs = 0;
}

BiteMessage* BiteMessage::AddParam(int val)
{
	char buff[8];
	sprintf(buff, "%d", nbArgs++);
	args[buff] = val;
	return this;
}

BiteMessage* BiteMessage::AddParam(const char* val)
{
	char buff[8];
	sprintf(buff, "%d", nbArgs++);
	args[buff] = val;
	return this;
}

BiteMessage* BiteMessage::AddParam(double val)
{
	char buff[8];
	sprintf(buff, "%d", nbArgs++);
	args[buff] = val;
	return this;
}

Json::Value BiteMessage::ToJson()
{
	Json::Value message;
	message[identifier] = args;
	return message;
}
