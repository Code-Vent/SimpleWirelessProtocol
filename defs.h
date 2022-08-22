#ifndef DEF_H
#define DEF_H

#define BIT0_ON 0b00000001
#define BIT1_ON 0b00000010
#define BIT2_ON 0b00000100
#define BIT3_ON 0b00001000
#define BIT4_ON 0b00010000
#define BIT5_ON 0b00100000
#define BIT6_ON 0b01000000
#define BIT7_ON 0b10000000
#define ZERO    0b00000000
#define ONE     0b00000001
#define ONES     0b11111111


#define BIT0_OFF 0b11111110
#define BIT1_OFF 0b11111101
#define BIT2_OFF 0b11111011
#define BIT3_OFF 0b11110111
#define BIT4_OFF 0b11101111
#define BIT5_OFF 0b11011111
#define BIT6_OFF 0b10111111
#define BIT7_OFF 0b01111111




#define TOGGLE_RA0 ( PORTA.F0 = (PORTA.F0 == 0)? 1 : 0 )
#define ON_RA0 (PORTA.F0 = 1)
#define OFF_RA0 (PORTA.F0 = 0)
#define IS_RA0_ON (PORTA.F0 == 1)

#define TOGGLE_RA1 ( PORTA.F1 = (PORTA.F1 == 0)? 1 : 0 )
#define ON_RA1 (PORTA.F1 = 1)
#define OFF_RA1 (PORTA.F1 = 0)
#define IS_RA1_ON (PORTA.F1 == 1)

#define TOGGLE_RA2 ( PORTA.F2 = (PORTA.F2 == 0)? 1 : 0 )
#define ON_RA2 (PORTA.F2 = 1)
#define OFF_RA2 (PORTA.F2 = 0)
#define IS_RA2_ON (PORTA.F2 == 1)

#define TOGGLE_RA3 ( PORTA.F3 = (PORTA.F3 == 0)? 1 : 0 )
#define ON_RA3 (PORTA.F3 = 1)
#define OFF_RA3 (PORTA.F3 = 0)
#define IS_RA3_ON (PORTA.F3 == 1)

#define TOGGLE_RA4 ( PORTA.F4 = (PORTA.F4 == 0)? 1 : 0 )
#define ON_RA4 (PORTA.F4 = 1)
#define OFF_RA4 (PORTA.F4 = 0)
#define IS_RA4_ON (PORTA.F4 == 1)

#define TOGGLE_RA5 ( PORTA.F5 = (PORTA.F5 == 0)? 1 : 0 )
#define ON_RA5 (PORTA.F5 = 1)
#define OFF_RA5 (PORTA.F5 = 0)
#define IS_RA5_ON (PORTA.F5 == 1)

#define TOGGLE_RA6 ( PORTA.F6 = (PORTA.F6 == 0)? 1 : 0 )
#define ON_RA6 (PORTA.F6 = 1)
#define OFF_RA6 (PORTA.F6 = 0)
#define IS_RA6_ON (PORTA.F6 == 1)

#define TOGGLE_RA7 ( PORTA.F7 = (PORTA.F7 == 0)? 1 : 0 )
#define ON_RA7 (PORTA.F7 = 1)
#define OFF_RA7 (PORTA.F7 = 0)
#define IS_RA7_ON (PORTA.F7 == 1)

#define TOGGLE_RB0 ( PORTB.F0 = (PORTB.F0 == 0)? 1 : 0 )
#define ON_RB0 (PORTB.F0 = 1)
#define OFF_RB0 (PORTB.F0 = 0)
#define IS_RB0_ON (PORTB.F0 == 1)

#define TOGGLE_RB1 ( PORTB.F1 = (PORTB.F1 == 0)? 1 : 0 )
#define ON_RB1 (PORTB.F1 = 1)
#define OFF_RB1 (PORTB.F1 = 0)
#define IS_RB1_ON (PORTB.F1 == 1)

