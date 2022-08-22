#include"protocol.h"

//static unsigned char tmr0Offset;
//static unsigned long longVal;
static char error;
static unsigned char i;
static unsigned char srcAddr = 0x1e;
static unsigned char destAddr = 0x0e;
static unsigned char tmr0_;
static unsigned char block = FALSE;
static unsigned char val = 0xde;
static unsigned char header;
static unsigned char address;
static unsigned char temp;

void(*now)(void) = 0;
void(*future)(void) = 0;
void shortSilence();

void seekSilence(void)
{
	tmr0_ = TMR0;
	shortSilence();
	while (tmr0_ != TMR0) {
		tmr0_ = TMR0;
		shortSilence();
	}
	now = future;
}

char canTransmit() {
	if (TMR0 <= TMR0_Offset) {
		TMR0 = TMR0_Offset;
		return TRUE;
	}
	return FALSE;
}

char canReceive() {
	if (TMR0 > TMR0_Offset) {
		return TRUE;
	}
	return FALSE;
}

void shortSilence() {
	OFF_RA6;
	Delay_us(3 * MicroSecPerBit);
}

void longSilence() {
	OFF_RA6;
	Delay_us(7 * MicroSecPerBit);
}

void blocking() {
	while (block == TRUE);
}

void doNothing() {
}

/*
Transmit Algorithm:
- call collision avoidance function
- if TMR0 == sentinel(it is OK to transmit)
- transmit pulses to cause TMR0 interrupt on the recipient
- transmit message header
- delay for t >= 2 * duration per bit
- transmit source address
- delay for t >= 2 * duration per bit
- transmit destination address
- delay for t >= 2 * duration per bit
- set TMR0 equal to sentinel value
- enable interrupt
- On interrupt transmit data
*/

void sendData() {
	Soft_UART_Write(val);
}

void transmit() {
	if (canTransmit() == FALSE)
		return;
	for (i = 0;i < 255;++i) {
		TOGGLE_RA6;
		Delay_us(MicroSecPerBit);
	}
	longSilence();
	Soft_UART_Write(MSG_HEADER);
	shortSilence();
	Soft_UART_Write(destAddr);
	longSilence();
	now = doNothing;
	future = sendData;
	seekSilence();
	now();
	TMR0 = TMR0_Offset;
}

/*
Receive Algorithm:
- if TMR0 != sentinel(incoming message)
- enable TMR0 interrupt
- On interrupt
- delay for t >= 2 * duration per bit
- read a byte from receive
- if byte == message header
- read 2 byte from receiver
- if byte[0] == myAddress
- transmit pulses to cause TMR0 interrupt on the sender
- else
- wait for t = time to transmit pulses
- read remaining data from sender
- if destinationAdderess == myAddress
- process message
- else
- if findOnMyList(destinationAdderess) == true
- wait to transmit data
- forward/transmit message
- else
- save sourceAddress
*/

void readDestAddress() {
	header = Soft_UART_Read(&error);
	address = Soft_UART_Read(&error);
}

void readData() {
	temp = Soft_UART_Read(&error);
	if (temp == val)
		TOGGLE_RB5;
	shortSilence();
}

void receive() {
	if (canReceive() == FALSE)
		return;
	now = doNothing;
	future = readDestAddress;
	seekSilence();
	now();
	if (address != destAddr)
		return;
	for (i = 0;i < 255;++i) {
		TOGGLE_RA6;
		Delay_us(MicroSecPerBit);
	}
	readData();
	TMR0 = TMR0_Offset;
}


void protocolInit() {
	//Set TMR0 clock to external clock input
	// rising edge <--------- Important
	OPTION_REG = BIT5_ON | BIT3_ON | BIT2_ON | BIT1_ON | BIT0_ON;
	INTCON = 0;
	TMR0 = TMR0_Offset;
	Soft_UART_Init(&PORTA, 7, 6, BitsPerSec, 0);
	TRISB.F5 = 0;
	shortSilence();
}