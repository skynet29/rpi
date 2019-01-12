#include "RearAgent.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <json/reader.h>
#include <json/writer.h>

#include "DSequencer.h"
#include "DLogger.h"

#include "StartMotorAction.h"
#include "ValveAction.h"
#include "ValveStateAction.h"

#include "ValveFillAlternateAction.h"
#include "ValveEmptyAlternateAction.h"
#include "NovaVoltageSensor.h"
#include "NovaCurrentSensor.h"
#include "NovaTemperatureSensor.h"
#include "NovaKeepAliveSensor.h"
#include "ConsPowerSensor.h"
#include "IncMotorPowerAction.h"
#include "StopMotorAction.h"



static DLogger logger("RearAgent");

static int GetValveState(Json::Value& root)
{
	int state = 0;
	Json::Value valves = root["valves"];
	if (valves.isArray())
	{
		for(uint i = 0; i < valves.size(); i++)
			state |= ValveController::GetValveId(valves[i].asString().c_str());
		
	}	
	return state;
}


enum {MOTOR_STARTFWD, MOTOR_STARTREV, MOTOR_STOP, MOTOR_INCPOWER, MOTOR_DECPOWER, MOTOR_SETPOWER, VALVE_OPEN, VALVE_CLOSE,
	BUOY_FILL, BUOY_EMPTY, BUOY_TRANSFER, BUOY_STOP, STOP, MOTOR_POWEROFF, MOTOR_POWERUP, CMD_COUNT};

static const char* cmds[] = {"motor_startfwd", "motor_startrev", "motor_stop", "motor_incpower", "motor_decpower", "motor_setpower", "valve_open", "valve_close",
	"buoy_fill", "buoy_empty", "buoy_transfer", "empty_all", "stop", "motor_poweroff", "motor_powerup"};


enum {
	ACTION_NONE = -1, 
	ACTION_FILL_FRONT, ACTION_FILL_REAR,
	ACTION_EMPTY_FRONT, ACTION_EMPTY_REAR,
	ACTION_TRANSFER_R2F, ACTION_TRANSFER_F2R,
	ACTION_SURFACE,
	ACTION_FILL_BOTH, ACTION_EMPTY_BOTH
};

static int GetCmdId(const char* cmd)
{
	for(int i = 0; i < CMD_COUNT; i++)
	{
		if (strcmp(cmds[i], cmd) == 0)
			return i;
	}
	return -1;
}


int main(int argc, char* argv[])
{
	RearAgent agent;	
	
	if (!agent.ParseArg(argc, argv))
		return -1;	
		
	return agent.Run();
}
	
RearAgent::RearAgent()
{
	curSeq = NULL;
	curAction = ACTION_NONE;
	safetyActivated = false;
	
	frontTemp = 0.0;
	frontWl = 0.0;	
	ftPressure = 0.0;
	
	rearTemp = 0.0;
	rearWl = 0.0;
	rtPressure = 0.0;
	pumpPressure = 0.0;
	pumpTemp = 0.0;
	navCurrent = 0.0;
	batteryTemp = 0.0;
	
	// default value for config parameters
	isSimu = false;
	maxTankPressure = 2.0; 	// 2 bars
	maxPumpPressure = 40.0; // 40 bars
	maxPumpTemp = 50.0;		// 50 °C
	monitorPeriod = 1000; 	// 1 sec
	stopMotorDelay = 10000;	// 10 sec
	minTankPressure = -1.0; // -1 bar
	openValveDelay = 500; // 500 ms
	motorPowerStepDelay = 500; // 500 ms

	
	// bite nodes
	thrusterNode = AddBiteNode("thruster");
	thrusterNode->AddSensor(new NovaKeepAliveSensor("novaCnx", nova));	
	thrusterNode->AddSensor(new NovaVoltageSensor("novaVoltage", nova));	
	thrusterNode->AddSensor(new NovaCurrentSensor("novaCurrent", nova));	
	thrusterNode->AddSensor(new NovaTemperatureSensor("novaMotTemp", nova));	
	
	AddBiteNode("pump");
	AddBiteNode("sensors")->AddSensor(new ConsPowerSensor("consPower", this));	
	
	// modif_SH : init monitoring volumes et ajout du controleur de vannes
	p_MonitoringVolume = MonitoringVolume::instance();
	p_MonitoringVolume->setValveController(&(this->valveCtrl));
		
}