#define TOGGLE_RB2 ( PORTB.F2 = (PORTB.F2 == 0)? 1 : 0 )
#define ON_RB2 (PORTB.F2 = 1)
#define OFF_RB2 (PORTB.F2 = 0)
#define IS_RB2_ON (PORTB.F2 == 1)

#define TOGGLE_RB3 ( PORTB.F3 = (PORTB.F3 == 0)? 1 : 0 )
#define ON_RB3 (PORTB.F3 = 1)
#define OFF_RB3 (PORTB.F3 = 0)
#define IS_RB3_ON (PORTB.F3 == 1)

#define TOGGLE_RB4 ( PORTB.F4 = (PORTB.F4 == 0)? 1 : 0 )
#define ON_RB4 (PORTB.F4 = 1)
#define OFF_RB4 (PORTB.F4 = 0)
#define IS_RB4_ON (PORTB.F4 == 1)

#define TOGGLE_RB5 ( PORTB.F5 = (PORTB.F5 == 0)? 1 : 0 )
#define ON_RB5 (PORTB.F5 = 1)
#define OFF_RB5 (PORTB.F5 = 0)
#define IS_RB5_ON (PORTB.F5 == 1)

#define TOGGLE_RB6 ( PORTB.F6 = (PORTB.F6 == 0)? 1 : 0 )
#define ON_RB6 (PORTB.F6 = 1)
#define OFF_RB6 (PORTB.F6 = 0)
#define IS_RB6_ON (PORTB.F6 == 1)

#define TOGGLE_RB7 ( PORTB.F7 = (PORTB.F7 == 0)? 1 : 0 )
#define ON_RB7 (PORTB.F7 = 1)
#define OFF_RB7 (PORTB.F7 = 0)
#define IS_RB7_ON (PORTB.F7 == 1)

#define ENABLE_TMR0_INTERRUPT (INTCON.F5 = 1)
#define DISABLE_TMR0_INTERRUPT (INTCON.F5 = 0)
#define CLEAR_TMR0_FLAG (INTCON.F2 = 0)

#define MAKE_RA0_INPUT (TRISIA.F0 = 1)
#define MAKE_RA0_OUTPUT (TRISIA.F0 = 0)
#define MAKE_RA1_INPUT (TRISIA.F1 = 1)
#define MAKE_RA1_OUTPUT (TRISIA.F1 = 0)
#define MAKE_RA2_INPUT (TRISIA.F2 = 1)
#define MAKE_RA2_OUTPUT (TRISIA.F2 = 0)
#define MAKE_RA3_INPUT (TRISIA.F3 = 1)
#define MAKE_RA3_OUTPUT (TRISIA.F3 = 0)
#define MAKE_RA4_INPUT (TRISIA.F4 = 1)
#define MAKE_RA4_OUTPUT (TRISIA.F4 = 0)

#define MAKE_RB0_INPUT (TRISB.F0 = 1)
#define MAKE_RB0_OUTPUT (TRISB.F0 = 0)
#define MAKE_RB1_INPUT (TRISB.F1 = 1)
#define MAKE_RB1_OUTPUT (TRISB.F1 = 0)
#define MAKE_RB2_INPUT (TRISB.F2 = 1)
#define MAKE_RB2_OUTPUT (TRISB.F2 = 0)
#define MAKE_RB3_INPUT (TRISB.F3 = 1)
#define MAKE_RB3_OUTPUT (TRISB.F3 = 0)
#define MAKE_RB4_INPUT (TRISB.F4 = 1)
#define MAKE_RB4_OUTPUT (TRISB.F4 = 0)
#define MAKE_RB5_INPUT (TRISB.F5 = 1)
#define MAKE_RB5_OUTPUT (TRISB.F5 = 0)
#define MAKE_RB6_INPUT (TRISB.F6 = 1)
#define MAKE_RB6_OUTPUT (TRISB.F6 = 0)
#define MAKE_RB7_INPUT (TRISB.F7 = 1)
#define MAKE_RB7_OUTPUT (TRISB.F7 = 0)


#endif

