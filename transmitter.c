#include"protocol.h"


int main()
{

        protocolInit();
        while(1){
            Delay_ms(1000);
            transmit();
        }
}