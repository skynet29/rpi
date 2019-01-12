#ifndef DLOGGER_H
#define DLOGGER_H

#include <string>
#include <json/value.h>

using namespace std;


class DLogger
{
public:
	enum Level {DEBUG, INFO, WARNING, ERROR};
	
	DLogger(const string& name);
	
	void SetLevel(Level minLevel);
	void Write(Level level, const char* fmt, ...);
	string GetName() {return name;}
	
	static DLogger* GetLogger(const string& name);
	static void Init(Json::Value node);
	static bool Init(const char* fileName);
	
	static string GetDate();
private:
	string name;
	Level minLevel;	
	DLogger* next;
};

#endif