void RearAgent::DecodeMsg(Json::Value& root)
{
	string cmd;
	lastMsg = "";
		
	if (!GetConfigString(root, "cmd", cmd))
	{
		logger.Write(DLogger::ERROR, "Cmd parse error");
		return;
	}
	
	int cmdId = GetCmdId(cmd.c_str());
	if (cmdId < 0)
	{
		logger.Write(DLogger::ERROR, "Unknown command");
		return;
	}
	
	switch(cmdId)
	{
	case MOTOR_STARTFWD:		
		nova.StartFwd();
		break;
		
	case MOTOR_STARTREV:
		nova.StartRev();
		break;
		
	case MOTOR_STOP:
		nova.Stop();
		break;
		
	case MOTOR_INCPOWER:
		nova.IncPower();
		break;
		
	case MOTOR_DECPOWER:
		nova.DecPower();
		break;
	
	case MOTOR_SETPOWER:
		{
			int power;
			if (GetConfigInt(root, "power", power))
				nova.SetPower(power);
		}
		break;
		
	case MOTOR_POWEROFF:
		{
			valveCtrl.Close(ValveController::NOVA_MAIN);
			valveCtrl.Close(ValveController::NOVA_PRECHARGE);
			nova.Reset();
		}
		break;
		
	case MOTOR_POWERUP:
		{
			valveCtrl.Open(ValveController::NOVA_PRECHARGE);
		}
		break;		
	
	case VALVE_OPEN:
		{	
			valveCtrl.Open(GetValveState(root));
			valveCtrl.DumpState();		
		}
		break;
		
	case VALVE_CLOSE:
		{	
			valveCtrl.Close(GetValveState(root));
			valveCtrl.DumpState();		
		}
		break;
		
	case BUOY_FILL:
	case BUOY_EMPTY:
	case BUOY_TRANSFER:
		if (!DecodeBuoyCmd(cmdId, root))
		{
			logger.Write(DLogger::ERROR, "command refused");
			lastMsg = "command refused";
		}
		break;
		
	case BUOY_STOP:
		EmergencyStop();
		break;
		
	case STOP:
		Stop();
		break;
		
	default:
		{
			logger.Write(DLogger::WARNING, "Command not implemented");
		}
	}
	
}

