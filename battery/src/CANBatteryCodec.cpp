
#include "CANBatteryCodec.hpp"

CANBatteryCodec::CANBatteryCodec()
{
}

CANBatteryCodec::~CANBatteryCodec()
{
}

void CANBatteryCodec::decode(const struct can_frame& frame)
{
        char buf[256] = { '\0' };
    switch(frame.can_id)
    {
       case 0x531:
            bmsStatusCodec.decode(frame.data);

            printf("soc:%u%% health: %hu cur(mA): %hd\n", bmsStatusCodec.getBmsStatus().stateOfCharge,
                    bmsStatusCodec.getBmsStatus().health,
                    bmsStatusCodec.getBmsStatus().current);

            sprintf(buf, "BMS_STA;%u;%hu;%hd\n", bmsStatusCodec.getBmsStatus().stateOfCharge,
                    bmsStatusCodec.getBmsStatus().health,
                    bmsStatusCodec.getBmsStatus().current);
            logger.log(buf);
            break;
       case 0x532:
            cellStatusCodec.decode(frame.data);
            printf("add:%u volt(cV):%hu t1:%hhd t2:%hhd\n", cellStatusCodec.getCellStatus().address,
                    cellStatusCodec.getCellStatus().voltage,
                    cellStatusCodec.getCellStatus().temperatureOne,
                    cellStatusCodec.getCellStatus().temperatureTwo);

            sprintf(buf, "CELL_STA;%u;%hu;%hd;%hd\n", cellStatusCodec.getCellStatus().address,
                    cellStatusCodec.getCellStatus().voltage,
                    cellStatusCodec.getCellStatus().temperatureOne,
                    cellStatusCodec.getCellStatus().temperatureTwo);
            logger.log(buf);

            break;
       case 0x533:
            bmsSecuCodec.decode(frame.data);
            printf("Terr:%d VminErr:%d VMaxErr:%d\n", bmsSecuCodec.getBmsSecu().temperatureError,
                    bmsSecuCodec.getBmsSecu().minVoltage,
                    bmsSecuCodec.getBmsSecu().minVoltage);

            sprintf(buf, "SECU;%d;%d;%d\n", bmsSecuCodec.getBmsSecu().temperatureError,
                    bmsSecuCodec.getBmsSecu().minVoltage,
                    bmsSecuCodec.getBmsSecu().minVoltage);
            logger.log(buf);

            break;
       default:
        printf("UNKNOWN\n");
        break;
    }
}
