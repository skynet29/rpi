#include "DLogger.h"
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>
#include <json/reader.h>

//static vector<DLogger*> loggers;
static DLogger* loggers = NULL;

static const string levelNames[] = {"DEBUG", "INFO", "WARNING", "ERROR"};

static int GetLevel(const string& name)
{
	for(unsigned i = 0; i < 4; i++)
	{
		if (levelNames[i] == name)
			return i;
	}
	return -1;
}

DLogger::DLogger(const string& name)
{
	this->name = name;
	this->minLevel = DEBUG;
	
	//loggers.push_back(this);
	next = loggers;
	loggers = this;
	printf("Add Logger %s\n", name.c_str());
}
	
void DLogger::SetLevel(Level minLevel)
{
	this->minLevel = minLevel;
}

void DLogger::Write(Level level, const char* fmt, ...)
{
	if (level >= minLevel)
	{
		fprintf(stderr, "%s:%s:%s: ", GetDate().c_str(), name.c_str(), levelNames[level].c_str());
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
		fprintf(stderr, "\n");
	}
}

DLogger* DLogger::GetLogger(const string& name)
{
	for(DLogger* logger = loggers; logger != NULL; logger = logger->next)
	{
		if (logger->name == name)
			return logger;
	}
	return NULL;
}

void DLogger::Init(Json::Value root)
{
	Json::Value defaultLog = root["default"];
	if (defaultLog.isString())
	{
		int level = GetLevel(defaultLog.asString());
		if (level != -1)
		{
			for(DLogger* logger = loggers; logger != NULL; logger = logger->next)
			{
				logger->SetLevel((Level)level);
			}
			//for(unsigned i = 0; i < loggers.size(); i++)
				//loggers[i]->SetLevel((Level)level);
		}
	}
	
	Json::Value::Members keys = root.getMemberNames();
	for(unsigned i = 0; i < keys.size(); i++)
	{
		DLogger* logger = GetLogger(keys[i]);
		if (logger == NULL)
			continue;
			
		Json::Value node = root[logger->GetName()];
		if (!node.isString())
			continue;
			
		int level = GetLevel(node.asString());
		if (level != -1)
			logger->SetLevel((Level)level);
	}
	
	
}


bool DLogger::Init(const char* fileName)
{
	ifstream in;
	in.open(fileName, ifstream::in);
	if (!in.good())
	{
		cout << "Failed to open file " << fileName << endl;
		return false;
	}	

	Json::Reader reader;
	Json::Value root;
	
	if (!reader.parse(in, root))
	{
		cout << "Failed  to parse config file: " << reader.getFormatedErrorMessages() << endl;
		return false;
	}	
	
	DLogger::Init(root);	
	return true;
}


string DLogger::GetDate()
{
	time_t now;
	time(&now);
	
	struct tm* ptm;
	ptm = localtime(&now);
	
	char buff[64];	
	strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", ptm);
	
	return buff;
}
