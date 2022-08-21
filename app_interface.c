#include"app_interface.h"

int j;


void AppInterface_init(char mode, int baud) {
        /*
        Steps to follow when setting up a Synchronous Master
        Transmission:
        1. Initialize the SPBRG register for the appropriate
        baud rate
        2. Enable the synchronous master serial port by
        setting bits SYNC, SPEN, and CSRC.
        */
        long fo = Clock_MHz();
        fo *= 1000000;
        SPBRG = fo/(4*baud) - 1;
        TXSTA = BIT4_ON;//SYNC
        RCSTA = BIT7_ON;//SPEN
        TXSTA.F7 = mode;
}

int AppInterface_readAppData(union Payload* p)
{
        /*
        3. Ensure bits CREN and SREN are clear.
        4. If interrupts are desired, then set enable bit
        RCIE.
        5. If 9-bit reception is desired, then set bit RX9.
        6. If a single reception is required, set bit SREN.
        For continuous reception set bit CREN.
        7. Interrupt flag bit RCIF will be set when reception
        is complete and an interrupt will be generated if
        enable bit RCIE was set.
        8. Read the RCSTA register to get the ninth bit (if
        enabled) and determine if any error occurred
        during reception.
        9. Read the 8-bit received data by reading the
        RCREG register.
        10. If any error occurred, clear the error by clearing
        bit CREN.
        */
        RCSTA |= BIT4_ON;
        for (j = 0;j < BUFFER_SIZE;j++) {
                while (!PIR1.F5);
                p->buffer[j] = RCREG;
        }
        RCSTA &= BIT4_OFF;
        return j;
}

void AppInterface_writeAppData(union Payload *p)
{
        /*
        3. If interrupts are desired, then set enable bit
        TXIE.
        4. If 9-bit transmission is desired, then set bit TX9.
        5. Enable the transmission by setting bit TXEN.
        6. If 9-bit transmission is selected, the ninth bit
        should be loaded in bit TX9D.
        7. Start transmission by loading data to the TXREG
        register.
        */
        TXSTA |= BIT5_ON;//SYNC
        for (j = 0;j < BUFFER_SIZE;j++) {
                while (!TXSTA.F1)//TRMT
                        TXREG = p->buffer[j];
        }
        TXSTA &= BIT5_OFF;//SYNC
}