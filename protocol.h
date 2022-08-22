//#include"16f628a.h"
#include"defs.h"

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define TRUE 1
#define FALSE 0

#define MicroSecPerBit (8000/Clock_kHz())
#define BitsPerSec (1000 * Clock_kHz()/8)
#define TMR0_Offset 128

#define MSG_HEADER 0xea

#define tx 6
#define rx 7


void protocolInit();
void transmit();
void void receive();


#endif
