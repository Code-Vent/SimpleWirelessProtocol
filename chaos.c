#include"chaos.h"
#include"defs.h"

static char msg_status = STATUS_MESSAGE_NONE;
static volatile char recipient_busy = 1;
static volatile char sender_busy = 1;
static char waiting_to_send = FALSE;
static char error;
static char header;
static char num_of_reg_nodes = 1;
static unsigned char registered_nodes[MAX_NODES] = { 0x0e };
static unsigned char myAddress;
static char tmr0_, i;
static char* rfPort = &PORTA;


typedef signed char(*Protocol_fn)(union Payload*);
void Protocol_init(unsigned baud);
void Protocol_interrupt();
void Protocol_collisionGuard();
void Protocol_doSentinelTimeout();
void Protocol_dontSentinelTimeout();
void Protocol_sendPreamble();
signed char Protocol_send(union Payload* p);
signed char Protocol_receive(union Payload* p);
signed char Protocol_idle(union Payload* p);
void Protocol_statusReset(void);
Protocol_fn Protocol_receiveListener();
Protocol_fn Protocol_sendListener();
void Protocol_enlist(char addr);
signed char Protocol_find(char addr);
void Protocol_receiveHelper(union Payload*);


void Protocol_statusReset(void)
{
        TMR0 = SENTINEL;
        DISABLE_INTERRUPT;
        ENABLE_RECEIVING;
        CLEAR_TMR0_INTERRUPT;
        msg_status = STATUS_MESSAGE_NONE;
}

void interrupt(void)
{
        CLEAR_TMR0_INTERRUPT;

        if (msg_status == STATUS_MESSAGE_INCOMING)
        {
                ++sender_busy;
        }
        else if (msg_status == STATUS_MESSAGE_OUTGOING)
        {
                ++recipient_busy;
        }
        else
        {

        }
}

void Protocol_doSentinelTimeout()
{
        tx = 0;
        //clock TMR0 to overflow on the listening devices
        for (i = 0; i < SENTINEL_TIME_OUT; tx = 0, ++i)
        {
                Delay_us(U_DURATION_PER_BIT);
                tx = 1;
                Delay_us(U_DURATION_PER_BIT);
        }
}

void Protocol_dontSentinelTimeout()
{
        tx = 0;
        //dont clock TMR0 to overflow on the listening devices
        for (i = 0; i < SENTINEL_TIME_OUT; tx = 0, ++i)
        {
                Delay_us(U_DURATION_PER_BIT);
                tx = 0;
                Delay_us(U_DURATION_PER_BIT);
        }
}

void Protocol_sendPreamble()
{
        ENABLE_SENDING;
        Protocol_doSentinelTimeout();
        //To ensure that interrupt mode is entered in
        // the recipient***************
        Delay_us(U_DURATION_PER_BIT); // delta T
        Delay_us(U_DURATION_PER_BIT); // delta T
        Delay_us(U_DURATION_PER_BIT);
        Soft_UART_Write(MSG_HEADER);
        Delay_us(U_DURATION_PER_BIT);
        Delay_us(U_DURATION_PER_BIT);
}

signed char Protocol_send(union Payload* p)
{
        Protocol_collisionGuard();
        if (TMR0 == SENTINEL) {
                Protocol_sendPreamble();
                Delay_us(U_DURATION_PER_BIT);
                Soft_UART_Write(p->content.srcAddress);
                Delay_us(U_DURATION_PER_BIT);
                Soft_UART_Write(p->content.destAddress);
                Delay_us(U_DURATION_PER_BIT);
                recipient_busy = 1;
                ENABLE_RECEIVING;
                ENABLE_INTERRUPT;
                i = 0;

                // This should not take more than 40ms
                while (recipient_busy == 1)
                {
                        if (i++ < (3 * SENTINEL_TIME_OUT))
                        {
                                Delay_us(U_DURATION_PER_BIT);
                        }
                        else
                        {
                                  //Delay_ms(150);
                                Protocol_statusReset();
                                return FAILED;
                        }
                }
                recipient_busy = 1;
                DISABLE_INTERRUPT;
                ENABLE_SENDING;
                for (i = 0;i < BUFFER_SIZE - 2;i++) {
                        Delay_us(U_DURATION_PER_BIT);
                        Soft_UART_Write(p->content.data_[i]);
                        Delay_us(U_DURATION_PER_BIT);
                }
                return SUCCESS;
        }
        else {
                msg_status = STATUS_MESSAGE_INCOMING;
                return;
        }
        waiting_to_send = FALSE;
        Protocol_statusReset();
        //A Little delay might be needed here
        return;
}

