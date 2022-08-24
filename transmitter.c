#include"protocol.h"

unsigned char srcAddress = 0xea;
unsigned char destAddress = 0xef;

int main()
{
	TRISB.F5 = 0;
    protocolInit(srcAddress);
    while(1){
		Delay_ms(1000);
		if (transmit(destAddress, 0xaa) == TRUE)
			TOGGLE_RB5;
    }
}