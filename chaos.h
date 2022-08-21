
#include"app_interface.h"

#define U_DURATION_PER_BIT 120
#define MSG_HEADER 0xFC

#define CLEAR_TMR0_INTERRUPT (INTCON.F2 = 0)

#define DISABLE_INTERRUPT (INTCON.F7 = 0)
#define ENABLE_INTERRUPT  (INTCON.F7 = 1)

#define ENABLE_SENDING        (status_pin = 1)
#define ENABLE_RECEIVING (status_pin = 0)

#define SENTINEL 224
#define SENTINEL_TIME_OUT  32

#define SENT                0x02
#define RECEIVED        0x06
#define OUT                0x02
#define IN        0x06


#define TRUE ONE
#define FALSE ZERO

#define SUCCESS 0x08
#define FAILED -1

#define STATUS_MESSAGE_NONE 7
#define STATUS_MESSAGE_INCOMING 8
#define STATUS_MESSAGE_OUTGOING 9

#define MAX_NODES 32

#define MASTER 1
#define SLAVE  0

#define rx_pin 4
#define tx_pin 1
#define SlaveNOP 0b00000000//sp1 sp0(0,0)=>idle
#define SlaveInit 0b00000001//sp1 sp0(0,1)=>Chaos_xxxxInit
#define SlaveRead 0b00000100//sp1 sp0(1,0)=>slaveRead<=>masterWrite
#define SlaveWrite 0b00000101//sp1 sp0(1,1)=>slaveWrite<=>masterRead

#define rx PORTA.F2
#define tx PORTA.F1
#define status_pin PORTA.F3
#define status_pin_dir TRISA.F3
#define sp0 PORTA.F0//use a pull-down resistor for this pin
#define sp0_dir TRISA.F0
#define sp1 PORTB.F7//use a pull-down resistor for this pin
#define sp1_dir TRISB.F7

typedef char(*Chaos_fn)(union Payload*);
void Chaos_slaveInit(int baud);
void Chaos_masterInit(int baud);
char Chaos_RFscan(union Payload*);
char Chaos_read(union Payload*);
char Chaos_write(union Payload*);
char Chaos_idle(union Payload*);
Chaos_fn Chaos_masterGetOperation();
void Choas_slaveSetOperation(char op);
void Choas_slaveSaveAddress(char addr);
void Choas_RFinit(unsigned baud);
extern void LoadInitPayload();