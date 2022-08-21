# chaos
Communication protocol for home appliances using 433MHz transmitters and receivers

The protocol uses TMR0 to synchronize transmitt and receive
The receiver output is connected to the external clock source 
of TMR0.

Protocol Algorithm

Init:
- configure TMR0 to use externel clock source
- set TMR0 equal to sentinel value
- set duration per bit
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
