#include"16f628a.h"

#ifndef sfr
#define sfr
#endif

#ifndef sbit
#define sbit char
#endif

#ifndef at
#define at =
#endif

#ifndef Delay_us
#define Delay_us
#endif

#ifndef Delay_ms
#define Delay_ms
#endif

char TMR0;


#define BIT0_ON 0x01
#define BIT1_ON 0x02
#define BIT2_ON 0x04
#define BIT3_ON 0x08
#define BIT4_ON 0x10
#define BIT5_ON 0x20
#define BIT6_ON 0x40
#define BIT7_ON 0x80
#define ZERO    0x00
#define ONE     0x01

#define BIT0_OFF 0xFE
#define BIT1_OFF 0xFD
#define BIT2_OFF 0xFB
#define BIT3_OFF 0xF7
#define BIT4_OFF 0xEF
#define BIT5_OFF 0xDF
#define BIT6_OFF 0xBF
#define BIT7_OFF 0x7F

#define U_DURATION_PER_BIT 120
#define MSG_HEADER 0xFC

#define APPLY_PRESCALER_TO_TMR0  (OPTION_REG.B3 = 0)
#define SET_CLOCK_SRC_TMR0_INTERNAL (OPTION_REG.B5 = 0)
#define SET_CLOCK_SRC_TMR0_EXTERNAL (OPTION_REG.B5 = 1)
#define APPLY_PRESCALER_TO_WDT  (OPTION_REG.B3 = 1)
#define CLEAR_TMR0_INTERRUPT (INTCON.B2 = 0)

#define DISABLE_INTERRUPT (INTCON.B7 = 0)
#define ENABLE_INTERRUPT  (INTCON.B7 = 1)


#define ENABLE_SENDING        (status_pin = 1)
#define ENABLE_RECEIVING (status_pin = 0)

#define SENTINEL 224
#define SENTINEL_TIME_OUT  32

#define SERIAL_OUT_DELAY 150
#define SERIAL_OUT_LOOP 8
#define SERIAL_OUT_ONE 8
#define SERIAL_OUT_ZERO 1
#define SERIAL_ONE_THRESHOLD 4
#define SERIAL_IN_DELAY 25
#define SERIAL_IN_LOOP (2 * SERIAL_OUT_LOOP)
#define BYTE 8

#define TRUE ONE
#define FALSE ZERO

#define ON_STATE 30
#define OFF_STATE 40

#define SUCCESS 0x08
#define FAILED -1

#define MAX_SPEED 5

#define TOGGLE_DEVICE_STATE 0
#define GET_DEVICE_STATE 1
#define SET_DEVICE_SPEED 2
#define GET_DEVICE_SPEED 3
#define REGISTER_ME 4
#define HANDLE_BROADCAST    5
#define HOST_FEEDBACK    6
#define STATUS_MESSAGE_NONE 7
#define STATUS_MESSAGE_INCOMING 8
#define STATUS_MESSAGE_OUTGOING 9

#define DIRECT 0
#define BROADCAST    1

#define ALL_ACCESS 0xff

#define PING_TIMES 50
#define MAX_NODES 32

#define STATUS_MESSAGE_RECEIVED 6



extern sfr sbit rx_pin;
extern sfr sbit tx_pin;
extern sfr sbit status_pin;
extern sfr sbit toggle_pin;
extern sfr sbit speed_bit0_pin;
extern sfr sbit speed_bit1_pin;
extern sfr sbit speed_bit2_pin;


void chaos_UART_Init(unsigned char my_address, unsigned char host_address);
void chaos_Post(unsigned char message, unsigned char extra, unsigned char recipient_address);
signed char chaos_Register_me();
void chaos_feedback_listener();
void chaos_send_listener();
void chaos_receive_listener();

#define chaos_Scan(){\
                          chaos_receive_listener();\
                          chaos_feedback_listener();\
                          chaos_send_listener();\
                    }