bool RearAgent::DecodeBuoyCmd(int cmd, Json::Value& root)
{
	
	if (curSeq != NULL)
	{
		logger.Write(DLogger::ERROR, "An action is already in progress");
		return false;
	}
	
	if (!nova.IsInit())
	{
		logger.Write(DLogger::ERROR, "Motor not initialized");
		return false;
	}
	
	StopCurrentAction();

	double power;
	int duration=0;
	double vol=0.0; // modif_SH : ajout parametre vol dans requete
	int period;
	
	if (!GetConfigDouble(root, "power", power))
		return false;

    // modif_SH : test si parametre duration ou volume dans requete
    bool bParams = GetConfigInt(root, "duration", duration);
    bParams = bParams || GetConfigDouble(root, "vol", vol);
    
    if (!bParams) {
		logger.Write(DLogger::ERROR, "DecodeBuoyCmd : duration and vol parameters not initialized in command : command aborted");
		return false;
	}
    if ((duration==0) && (vol==0.0)) {
		logger.Write(DLogger::ERROR, "DecodeBuoyCmd : duration and vol parameters set to 0 : command aborted");
		return false;
	}
	
	logger.Write(DLogger::DEBUG, "DecodeBuoyCmd  duration:%d vol=%f", duration,vol);
	
	int state = 0;
	DAbstractAction* action = NULL;
	
	if (cmd == BUOY_FILL || cmd == BUOY_EMPTY)
	{
		string valves;
		if (!GetConfigString(root, "valve", valves))
			return false;
		
		state |= ValveController::THSV;
		
		state |= (cmd == BUOY_FILL) ? ValveController::WIV : ValveController::WEV;

		if (valves == "rt")
		{
			state |= (cmd == BUOY_FILL) ? ValveController::RTFV : ValveController::RTDV;
			curAction = (cmd == BUOY_FILL) ? ACTION_FILL_REAR : ACTION_EMPTY_REAR;
		}	
		else if (valves == "ft")
		{
			state |= (cmd == BUOY_FILL) ? ValveController::FTFV : ValveController::FTDV;
			curAction = (cmd == BUOY_FILL) ? ACTION_FILL_FRONT : ACTION_EMPTY_FRONT;
		}	
		else if (valves == "both")
		{
			state |= (cmd == BUOY_FILL) ? ValveController::RTFV : ValveController::RTDV;
			state |= (cmd == BUOY_FILL) ? ValveController::FTFV : ValveController::FTDV;
			curAction = (cmd == BUOY_FILL) ? ACTION_FILL_BOTH : ACTION_EMPTY_BOTH;
		}
		/*
		else if (valves == "alternate" && GetConfigInt(root, "period", period))
		{
			if (cmd == BUOY_FILL)
				action = new ValveFillAlternateAction(&valveCtrl, duration, period); 
			else		
				action = new ValveEmptyAlternateAction(&valveCtrl, duration, period); 
		}
		*/
		else
			return false;
		
	}
	else // TRANSFER
	{
		string dir;
		if (!GetConfigString(root, "dir", dir))
			return false;

		if (dir == "rt2ft")
		{
			state |= ValveController::RTDV | ValveController::FTFV;
			curAction = ACTION_TRANSFER_R2F;
		}	
		else if (dir == "ft2rt")
		{
			state |= ValveController::FTDV | ValveController::RTFV;
			curAction = ACTION_TRANSFER_F2R;
		}
		else
			return false;

	}
	

	
	curSeq = new DSequencer();

	// first, open valve
	curSeq->AddAction(new ValveStateAction(this, state));
	// wait
	curSeq->AddWaitAction(openValveDelay);
	
	/*
	if (cmd == BUOY_EMPTY)
	{
		// for the drain, open drain valve before starting pump motor
		curSeq->AddAction(new ValveStateAction(this, state & (ValveController::RTDV | ValveController::FTDV)));
	}
	*/
		
	//curSeq->AddAction(new StartMotorAction(&nova, power));
	//2eme, start motor with ramp up
	StartMotor(curSeq, power);
	
	
	// 3eme, wait end action (duration or pressure limit)
	if (action == NULL)
	{
		// modif_SH : add parameter vol at ValveAction
		curSeq->AddAction(new ValveAction(this, state, duration,vol));
	}
	else
		curSeq->AddAction(action);
		
	// 4eme, stop motor
	curSeq->AddAction(new StopMotorAction(&nova));	
	curSeq->Start();
	
	DTimer::GetTimeOfDay(&curActionStartTime);
	
	return true;
	
}





void RearAgent::OnRecvData(DSocket* client, const char* msg)
{
	if (client == (DSocket*)biteClient)
		return;

	logger.Write(DLogger::DEBUG, "Msg:%s", msg);
	
	Json::Value root;
	Json::Reader reader;

	if (!reader.parse(msg, root))		
	{
		logger.Write(DLogger::ERROR, "JSON Parse error: Msg:%s", msg);
		return;			
	}
	
	if (client == (DSocket*)frontClient)
	{
		DecodeFrontAgentMsg(root);
	}
	else if (client == (DSocket*)navClient)
	{
		DecodeNavAgentMsg(root);
	}		
	else
	{
		logger.Write(DLogger::INFO, "Msg:%s", msg);

		DecodeMsg(root);
	}
}

bool RearAgent::OnStartup()
{
	if (!BiteServerApp::OnStartup())
		return false;
		
	if (!valveCtrl.Init())
		return false;
		
					
	if (isSimu)
		nova.Init();
	else if (!nova.Init("/dev/ttyAMA0"))
		return false;
	else
		valveCtrl.Open(ValveController::NOVA_PRECHARGE);
	
	
	monitoringTimer.Start(monitorPeriod);
	
	
	return true;
}


void RearAgent::StopCurrentAction()
{
	if (curSeq != NULL)
	{
		logger.Write(DLogger::INFO, "Action aborted");
		lastMsg = "Action aborted";
		delete curSeq;
		curSeq = NULL;
		curAction = ACTION_NONE;
	}
}

