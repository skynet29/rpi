#include "NovaMotor.h"

#include "wiringSerial.h"

#include "DThread.h"
#include "DTimer.h"
#include "DLogger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


#define INIT_STRING "Motorloop started"
#define MIN_POWER 	2
#define TIMEOUT_ECHO	1000 // 1 sec
#define KEEP_ALIVE_TIMEOUT	1000 // 1sec 

#define DUMPFLT(a) \
	printf("%s=%f\n", #a, a);

#define DUMPDATE(a) \
	printf("%s=%s\n", #a, GetStrDate(a));
	
static DLogger log("NovaMotor");
	
static char* GetStrDate(time_t& date)
{
	struct tm* ptm;
	ptm = localtime(&date);
	
	static char buff[64];
	
	strftime(buff, sizeof(buff), "%Y-%m-%dT%H:%M:%S", ptm);
	return buff;
}
	
	
static bool DecodeValue(const char* fields[], int fieldsLen, const char* name, float& var)
{
	for(int i = 0; i < fieldsLen; i+=2)
	{
		if (strcmp(fields[i], name) == 0)
		{
			var = atof(fields[i+1]);
			return true;
		}
	}
	return false;
}	

static void AddChar(char* buff, const char* c, int nb)
{
	for(int i = 0; i < nb; i++)
		strcat(buff, c);
}

NovaMotor::MonitorData::MonitorData()
{
	rpm = 0.0;
	voltage = 0.0;
	current = 0.0;
	tempCont = 0.0;
	tempMot = 0.0;	
}

	
void NovaMotor::MonitorData::Dump()
{
	DUMPFLT(rpm);
	DUMPFLT(voltage);
	DUMPFLT(current);
	DUMPFLT(tempCont);
	DUMPFLT(tempMot);
	DUMPDATE(date);
	
}


NovaMotor::NovaMotor()
{
	fd = -1;
	maxPower = 10;
	timeoutEcho = TIMEOUT_ECHO;

	// modif_SH : init ref monitoring volumes
	p_MonitoringVolume = MonitoringVolume::instance();
	
	Reset();
}


void NovaMotor::Reset()
{
	isBooted = false;
	isInit = false;
	echoMsg = NULL;
	curPower = 0;	
}

NovaMotor::~NovaMotor()
{
	if (fd >= 0)
		serialClose(fd);
}

void NovaMotor::SetMaxPower(int maxPower)
{
	this->maxPower = maxPower;
}

void NovaMotor::SetTimeoutEcho(int timeoutEcho)
{
	this->timeoutEcho = timeoutEcho;
}
	
void NovaMotor::GetMonitorData(MonitorData& theData)
{
	memcpy(&theData, &data, sizeof(data));
	theData.power = curPower;
}
	
bool NovaMotor::Init(const char* deviceName)
{
	fd = serialOpen(deviceName, 115200);

	if (fd < 0)
	{
		log.Write(DLogger::ERROR, "serialOpen():%s", strerror(errno));
		//perror("serialOpen()");
		return false;
	}	
	
	Start(); // start reception thread

	return true;
}

bool NovaMotor::Init()
{
	isInit = true;
	return true;
}

void NovaMotor::SendConfig()
{
	SendMsg("s");	
	SendMsg("a");	//enter setup mode
	SendMsg("rp029"); // frequence du moteur en millier de periodes par minutes	
	SendMsg("pp021"); // nombre paires de poles ici 21
					  // donc vitesse max 29000/21 = 1381 t/mn
	SendMsg("cl050");
	SendMsg("e");	// exit setup mode
	log.Write(DLogger::INFO, "Init finished");
	isInit = true;	
	aliveTimer.Start(KEEP_ALIVE_TIMEOUT);
}


void NovaMotor::OnRun()
{
	char buff[1024];

	uint len = 0;

	while(1)
	{	
		char c = (char)serialGetchar(fd);
		
		if (c >= 0)			
		{
			if  (c != 10 && c != 13)
			{
				if (len < sizeof(buff))
					buff[len++] = c;
				else
				{
					log.Write(DLogger::WARNING, "buffer overflow");
					len = 0;
				}
			}

			if (echoMsg != NULL)
			{
				buff[len] = 0;
				//printf("Test echo %s\n", buff);
				if (strcmp(buff, echoMsg) == 0)
				{
					free(echoMsg);
					len = 0;
					echoMsg = NULL;
				}
			}
			else if (c == 10)
			{
				buff[len] = 0;
				OnRecvData(buff);
				len = 0;
			}
       	}
	}
}

void NovaMotor::OnRecvData(char* msg)
{
	if (strstr(msg, INIT_STRING) != NULL)
	{
		log.Write(DLogger::INFO, "Found init string");
		isBooted = true;
	}

	else if (isInit)
	{
		log.Write(DLogger::DEBUG, "Msg:%s\n", msg);
		DecodeMonitorMsg(msg);	
	}
}

void NovaMotor::DecodeMonitorMsg(char* msg)
{
	const char* fields[16];

	char* p = strtok(msg, ",=");
	int i = 0;
	while (p && i < 16)
	{
		/*
		printf("token=%s?\n", p);
		if ((i % 2) == 1)
			printf("val=%f\n", atof(p));
		*/

		fields[i++] = p;
		p = strtok(NULL, ",=");
	}
	//printf("i=%d\n", i);
	bool decodingOk = true;
	decodingOk &= DecodeValue(fields, i, "U", data.voltage);
	decodingOk &= DecodeValue(fields, i, "I", data.current);
	decodingOk &= DecodeValue(fields, i, "RPM", data.rpm);
	decodingOk &= DecodeValue(fields, i, "con", data.tempCont);
	decodingOk &= DecodeValue(fields, i, "mot", data.tempMot);
	if (decodingOk)
	{
		aliveTimer.Start(KEEP_ALIVE_TIMEOUT);
		
		// modif_SH : maj des volumes
		p_MonitoringVolume->onRPMReceived(data.rpm);	
	}
	else
		log.Write(DLogger::WARNING, "error while decoding monitoring message");
	time(&data.date);
	
}

bool NovaMotor::IsAlive()
{
	return (isInit && !aliveTimer.IsExpired());
}

bool NovaMotor::SendMsg(const char* msg)
{
	log.Write(DLogger::INFO, "send:%s\n", msg);
	serialPuts(fd, msg);
	if (timeoutEcho == 0) // don't wait echo
		return true;
	
	echoMsg = strdup(msg);
	DTimer timer(TIMEOUT_ECHO); 
	while (echoMsg != NULL)
	{
		if (timer.IsExpired())
		{
			log.Write(DLogger::WARNING, "Echo timeout\n");
			char* temp = echoMsg;
			echoMsg = NULL;
			free(temp);
			return false;
		}
		Sleep(10);
	}
	return true;
}

bool NovaMotor::StartRev()
{
	if (isInit && curPower == 0)
	{
		SendMsg("1--------r");
		curPower = MIN_POWER;
		return true;
	}
	return false;
}

bool NovaMotor::StartFwd()
{
	if (isInit && curPower == 0)
	{
		SendMsg("1--------f");
		curPower = MIN_POWER;
		
		//modif_SH : start monitoring du volume
		p_MonitoringVolume->Start();
		return true;
	}	
	return false;
}

bool NovaMotor::Stop()
{
	SendMsg("0");
	curPower = 0;
	
	//modif_SH : stop monitoring du volume
	p_MonitoringVolume->Stop();
	return true;
}

bool  NovaMotor::IncPower()
{
	if (curPower != 0 && curPower < maxPower)
	{
		SendMsg("+");
		curPower++;
		return true;
	}
	return false;

}

bool  NovaMotor::DecPower()
{
	if (curPower > MIN_POWER)
	{
		SendMsg("-");
		curPower--;
		return true;
	}
	return false;
}


bool NovaMotor::SetPower(int power)
{
	if (curPower == 0)
	{
		log.Write(DLogger::ERROR, "Motor not started\n");
		return false;
	}
	
	if (power < MIN_POWER || power > maxPower)
	{
		log.Write(DLogger::ERROR, "Power out of range\n");
		return false;
	}
	
	if (power == curPower)
		return true; // nothing to do
	
	char buff[128];
	
	if (power < 10)
	{
		strcpy(buff, "1");
		AddChar(buff, "-", 10 - power);
	}
	else
	{
		sprintf(buff, "%d", power / 10);
		AddChar(buff, "+", power % 10);
	}
	
	curPower = power;
	//printf("buff=%s\n", buff);
	SendMsg(buff);
	
	return true;
}
