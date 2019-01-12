#include "ImuAgent.h"

#include "DLogger.h"

#include <stdio.h>
#include <json/writer.h>
#include "RTIMULib.h"


static DLogger logger("ImuAgent");

int main(int argc, char* argv[])
{
	ImuAgent agent;	
	if (!agent.ParseArg(argc, argv))
		return -1;
	
	return agent.Run();
}


ImuAgent::ImuAgent()
{
}

bool ImuAgent::OnStartup()
{
	if (!DServerApp::OnStartup())
		return false;
		
		
    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");

    imu = RTIMU::createIMU(settings);

    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        logger.Write(DLogger::ERROR, "No IMU found\n");
        return false;
    }

    //  set up IMU

    imu->IMUInit();
		
	timer.Start(monitorPeriod);
	
	return true;
}

void ImuAgent::OnIterate()
{
	DServerApp::OnIterate();

	while (imu->IMURead()) 
	{
		RTIMU_DATA imuData = imu->getIMUData();

		if (timer.IsExpired())
		{
			struct timeval tv;
			DTimer::GetTimeOfDay(&tv);

			Json::Value root;
			root["t"] = (Json::UInt)tv.tv_sec;
			/*
			root["roll"] = imuData.fusionPose.x() * RTMATH_RAD_TO_DEGREE;
			root["pitch"] = imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE;
			root["yaw"] = imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE;
			*/

			root["oriType"] = "MAGNETIC";
			root["id"] = "IMU";
			root["ok"] = true;
			root["attCov"].append(0.0);
			root["attCov"].append(0.0);
			root["attCov"].append(0.0);
			root["att"].append(imuData.fusionPose.x() * RTMATH_RAD_TO_DEGREE); // roll
			root["att"].append(imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE); // pitch
			root["att"].append(imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE); // yaw
			root["acc"].append(imuData.accel.x()); // ax [g]
            root["acc"].append(imuData.accel.y()); // ay [g]
            root["acc"].append(imuData.accel.z()); // az [g]
					
			Json::FastWriter writer;
			string msg = writer.write(root);
			size_t idx = msg.rfind("}");
			char buff[16];
			sprintf(buff, "%03d", tv.tv_usec/1000);
			//printf("buff=%s\n", buff);
			msg = msg.substr(0, idx) + buff + msg.substr(idx); // force date in ms
		
			logger.Write(DLogger::DEBUG, "Msg=%s", msg.c_str());
			server.Broadcast(msg.c_str());
			timer.Start(monitorPeriod);
		}

	}
						
}


bool ImuAgent::OnConfig(Json::Value& root)
{
	if (!DServerApp::OnConfig(root))
		return false;
	
	if (!root["monitorPeriod"].isInt())
		monitorPeriod = 1000; // 1sec
	else
		monitorPeriod = root["monitorPeriod"].asInt();
	
	return true;
}

