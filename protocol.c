#include"protocol.h"

static char error;
static unsigned char i;
static unsigned char destAddress;
static unsigned char tmr0_;
static unsigned char value;
static unsigned char header;
static unsigned char address;

void(*now)(void) = 0;
void(*future)(void) = 0;
void shortSilence();
void doNothing();
char setAfterCountdown(unsigned char t, void(*lhs)(), void(*rhs)());

void seekSilence(int counter)
{
    tmr0_ = TMR0;
    shortSilence();
	if(tmr0_ == TMR0)
		return;
    while (tmr0_ != TMR0 && setAfterCountdown(--counter, future, doNothing)==FALSE) {
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

char setAfterCountdown(unsigned char t, void(*lhs)(), void(*rhs)()) {
	if (t > 0) return FALSE;
	lhs = rhs;
	return TRUE;
}

void doNothing() {
}

void sendByte() {
	Soft_UART_Write(value);
}

char transmit(unsigned char destAddr, unsigned char b) {
	if (canTransmit() == FALSE)
		return FALSE;
	for (i = 0;i < 255;++i) {
		TOGGLE_RA6;
		Delay_us(MicroSecPerBit);
	}
	longSilence();
	Soft_UART_Write(MSG_HEADER);
	shortSilence();
	Soft_UART_Write(destAddr);
	shortSilence();
	value = b;
	now = doNothing;
	future = sendByte;
	seekSilence(512);
	now();
	TMR0 = TMR0_Offset;
	return (now == doNothing) ? FALSE : TRUE;
}

void readDestAddress() {
	header = Soft_UART_Read(&error);
	destAddress = Soft_UART_Read(&error);
}

void readByte() {
	value = Soft_UART_Read(&error);
	shortSilence();
}

char receive(unsigned char* v) {
	if (canReceive() == FALSE)
		return FALSE;
	now = doNothing;
	future = readDestAddress;
	seekSilence(512);
	now();
	if (address != destAddress || header != MSG_HEADER)
		return FALSE;
	for (i = 0;i < 255;++i) {
		TOGGLE_RA6;
		Delay_us(MicroSecPerBit);
	}
	readByte();
	*v = value;
	TMR0 = TMR0_Offset;
	destAddress = header = 0;
	return TRUE;
}


void protocolInit(unsigned char addr) {
	//Set TMR0 clock to external clock input
	// rising edge <--------- Important
	OPTION_REG = BIT5_ON | BIT3_ON | BIT2_ON | BIT1_ON | BIT0_ON;
	INTCON = 0;
	TMR0 = TMR0_Offset;
	Soft_UART_Init(&PORTA, 7, 6, BitsPerSec, 0);
	address = addr;
	shortSilence();
}