void RearAgent::Stop()
{
	valveCtrl.SetState(0); // close all valves
	nova.Stop();
	StopCurrentAction();
	//stopMotorTimer.Start(stopMotorDelay);

}

void RearAgent::EmergencyStop()
{
	logger.Write(DLogger::INFO, "EmergencyStop");
	//StopCurrentAction();
	Stop();

	int state;
	
	curSeq = new DSequencer();

	// first empty front
	state = ValveController::THSV | ValveController::WEV | ValveController::FTDV;
	
	curSeq->AddAction(new ValveStateAction(this, state));
	curSeq->AddWaitAction(openValveDelay);
	StartMotor(curSeq, emergencyStopNovaPower);
	curSeq->AddAction(new ValveAction(this, state, emergencyStopDrainDuration));
	curSeq->AddAction(new StopMotorAction(&nova));	
	
	// then empty rear

	state = ValveController::THSV | ValveController::WEV | ValveController::RTDV;
	
	curSeq->AddAction(new ValveStateAction(this, state));
	curSeq->AddWaitAction(openValveDelay);
	StartMotor(curSeq, emergencyStopNovaPower);
	curSeq->AddAction(new ValveAction(this, state, emergencyStopDrainDuration));
	curSeq->AddAction(new StopMotorAction(&nova));	
	
	
	curSeq->Start();
	curAction = ACTION_SURFACE;
	DTimer::GetTimeOfDay(&curActionStartTime);
}

void RearAgent::OnIterate()
{
	BiteServerApp::OnIterate();
	
	ReadSensors();
	
	if (pumpPressure > maxPumpPressure && nova.GetCurPower() != 0)
	{
		logger.Write(DLogger::ERROR, "Pump pressure too high: %f", pumpPressure);
		lastMsg = "Pump pressure too high";
		StopCurrentAction();
		nova.Stop();
		valveCtrl.SetState(0); // close all valves
	}

	/*
	if (pumpTemp > maxPumpTemp  && nova.GetCurPower() != 0)
	{
		logger.Write(DLogger::ERROR, "Pump temperature too high: %f", pumpTemp);
		StopCurrentAction();
		nova.Stop();
		valveCtrl.SetState(0); // close all valves
	}
	*/
	
		
	if (!nova.IsInit())
	{
		if (nova.IsBooted())
		{
			valveCtrl.Open(ValveController::NOVA_MAIN);
			nova.SendConfig();
			valveCtrl.Close(ValveController::NOVA_PRECHARGE);
		}
	}
	
	if (curSeq != NULL)
	{
		//lastMsg = "Action in progress";
		DSequencer::State state = curSeq->Iterate();
		if (state != DSequencer::IN_PROG)
		{
			if (state == DSequencer::ERROR)
			{
				logger.Write(DLogger::INFO, "Action completed on error");
				//lastMsg = "Action completed on error";
			}
			else
			{
				logger.Write(DLogger::INFO, "Action completed successfully");
				//lastMsg = "Action completed successfully";
			}
			delete curSeq;
			curSeq = NULL;
			curAction = ACTION_NONE;
			//stopMotorTimer.Start(stopMotorDelay);
		}
	}
	
	/*
	if (curSeq == NULL && nova.GetCurPower() != 0 && stopMotorTimer.IsExpired())
	{
		// stop motor 30 sec after last action
		logger.Write(DLogger::INFO, "Stop motor after timeout\n");
		nova.Stop();
		stopMotorTimer.Stop();
	}
	*/
	if (server.GetClientCount() == 2 && !safetyActivated)
	{
		safetyActivated = true; // reactivate safety check when all agents are connected
		logger.Write(DLogger::INFO, "Safety reactivated");
	}
		
	if (safetyActivated && server.GetClientCount() < 2 && curAction != ACTION_SURFACE)
	{
		logger.Write(DLogger::WARNING, "No more client connected, emergency surface\n");
		//nova.Stop();
		EmergencyStop();
		safetyActivated = false;
	}
	
				
	if (monitoringTimer.IsExpired())
	{
		logger.Write(DLogger::DEBUG, "curAction=%d", curAction);			
		string msg = EncodeMonitorMsg();
		logger.Write(DLogger::DEBUG, "MonitorMsg=%s", msg.c_str());
			
		server.Broadcast(msg.c_str());
		monitoringTimer.Start(monitorPeriod);
	}
		
}

