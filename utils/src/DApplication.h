#ifndef DAPPLICATION_H
#define DAPPLICATION_H

#include <json/value.h>
#include <string>
#include <vector>

using namespace std;

class DApplication
{
public:
	DApplication();
	
	int Run();
	bool ParseArg(int argc, char* argv[]);
	void Exit();
	
protected:
	virtual bool OnStartup() {return true;}	
	virtual void OnIterate() {}
	virtual void OnExit() {}
	virtual bool OnConfig(Json::Value& root);
	
	bool GetConfigInt(Json::Value& root, const char* path, int& val);
	bool GetConfigBool(Json::Value& root, const char* path, bool& val);
	bool GetConfigString(Json::Value& root, const char* path, string& val);
	bool GetConfigDouble(Json::Value& root, const char* path, double& val);
	
	static int Tokenize(const char* s, const char* token, vector<string>& v);
	static Json::Value GetPathNode(Json::Value& root, const char* path);
private:
	bool isRunning;
};

#endif
