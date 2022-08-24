#include"protocol.h"

unsigned char myAddress = 0xef;
unsigned char dat;

int main()
{
	TRISB.F5 = 0;
	protocolInit(myAddress);
	while (1) {
		if (receive(&dat) == TRUE)
			TOGGLE_RB5;
	}
}