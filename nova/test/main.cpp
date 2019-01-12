#include "NovaMotor.h"

#include <stdio.h>
#include <stdlib.h>
#include "DThread.h"

void DisplayMenu()
{
	printf("Menu\n");
	printf("\t 1 - Start Reverse\n");
	printf("\t 2 - Start Foward\n");
	printf("\t 3 - Stop\n");
	printf("\t 4 - Inc Power\n");
	printf("\t 5 - Dec Power\n");
	printf("\t 6 - Print Current Power\n");
	printf("\t 7 - Print Monitor Data\n");
	printf("Choix: ");
}

static bool WaitNovaBoot(void* arg)
{
	return ((NovaMotor*)arg)->IsBooted();
}

int main(int argc, char* argv[])
{
	NovaMotor nova;
		
	if (!nova.Init("/dev/ttyAMA0"))
	{
		printf("Init failed\n");
		return -1;
	}
	
	if (!DThread::WaitCond(WaitNovaBoot, &nova, 60000))
	{
		printf("Boot timeout\n");
		return -1;
	}
		
	nova.SendConfig();
		
	while (1)
	{
		
		char buff[1204];
		
		DisplayMenu();

		gets(buff);
		switch (atoi(buff))
		{
			case 1:
				nova.StartRev();
				break;

			case 2:
				nova.StartFwd();
				break;

			case 3:
				nova.Stop();
				break;
				
			case 4:
				nova.IncPower();
				break;

			case 5:
				nova.DecPower();
				break;
				
			case 6:
				printf("Current Power=%d\n", nova.GetCurPower());
				break;

			case 7:
			{
				NovaMotor::MonitorData data;
				nova.GetMonitorData(data);
				data.Dump();
			}
				break;
				
			default:
				printf("Bad command\n");
				
		}

		//nova.SendMsg(buff);
	}


	return 0;
}

