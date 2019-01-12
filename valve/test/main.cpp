#include "ValveController.h"


int main(int argc, char* argv[])
{
	ValveController valve;
	
	if (!valve.Init())
		return -1;

	valve.DumpState();
	
	valve.Open(ValveController::WIV | ValveController::FTFV);
	valve.DumpState();
	
	valve.Open(ValveController::ALL);
	valve.DumpState();
	
	valve.Close(ValveController::RTFV | ValveController::RTDV);
	valve.DumpState();
	
	valve.InvertState(ValveController::WIV | ValveController::RTFV);
	valve.DumpState();

	return 0;
}