signed char Protocol_receive(union Payload* p)
{
        ENABLE_INTERRUPT;
        i = 0;
        sender_busy = 1;
        while (sender_busy == 1)
        {
                if (i++ < 3 * SENTINEL_TIME_OUT)
                {
                        Delay_us(U_DURATION_PER_BIT);
                }
                else
                {
                        Protocol_statusReset(); //In case of Noise(fake News)
                        Delay_ms(150);
                        return FAILED;
                }
        }

        sender_busy = 1;
        DISABLE_INTERRUPT;
        Delay_us(U_DURATION_PER_BIT);

        do
                header = Soft_UART_Read(&error);
        while (error);

        if (header == MSG_HEADER)
        {
                for (i = 0; i < 2; i++) {
                        do
                                p->buffer[i] = Soft_UART_Read(&error);
                        while (error);
                }
                ENABLE_SENDING;
                if (p->content.destAddress == myAddress)
                {
                        Protocol_doSentinelTimeout();  //clock the sender into TMR0 interrupt
                        Protocol_receiveHelper(p);
                        TOGGLE_RB5;
                }
                else
                {   //Message is meant for someone else
                        Protocol_dontSentinelTimeout();
                        Protocol_receiveHelper(p);
                        if (Protocol_find(p->content.destAddress) == SUCCESS) {
                                waiting_to_send = TRUE;
                        }
                        else {
                                Protocol_enlist(p->content.srcAddress);
                        }
                        return FAILED;
                }
                ENABLE_SENDING;
                Delay_us(500);
        }
        Protocol_statusReset();
        return SUCCESS;
}

signed char Protocol_idle(union Payload * p)
{
        return 0;
}

void Protocol_collisionGuard()
{
        for (i = 0;i < myAddress;++i)
                Delay_ms(1);
}

//sp1 sp0(0,0)=>OK
//sp1 sp0(0,1)=>Chaos_xxxxInit
//sp1 sp0(1,0)=>slaveRead<=>masterWrite
//sp1 sp0(1,1)=>slaveWrite<=>masterRead

void Chaos_slaveInit(int baud)
{
        sp0_dir = 0;
        sp1_dir = 0;
        CMCON = ONES;
        AppInterface_init(SLAVE, baud);
}

void Chaos_masterInit(int baud)
{
        sp0_dir = 1;
        sp1_dir = 1;
        CMCON = ONES;
        AppInterface_init(MASTER, baud);
}

void Choas_slaveSetOperation(char op) {
        switch (op) {
        case SlaveInit:
                sp0 = 1;
                sp1 = 0;
                break;
        case SlaveRead:
                sp0 = 0;
                sp1 = 1;
                break;
        case SlaveWrite:
                sp0 = 1;
                sp1 = 1;
                break;
        case SlaveNOP:
                sp0 = 0;
                sp1 = 0;
        }
}

void Choas_slaveSaveAddress(char addr) {
        myAddress = addr;
}

Chaos_fn Chaos_masterGetOperation() {
        if (SlaveNOP == PORTA & SlaveNOP) {
                return Chaos_idle;
        }
        else if (SlaveInit == PORTA & SlaveInit) {
                LoadInitPayload();
                return Chaos_Write;
        }
        else if (SlaveRead == PORTA & SlaveRead) {
                return Chaos_Write;
        }
        else {
                return Chaos_Read;
        }
}

void Choas_RFInit(unsigned baud) {
        Protocol_init(baud);
}

void Protocol_enlist(char addr)
{
        if (num_of_reg_nodes < MAX_NODES)
        {
                registered_nodes[num_of_reg_nodes] = addr;
                ++num_of_reg_nodes;
                return;
        }
}

signed char Protocol_find(char addr)
{
        for (i = 0;i < num_of_reg_nodes;++i)
        {
                if (registered_nodes[i] == addr)
                        return SUCCESS;
        }
        return FAILED;
}

void Protocol_receiveHelper(union Payload *p)
{
        ENABLE_RECEIVING;
        for (i = 0; i < BUFFER_SIZE - 2; i++) {
                do
                        p->content.data_[i] = Soft_UART_Read(&error);
                while (error);
        }
}

Protocol_fn Protocol_receiveListener()
{
        tmr0_ = TMR0;
        if (tmr0_ != SENTINEL)
        {
                msg_status = STATUS_MESSAGE_INCOMING;
                //wait for U_DURATION_PER_BIT
                //if there is no change in TMRO, its probably noise reset and return
                Delay_us(U_DURATION_PER_BIT);
                if (TMR0 == tmr0_) {
                        Protocol_statusReset();
                        return Protocol_idle;
                }
                return Protocol_receive;
        }
        return Protocol_idle;
}

Protocol_fn Protocol_sendListener()
{
        if (waiting_to_send == TRUE)
        {
                msg_status = STATUS_MESSAGE_OUTGOING;
                return Protocol_send;
        }
        return Protocol_idle;
}

char Chaos_Read(union Payload * p)
{
        AppInterface_readAppData(p);
        if(p->content.srcAddress == myAddress)
                waiting_to_send = TRUE;
        return IN;
}

char Chaos_Write(union Payload * p)
{
        if (p->content.destAddress == myAddress)
                AppInterface_writeAppData(p);
        return OUT;
}

char Chaos_idle(union Payload * p)
{
        return 0;
}

char Chaos_RFscan(union Payload *p)
{
        if (Protocol_sendListener()(p) == SUCCESS)
                return SENT;
        if (Protocol_receiveListener()(p) == SUCCESS)
                return RECEIVED;
}


void Protocol_init(unsigned baud)
{
        //Set TMR0 clock to external clock input
        // rising edge <--------- Important
        TRISB.F5 = 0;
        OPTION_REG = BIT5_ON | BIT3_ON | BIT2_ON | BIT1_ON | BIT0_ON;
        INTCON = BIT7_ON | BIT5_ON; // Enable interrupt TMR0 and Global Interrupts
        TMR0 = SENTINEL;
        error = Soft_UART_Init(rfPort, rx_pin, tx_pin, baud, 0);
        status_pin_dir = 0;
        return;
}