void RearAgent::ReadSensors()
{
	sensorMngr.GetSensorValue("temperature", rearTemp);
	sensorMngr.GetSensorValue("waterleakage", rearWl);
	sensorMngr.GetSensorValue("tankPressure", rtPressure);
	sensorMngr.GetSensorValue("pumpPressure", pumpPressure);
	sensorMngr.GetSensorValue("pumpTemp", pumpTemp);
}


string RearAgent::EncodeMonitorMsg()
{
	NovaMotor::MonitorData data;
	nova.GetMonitorData(data);
	
	Json::Value novaNode;
	novaNode["POW"] = data.power;
	novaNode["U"] = TRUNK(data.voltage, 2);
	novaNode["I"] = TRUNK(data.current, 2);
	novaNode["RPM"] = TRUNK(data.rpm, 2);
	novaNode["TCONT"] = TRUNK(data.tempCont, 2);
	novaNode["TMOT"] = TRUNK(data.tempMot, 2);
	novaNode["VALVES_ST"] = valveCtrl.GetState();
	novaNode["STATUS"] = nova.IsAlive();
	
	// modif_SH : add return values VOL,AR,AV
	novaNode["VOL"] = TRUNK(p_MonitoringVolume->getVol(), 2);
    novaNode["AR"] = TRUNK(p_MonitoringVolume->getAr(), 2);	
	novaNode["AV"] = TRUNK(p_MonitoringVolume->getAv(), 2);	
	
	Json::Value valves(Json::arrayValue);
	
	for(unsigned i = 0; i < 9; i++)
	{
		if (valveCtrl.IsOpen(1 << i))
			valves.append(valveCtrl.GetValveStr(i));
	}		
	
	Json::Value sensors;
	sensors["FT"] = frontTemp;
	sensors["FWL"] = frontWl;
	sensors["FTP"] = ftPressure;

	sensors["RPT"] = pumpTemp;
	sensors["RTP"] = rtPressure;
	sensors["RPP"] = pumpPressure;	
	sensors["RT"] = rearTemp;
	sensors["RWL"] = rearWl;
	
	
	time_t now;
	time(&now);

	Json::Value root;
	root["t"] = (Json::UInt)now;
	root["type"] = "bal";
	root["NOVA"] = novaNode;
	root["VALVES"] = valves;
	root["SENSORS"] = sensors;
	root["CMDSTATUS"] = lastMsg;
	root["CURACTION"] = curAction;
	
	if (curSeq != NULL)
	{
		struct timeval now;
		DTimer::GetTimeOfDay(&now);
		root["CURACTION_DURATION"] = DTimer::TimeSub(&now, &curActionStartTime) / 1000;				
	}
	else
		root["CURACTION_DURATION"] = 0;
	
	root["r_nrj"] = TRUNK(ComputeStateOfCharge(data.voltage, batteryTemp)*100, 2);
	
	
	Json::FastWriter writer;
	
	string msg = writer.write(root);	
	size_t idx = msg.rfind(",");
	return  msg.substr(0, idx) + "000" + msg.substr(idx); // force date in ms

}


void RearAgent::OnExit()
{
	DServerApp::OnExit();
	
	if (nova.GetCurPower() != 0)
	{
		printf("STOP MOTOR\n");
		nova.Stop();
	}
	
	valveCtrl.Close(ValveController::ALL); // close all valve,precharge and main
}


void RearAgent::DecodeNavAgentMsg(Json::Value& root)
{
	GetConfigDouble(root, "CURRENT", navCurrent);
	GetConfigDouble(root, "TEMP", batteryTemp);	
}

void RearAgent::DecodeFrontAgentMsg(Json::Value& root)
{
	GetConfigDouble(root, "TEMP", frontTemp);
	GetConfigDouble(root, "WL", frontWl);
	GetConfigDouble(root, "FTPS", ftPressure);
	if (isSimu)
	{
		GetConfigDouble(root, "RTPS", rtPressure);
		GetConfigDouble(root, "PPS", pumpPressure);
		GetConfigDouble(root, "PTS", pumpTemp);
	}
}

