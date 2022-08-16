#include"16f628a.h"
#include"chaos.h"

sbit rx_pin at PORTA.B2;
sbit tx_pin at PORTA.B1;
sbit status_pin at PORTA.B3;
sbit toggle_pin at PORTB.B7;
sbit speed_bit0_pin at PORTA.B0;
sbit speed_bit1_pin at PORTA.B7;
sbit speed_bit2_pin at PORTA.B6;

unsigned char my_address, recipient_address, host_address;

int i = 0;

void read_input()
{
	if (!IS_RA5_ON)//Key pressed
	{
		for (i = 0;i < 1000;++i)
		{
			Delay_us(250);
			chaos_Scan();
			Delay_us(250);
		}
	}

	chaos_Scan();
}

void init()
{
	//PortA Initialization
	TRISA = BIT2_ON | BIT4_ON | BIT5_ON;
	CMCON = BIT0_ON | BIT1_ON | BIT2_ON;
	VRCON = ZERO;

	//PortB Initialization
	TRISB = BIT7_OFF;

	
	TXSTA = ZERO;
	RCSTA = ZERO;

	PORTA = ZERO;
	PORTB = ZERO;

}


int main()
{
	init();
	chaos_UART_Init(0x0e, 0x0e);
	while (1)
		read_input();
}