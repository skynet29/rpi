/*
 * CANReceiver.hpp
 *
 *  Created on: 4 juil. 2014
 *      Author: win208adm
 */

#ifndef CANRECEIVER_HPP_
#define CANRECEIVER_HPP_

#include  <vector>

#include "CANCodec.hpp"

class CANReceiver
{
public:
  CANReceiver();
  int initialize();
  int start();
  void stop();

  virtual ~CANReceiver();

  void addCodec(CANCodec * canCodec);

private:
  CANReceiver(const CANReceiver& rhs);
  CANReceiver& operator=(const CANReceiver& rhs);

    void reset()
	{
	   m_fdSocket = -1;
	   m_isRunning = false;
	   m_isBound = false;
	}


  int m_fdSocket;
  bool m_isRunning;
  bool m_isBound;

 std::vector<CANCodec * > m_codecs;

};

#endif /* CANRECEIVER_HPP_ */

