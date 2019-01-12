#include "ValveController.h"

#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "DLogger.h"



//enum {WIV = 1, WEV = 2, RTFV = 4, RTDV = 8, FTFV = 16, FTDV = 32};


static DLogger log("ValveController");

struct Info
{
	const char* name;
	int pin;
	int openLevel;
};


static Info infos[] = {
	{"WIV", 0, LOW},
	{"WEV", 1, LOW},
	{"RTFV", 2, LOW},
	{"RTDV", 3, LOW},
	{"FTFV", 4, LOW},
	{"FTDV", 5, LOW},
	{"THSV", 6, LOW},
	{"NOVA_PRECHARGE", 17, HIGH},
	{"NOVA_MAIN", 18, HIGH}	
};

#define GPIO_COUNT 9
#define VALVE_COUNT 7
#define OPEN_LEVEL(i) (infos[i].openLevel)
#define CLOSE_LEVEL(i)(infos[i].openLevel == LOW ? HIGH : LOW)



int ValveController::GetValveId(const char* name)
{
	for(unsigned i = 0; i < GPIO_COUNT; i++)
	{
		if (strcmp(name, infos[i].name) == 0)
			return 1 << i;
	}
	return 0;
}


bool ValveController::Init()
{
	if (wiringPiSetup() < 0)
	{
		log.Write(DLogger::ERROR, "wiringPiSetup():%s", strerror(errno));
		return false;
	}
	
	// set all GPIO to OUTPUT mode
	for(unsigned i = 0; i < GPIO_COUNT; i++)
		pinMode(infos[i].pin, OUTPUT);
		
	//Close all
	Close(ALL);
	
	return true;
}

void ValveController::Open(int valve)
{
	for(unsigned i = 0; i < GPIO_COUNT; i++)
	{
		if ((valve & (1 << i)) != 0)
			digitalWrite(infos[i].pin, OPEN_LEVEL(i));
	}
		
}

void ValveController::SetState(int valve)
{
	for(unsigned i = 0; i < VALVE_COUNT; i++)
	{
		if ((valve & (1 << i)) != 0)
			digitalWrite(infos[i].pin, OPEN_LEVEL(i));
		else
			digitalWrite(infos[i].pin, CLOSE_LEVEL(i));
	}
		
}

void ValveController::InvertState(int valve)
{
	
	for(unsigned i = 0; i < VALVE_COUNT; i++)
	{
		if ((valve & (1 << i)) != 0)
		{
			int ret = digitalRead(infos[i].pin);
			digitalWrite(infos[i].pin, (ret == LOW) ? HIGH : LOW);
		}
	}
		
}

void ValveController::Close(int valve)
{
	for(unsigned i = 0; i < GPIO_COUNT; i++)
	{
		if ((valve & (1 << i)) != 0)
			digitalWrite(infos[i].pin, CLOSE_LEVEL(i));
	}
		
}

unsigned ValveController::GetState()
{
	unsigned state = 0;
	
	for(unsigned i = 0; i < GPIO_COUNT; i++)
	{
		int ret = digitalRead(infos[i].pin);
		
		//printf("ret[%d]=%d\n", i, ret);
		if (ret == OPEN_LEVEL(i))
			state |= (1 << i);
	}
	
	//printf("state=%02X\n", state);
	
	return state;
		
}

const char* ValveController::GetValveStr(unsigned idx)
{
	if (idx < GPIO_COUNT)
		return infos[idx].name;
	
	return NULL;
}

void ValveController::DumpState()
{
	int state = GetState();
	
	for(unsigned i = 0; i < GPIO_COUNT; i++)
	{
		bool isOpen = ((state & (1 << i)) != 0);		
		log.Write(DLogger::INFO, "RELAY[%s]:%s", infos[i].name, (isOpen) ? "OPEN" : "CLOSE");			
	}
}

int ValveController::GetValveIdx(unsigned valveId)
{
	int idx = -1;
	while (valveId != 0)
	{
		idx++;
		valveId >>= 1;
	}
	return idx;
}

bool ValveController::IsOpen(unsigned valveId)
{
	int idx = GetValveIdx(valveId);
	
	int ret = digitalRead(infos[idx].pin);
	
	return (ret == OPEN_LEVEL(idx));
}