bool RearAgent::OnConfig(Json::Value& root)
{
	if (!BiteServerApp::OnConfig(root))
		return false;
		
	GetConfigBool(root, "isSimu", isSimu);	
	GetConfigInt(root, "monitorPeriod", monitorPeriod);			
	GetConfigInt(root, "stopMotorDelay", stopMotorDelay);
	GetConfigDouble(root, "maxTankPressure", maxTankPressure);
	GetConfigDouble(root, "minTankPressure", minTankPressure);
	GetConfigDouble(root, "maxPumpPressure", maxPumpPressure);
	GetConfigDouble(root, "maxPumpTemp", maxPumpTemp);
	GetConfigInt(root, "motorPowerStepDelay", motorPowerStepDelay);	
	GetConfigInt(root, "openValveDelay", openValveDelay);	
			
	
	if (!GetConfigInt(root, "emergencyStop.novaPower", emergencyStopNovaPower))
	{
		logger.Write(DLogger::ERROR, "missing parameter %s", "emergencyStop.novaPower");
		return false;
	}
		
	if (!GetConfigInt(root, "emergencyStop.drainDuration", emergencyStopDrainDuration))
	{
		logger.Write(DLogger::ERROR, "missing parameter %s", "emergencyStop.novaPower");
		return false;
	}

		
	string frontAgentHost;
	int frontAgentPort;
	
	if (GetConfigString(root, "frontAgent.host", frontAgentHost) && GetConfigInt(root, "frontAgent.port", frontAgentPort))
	{
		frontClient = Subscribe(frontAgentHost.c_str(), frontAgentPort);
		if (frontClient == NULL)
		{
			logger.Write(DLogger::ERROR, "frontAgent subscribtion fail");
			return false;
		}
	}
		
	string navAgentHost;
	int navAgentPort;
	
	if (GetConfigString(root, "navAgent.host", navAgentHost) && GetConfigInt(root, "navAgent.port", navAgentPort))
	{
		navClient = Subscribe(navAgentHost.c_str(), navAgentPort);
		if (navClient == NULL)
		{
			logger.Write(DLogger::ERROR, "navAgent subscribtion fail");
			return false;
		}
	}
	
	if (!GetConfigInt(root, "navAgent.port", navAgentPort))
	{
		logger.Write(DLogger::ERROR, "missing parameter %s", "navAgent.port");
		return false;
	}
	
	int maxPower;
	if (!GetConfigInt(root, "nova.maxPower", maxPower))
	{
		logger.Write(DLogger::ERROR, "missing parameter %s", "nova.maxPower");
		return false;
	}	
	nova.SetMaxPower(maxPower);
	
	int timeoutEcho;
	if (GetConfigInt(root, "nova.timeoutEcho", timeoutEcho))
		nova.SetTimeoutEcho(timeoutEcho);
	
	// pseudo sensor
	
	
	double maxVal;
	
	if (GetConfigDouble(root, "nova.maxVoltage", maxVal))
		thrusterNode->GetSensorByName("novaVoltage")->SetMaxValue(maxVal);

	if (GetConfigDouble(root, "nova.maxCurrent", maxVal))
		thrusterNode->GetSensorByName("novaCurrent")->SetMaxValue(maxVal);

	if (GetConfigDouble(root, "nova.maxTemp", maxVal))
		thrusterNode->GetSensorByName("novaMotTemp")->SetMaxValue(maxVal);
	
	if (GetConfigDouble(root, "maxConsPower", maxVal))
		GetBiteNode("sensors")->GetSensorByName("consPower")->SetMaxValue(maxVal);
										
	return true;
}


