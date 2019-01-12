
// Standard C++ lib, STL
#include <cstdio>
#include <cstdlib>
#include <cstring>

// POSIX.1
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "CANReceiver.hpp"

CANReceiver::CANReceiver() : m_fdSocket(-1),
m_isRunning(false), m_isBound(false)
{
}

CANReceiver::~CANReceiver()
{
}

void CANReceiver::addCodec(CANCodec * canCodec)
{
   if (canCodec) { m_codecs.push_back(canCodec); }
}


int CANReceiver::initialize()
{
    int rc = 0;

	struct sockaddr_can addr;
	m_fdSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (m_fdSocket < 0)
	{
	   perror("CANReceiver::initialize() - error while opening socket");
	   rc = -1;
	}

	if (rc >= 0)
	{
		struct ifreq ifr;
		strcpy(ifr.ifr_name, "can0");
		ioctl(m_fdSocket, SIOCGIFINDEX, &ifr);

		addr.can_family = AF_CAN;
		addr.can_ifindex = ifr.ifr_ifindex;
		rc = bind(m_fdSocket, (struct sockaddr*)&addr, sizeof(addr));
		if (rc < 0)
		{
		   perror("CANReceiver::initialize() - socket bind error");
		   rc = -2;
		}
		else 
		{
			m_isBound = true;
		}
	}

	return rc;
}

int CANReceiver::start()
{
   int rc = 0;
   fd_set rdfs;
   m_isRunning = true;
 
   if (!m_isBound)
   {
      fprintf(stderr, "CANReceiver::start() - socket not bound\n");
      rc = -1;
	  reset();
	  return rc;
   }

   while (m_isRunning)
   {
      int ready = 0; 
      FD_ZERO(&rdfs);
	  FD_SET(m_fdSocket, &rdfs);
 
	  struct timespec ts = { 2, 0 };
	  ready = pselect(m_fdSocket+1, &rdfs, 0, 0, &ts, 0);
	  if (ready)
	  {
	     if (FD_ISSET(m_fdSocket, &rdfs))
		 {
		    struct can_frame frame;
		    int nbBytes = read(m_fdSocket, &frame, sizeof(frame));

			if (nbBytes < 0)
			{
				perror("CANReceiver::start() - socket read issue");
				reset();
				rc = -2;
				break;
			}
			else if (nbBytes < sizeof(frame))
			{
				fprintf(stderr, "CANReceiver::start() - incomplete CAN frame\n");
			}
			else
			{
			   printf("CAN: ID:%04x DLC:%02x Data:", frame.can_id, frame.can_dlc);
			   fflush(stdout);
			   for (int i=0; i<frame.can_dlc; ++i)
			   {
			     printf(" %02x", frame.data[i]);
			   }
               fflush(stdout);	puts(""); 

               for (std::vector<CANCodec*>::const_iterator cIt = m_codecs.begin();
                              cIt != m_codecs.end();
                               ++cIt)
               {
                  (*cIt)->decode(frame);
               }

			}
		 }
	  }
	  else if (ready < 0)
	  {
		puts("CANReceiver::start() - select()/pselect() -> not ready");
	  }
	  else
	  {
		puts("CANReceiver::start() - no data within 2 seconds");
	} 

   }

   return rc;

}


void CANReceiver::stop()
{
   m_isRunning = false;
}


