#include "CanMsg.hpp"

void CellStatusCodec::decode(const unsigned char * p) {
	m_cellStatus.address = p[0];
	m_cellStatus.voltage = (p[1] << 8 | p[2]) & 0xFFFF;
	m_cellStatus.temperatureOne = (signed char) p[3];
	m_cellStatus.temperatureTwo = (signed char) p[4];
}

void BmsStatusCodec::decode(const unsigned char * p) {
	m_bmsStatus.stateOfCharge = p[0];
	m_bmsStatus.health = (p[1] << 8 | p[2]) & 0xFFFF;
	m_bmsStatus.current = (short) ((p[3] << 8 | p[4]) & 0xFFFF);
}

void BmsSecuCodec::decode(const unsigned char * p) {
	m_bmsSecu.temperatureError = p[0] & 0x01;
	m_bmsSecu.minVoltage = p[0] & 0x02;
	m_bmsSecu.maxVoltage = p[0] & 0x04;
}