bool RearAgent::IsPressureSensorsOK(int valve)
{
	if (valveCtrl.IsOpen(ValveController::FTFV) && ftPressure > maxTankPressure)
	{
		logger.Write(DLogger::WARNING, "FTPS too high, close FTFV");
		lastMsg = "FTPS too high, close FTFV";
		valveCtrl.Close(ValveController::FTFV);
		if ((valve & (ValveController::FTFV | ValveController::RTFV)) == ValveController::FTFV)
			return false;
	}

	if (valveCtrl.IsOpen(ValveController::FTDV) && ftPressure < minTankPressure)
	{
	    p_MonitoringVolume->resetAv();
		logger.Write(DLogger::WARNING, "FTPS too low, close FTDV");
		lastMsg = "FTPS too low, close FTDV";
		valveCtrl.Close(ValveController::FTDV);
		if ((valve & (ValveController::FTDV | ValveController::RTDV)) == ValveController::FTDV)
			return false;
	}
		
	if (valveCtrl.IsOpen(ValveController::RTFV) && rtPressure > maxTankPressure)
	{
		logger.Write(DLogger::WARNING, "RTPS too high, close RTFV");
		lastMsg = "RTPS too high, close RTFV";
		valveCtrl.Close(ValveController::RTFV);
		if ((valve & (ValveController::FTFV | ValveController::RTFV)) == ValveController::RTFV)
			return false;
	}

	if (valveCtrl.IsOpen(ValveController::RTDV) && rtPressure < minTankPressure)
	{
	    p_MonitoringVolume->resetAr();
		logger.Write(DLogger::WARNING, "RTPS too low, close RTDV");
		lastMsg = "RTPS too low, close RTDV";
		valveCtrl.Close(ValveController::RTDV);
		if ((valve & (ValveController::FTDV | ValveController::RTDV)) == ValveController::RTDV)
			return false;
	}
		
	if (!valveCtrl.IsOpen(ValveController::RTFV) && !valveCtrl.IsOpen(ValveController::FTFV) &&
		(valve & (ValveController::FTFV | ValveController::RTFV)) == (ValveController::RTFV | ValveController::FTFV)) // FTFV & RTFV are closed
			return false;

	if (!valveCtrl.IsOpen(ValveController::RTDV) && !valveCtrl.IsOpen(ValveController::FTDV) &&
		(valve & (ValveController::FTDV | ValveController::RTDV)) == (ValveController::RTDV | ValveController::FTDV)) // FTDV & RTDV are closed
			return false;
			
	return true;
}



void RearAgent::StartMotor(DSequencer* seq, int power)
{
	seq->AddAction(new StartMotorAction(&nova)); // start motor (power 2%)
	for(int i = 2; i < power; i++)
	{
		seq->AddWaitAction(motorPowerStepDelay);
		seq->AddAction(new IncMotorPowerAction(this,&nova)); // increase power until reach desired power with 500 ms interval
	}
}

struct SOCCallibration
{
	int charge;
	double tCal0;
	double tCal23;
};

double RearAgent::ComputeStateOfCharge(double batteryVoltage, double batteryTemperature)
{
	static SOCCallibration cals[] = {
		{0,		0.000,	0.500},
		{10,	0.333,	0.567},
		{20,	0.417,	0.583},
		{30,	0.475,	0.600},
		{40,	0.508,	0.625},
		{50,	0.542,	0.658},
		{60,	0.575,	0.683},
		{70,	0.600,	0.717},
		{80,	0.650,	0.767},
		{90,	0.750,	0.875},
		{100,	0.917,	1.000}
	};
	
	double vMin = 80.0;
	double vMax = 110.0;
	double tNom = 23.0;
	
	double ratio = (batteryVoltage - vMin) / (vMax - vMin);
	//printf("ratio=%f\n", ratio);
	if (ratio >= 1)
		return 1.0;
		
	if (ratio <= 0)
		return 0.0;
		
	if (batteryTemperature >= tNom)
	{
		int i;
		for(i = 10; i >= 0; i--)
		{
			if (cals[i].tCal23 < ratio)
				break;
		}
		//printf("i=%d\n", i);
		if (i < 0) return  0.0;
		double ret = 0.1 * (i + (ratio - cals[i].tCal23)/(cals[i+1].tCal23 - cals[i].tCal23));
		return ret;
	}
	
	double rt = batteryTemperature / tNom;
	
	int i;
	for(i = 10; i >= 0; i--)
	{
		double val = (1 -rt) * cals[i].tCal0 + rt * cals[i].tCal23;
		
		//printf("val=%f\n", val);
		if (val < ratio)
			break;
	}
	//printf("i=%d\n", i);
	if (i < 0)
		return 0.0;
		
	double low = (1 -rt) * cals[i].tCal0 + rt * cals[i].tCal23;
	double high = (1 -rt) * cals[i+1].tCal0 + rt * cals[i+1].tCal23;
	
	return 0.1 * (i + (ratio - low) / (high - low));
	
}	
