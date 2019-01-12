/*
 * CanMsg.hpp
 *
 *  Created on: 4 juil. 2014
 *      Author: win208adm
 */

#ifndef CANMSG_HPP_
#define CANMSG_HPP_

extern "C"
{

typedef struct CellStatus
{
   unsigned char address;
   unsigned short voltage;
   signed char temperatureOne;
   signed char temperatureTwo;
} SCellStatus;

typedef struct BmsStatus
{
   unsigned char stateOfCharge;
   unsigned short health;
   short current;
} SBmsStatus;

typedef struct BmsSecu
{
   int temperatureError;
   int minVoltage;
   int maxVoltage;
} SBmsSecu;

} // extern "C"

struct CANMsgCodec
{
    virtual void decode(const unsigned char * p) = 0;
};

class CellStatusCodec : public CANMsgCodec
{
 public:
    void decode(const unsigned char * p);

    SCellStatus& getCellStatus()
    {
       return m_cellStatus;
    }
private:
    SCellStatus m_cellStatus;
};

class BmsStatusCodec : public CANMsgCodec
{
 public:
    void decode(const unsigned char * p);

    SBmsStatus& getBmsStatus()
    {
       return m_bmsStatus;
    }
private:
    SBmsStatus m_bmsStatus;
};

class BmsSecuCodec : public CANMsgCodec
{
 public:
    void decode(const unsigned char * p);

    const SBmsSecu& getBmsSecu() const
    {
       return m_bmsSecu;
    }
private:
    SBmsSecu m_bmsSecu;
};

#endif /* CANMSG_HPP_ */

