#ifndef VALVECONTROLLER_H
#define VALVECONTROLLER_H

class ValveController
{
public:
	enum {
		WIV = 1,
		WEV = 2,
		RTFV = 4,
		RTDV = 8,
		FTFV = 16,
		FTDV = 32,
		THSV = 64,
		NOVA_PRECHARGE = 128,
		NOVA_MAIN = 256,
		ALL = 0x1FF
		};
	
	bool Init();
	
	void Open(int valve);
	void Close(int valve);
	void SetState(int valve);
	void InvertState(int valve);
	
	unsigned int GetState();
	bool IsOpen(unsigned valveId);
	void DumpState();
	
	static int GetValveId(const char* name);
	static const char* GetValveStr(unsigned idx);
	static int GetValveIdx(unsigned valveId);
	
};

#endif
