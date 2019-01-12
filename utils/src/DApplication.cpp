#include "DApplication.h"

#include <signal.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <json/reader.h>
#include "DLogger.h"
#include <string.h>
#include <stdlib.h>

using namespace std;

static DApplication* app = NULL;

static DLogger log("DApplication");

static void SignalHandler(int sig)
{
	printf("MainHandler sig=%d\n", sig);	
	
	if (app)
		app->Exit();
}


Json::Value DApplication::GetPathNode(Json::Value& root, const char* path)
{
	vector<string> tokens;
	Tokenize(path, ".", tokens);
	
	Json::Value node = root;
	for(unsigned i = 0; i < tokens.size()-1; i++)
	{
		if (!node[tokens[i]].isObject())
			return Json::Value();
		node = node[tokens[i]];
	}
	
	return node[tokens[tokens.size()-1]];
}

int DApplication::Tokenize(const char* s, const char* token, vector<string>& v)
{
	char* clone = strdup(s);
	char* p = strtok(clone, token);
	while (p)
	{
		v.push_back(p);
		p = strtok(NULL, token);
	}
	free(clone);	
	return v.size();
}

DApplication::DApplication()
{
	app = this;
	
	isRunning = true;
}

int DApplication::Run()
{
	signal(SIGINT, SignalHandler);
	signal(SIGQUIT, SignalHandler);
	signal(SIGTERM, SignalHandler);	
		
	if (!OnStartup())
	{
		log.Write(DLogger::ERROR, "OnStartup() failed");
		return -1;
	}
		
	while (isRunning)
		OnIterate();
		
	OnExit();
		
	return 0;
}

void DApplication::Exit()
{
	isRunning = false;
}

bool DApplication::ParseArg(int argc, char* argv[])
{
	if (argc < 2) // no config file
		return false;
		
	ifstream in;
	in.open(argv[1], ifstream::in);
	if (!in.good())
	{
		cout << "Failed to open file " << argv[1] << endl;
		return false;
	}	
	
	Json::Reader reader;
	Json::Value root;
	
	if (!reader.parse(in, root))
	{
		cout << "Failed  to parse config file: " << reader.getFormatedErrorMessages() << endl;
		return false;
	}
	
	if (!OnConfig(root))
	{
		log.Write(DLogger::ERROR, "OnConfig() failed");
		return false;
	}
	
	return true;
}


bool DApplication::OnConfig(Json::Value& root)
{
	Json::Value logger = root["logger"];
	if (logger.isObject())
		DLogger::Init(logger);
		
	return true;
}

bool DApplication::GetConfigInt(Json::Value& root, const char* path, int& val)
{
	Json::Value node = GetPathNode(root, path);
	if (!node.isInt())
	{
		log.Write(DLogger::WARNING, "path not found or bad type: %s", path);
		return false;
	}		
	val = node.asInt();
	return true;
}

bool DApplication::GetConfigBool(Json::Value& root, const char* path, bool& val)
{
	Json::Value node = GetPathNode(root, path);
	if (!node.isBool())
	{
		log.Write(DLogger::WARNING, "path not found or bad type: %s", path);
		return false;
	}		
	val = node.asBool();
	return true;
}

bool DApplication::GetConfigString(Json::Value& root, const char* path, string& val)
{
	Json::Value node = GetPathNode(root, path);
	if (!node.isString())
	{
		log.Write(DLogger::WARNING, "path not found or bad type: %s", path);
		return false;
	}		
	val = node.asString();
	return true;
}

bool DApplication::GetConfigDouble(Json::Value& root, const char* path, double& val)
{
	Json::Value node = GetPathNode(root, path);
	if (!node.isDouble())
	{
		if (!node.isInt()) // try with integer
		{
			log.Write(DLogger::WARNING, "path not found or bad type: %s", path);
			return false;
		}
		val = (double)node.asInt();
		return true;
	}		
	val = node.asDouble();
	return true;
}

