
#include "CANReceiver.hpp"

int main(void)
{
    CANReceiver canr;
    canr.initialize();
    canr.start();
    return 0;
}
