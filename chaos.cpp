#include"chaos.h"


struct MessageInfo
{
	unsigned char command;
	unsigned char value;
	unsigned char extra;
}Rx, Tx, Fb;


void serial_out(char c);
char serial_in(void);
char chaos_UART_Read(char* error);
void chaos_UART_Write(char c);


void collision_guard();
void handle_broadcast();
void host_feedback();
void toggle_device_state();
void get_device_state();
void set_device_speed();
void get_device_speed();
char read_com_address();
void register_node();
void receive_helper(struct MessageInfo*);
void send_helper(struct MessageInfo*);
signed char on_my_list();
void host_feedback_impl();


void(*actions[7])() = {
	toggle_device_state,
	get_device_state,
	set_device_speed,
	get_device_speed,
	register_node,
	handle_broadcast,
	host_feedback
};


static unsigned char registered_nodes[MAX_NODES] = { 0x0e };
static char msg_status = STATUS_MESSAGE_NONE;
static unsigned char appliance_speed = 0;
static volatile char recipient_busy = 1;
static volatile char sender_busy = 1;
static char msg_type, reg_token = 0xff;
static volatile signed char result = FAILED;
static unsigned char my_address = 0x0e, recipient_address = 0x0e, host_address = 0x0e;
static char header, contact;
static char num_of_reg_nodes = 1;
static char waiting_to_send = FALSE;
static char waiting_to_feedback = FALSE;
static char error;
static int i = 0;



void status_reset(void)
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

void do_sentinel_timeout()
{
	tx_pin = 0;
	//clock TMR0 to overflow on the listening devices
	for (i = 0; i < SENTINEL_TIME_OUT; tx_pin = 0, ++i)
	{
		Delay_us(U_DURATION_PER_BIT);
		tx_pin = 1;
		Delay_us(U_DURATION_PER_BIT);
	}
}

void dont_sentinel_timeout()
{
	tx_pin = 0;
	//dont clock TMR0 to overflow on the listening devices
	for (i = 0; i < SENTINEL_TIME_OUT; tx_pin = 0, ++i)
	{
		Delay_us(U_DURATION_PER_BIT);
		tx_pin = 0;
		Delay_us(U_DURATION_PER_BIT);
	}
}

void receive_helper(struct MessageInfo* msg)
{
	do
		msg->command = chaos_UART_Read(&error);
	while (error);

	do
		msg->value = chaos_UART_Read(&error);
	while (error);

	do
		msg->extra = chaos_UART_Read(&error);
	while (error);

}

void chaos_receive()
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
			status_reset(); //In case of Noise(fake News)
			Delay_ms(150);
			return;
		}
	}

	sender_busy = 1;
	DISABLE_INTERRUPT;
	Delay_us(U_DURATION_PER_BIT);

	do
		header = chaos_UART_Read(&error);
	while (error);

	if (header == MSG_HEADER)
	{
		do
			contact = chaos_UART_Read(&error);
		while (error);

		if (contact == my_address)
		{
			ENABLE_SENDING;
			do_sentinel_timeout();  //clock the sender into TMR0 interrupt
			ENABLE_RECEIVING;
			receive_helper(&Rx);
			(*actions[Rx.command])();
			ENABLE_SENDING;
			Delay_us(500);
			chaos_UART_Write(result);
		}
		else if (contact == ALL_ACCESS)
		{
			receive_helper(&Tx);
			(*actions[HANDLE_BROADCAST])();
		}
		else
		{   //Message is meant for someone else
			dont_sentinel_timeout();
			ENABLE_RECEIVING;
			receive_helper(&Rx);
			do
				result = chaos_UART_Read(&error);
			while (error);

		}
	}
	status_reset();
	return;
}

void send_helper(struct MessageInfo* p)
{
	Delay_us(U_DURATION_PER_BIT);
	chaos_UART_Write(p->command);
	Delay_us(U_DURATION_PER_BIT);
	chaos_UART_Write(p->value);
	Delay_us(U_DURATION_PER_BIT);
	chaos_UART_Write(p->extra);
}

void send_preamble()
{
	ENABLE_SENDING;
	do_sentinel_timeout();
	//To ensure that interrupt mode is entered in
	// the recipient***************
	Delay_us(U_DURATION_PER_BIT); // delta T
	Delay_us(U_DURATION_PER_BIT); // delta T

	Delay_us(U_DURATION_PER_BIT);
	chaos_UART_Write(MSG_HEADER);
	Delay_us(U_DURATION_PER_BIT);
	Delay_us(U_DURATION_PER_BIT);
}

signed char direct_send(struct MessageInfo* p)
{
	chaos_UART_Write(recipient_address);
	TMR0 = SENTINEL;
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
		{     //Someone is not home
			  //Delay_ms(150);
			status_reset();
			return FAILED;
		}
	}
	recipient_busy = 1;
	DISABLE_INTERRUPT;
	ENABLE_SENDING;
	send_helper(p);
	return SUCCESS;
}

void collision_guard()
{
	for (i = 0;i<reg_token;++i)
		Delay_ms(1);
}

void chaos_send()
{
	collision_guard();
	//TMR0 might not be equal to SENTINEL due to noise re-write this
	//instead wait for a short time, if there is no change in SENTINEL
	//Initiate Sending and introduce TOLERANCE to SENTINEL
	if (TMR0 == SENTINEL)//No device is transmitting
	{
		send_preamble();

		if (msg_type == DIRECT)
		{
			if (direct_send(&Tx) != SUCCESS)
			{
				status_reset();
				return;
			}
			ENABLE_RECEIVING;

			do
				result = chaos_UART_Read(&error);
			while (error);

			if ((Tx.command == GET_DEVICE_STATE) ||
				(Tx.command == GET_DEVICE_SPEED))
			{
				//Setup Host Feedback
				Fb.value = recipient_address;
				Fb.extra = result;
				waiting_to_feedback = TRUE;
			}
		}
		else if (msg_type == BROADCAST)
		{
			chaos_UART_Write(ALL_ACCESS);
			send_helper(&Tx);
		}
	}
	else
	{
		msg_status = STATUS_MESSAGE_INCOMING;
		return;
	}
	waiting_to_send = FALSE;
	status_reset();
	//A Little delay might be needed here
	return;
}

void chaos_receive_listener()
{
	if (TMR0 != SENTINEL)
	{
		msg_status = STATUS_MESSAGE_INCOMING;
		//wait for U_DURATION_PER_BIT
		//if there is no change in TMRO then its noise reset and return
		chaos_receive();
	}
}

void chaos_send_listener()
{
	if (waiting_to_send == TRUE)
	{
		msg_status = STATUS_MESSAGE_OUTGOING;
		chaos_send();
	}
}

void chaos_feedback_listener()
{
	if (waiting_to_feedback == TRUE)
	{
		msg_status = STATUS_MESSAGE_OUTGOING;
		host_feedback_impl();
	}
}

void toggle_device_state()
{
	toggle_pin = (toggle_pin == 0) ? 1 : 0;
	result = SUCCESS;
}

void get_device_state()
{
	result = toggle_pin;
}

void set_device_speed()
{
	appliance_speed = Rx.extra;
	switch (appliance_speed)
	{
	case 0:
		speed_bit0_pin = 0;
		speed_bit1_pin = 0;
		speed_bit2_pin = 0;
		break;
	case 1:
		speed_bit0_pin = 1;
		speed_bit1_pin = 0;
		speed_bit2_pin = 0;
		break;
	case 2:
		speed_bit0_pin = 0;
		speed_bit1_pin = 1;
		speed_bit2_pin = 0;
		break;
	case 3:
		speed_bit0_pin = 1;
		speed_bit1_pin = 1;
		speed_bit2_pin = 0;
		break;
	case 4:
		speed_bit0_pin = 0;
		speed_bit1_pin = 0;
		speed_bit2_pin = 1;
		break;
	case 5:
		speed_bit0_pin = 1;
		speed_bit1_pin = 0;
		speed_bit2_pin = 1;
		break;
	case 6:
		speed_bit0_pin = 0;
		speed_bit1_pin = 1;
		speed_bit2_pin = 1;
		break;
	case 7:
		speed_bit0_pin = 1;
		speed_bit1_pin = 1;
		speed_bit2_pin = 1;
		break;
	default:
		speed_bit0_pin = 0;
		speed_bit1_pin = 0;
		speed_bit2_pin = 0;
		break;
	}
}

void get_device_speed()
{
	result = appliance_speed;
}

void host_feedback()
{
	Fb.value = Rx.value;
	Fb.extra = Rx.extra;
	waiting_to_feedback = TRUE;
	result = SUCCESS;
}

void host_feedback_impl()
{
	recipient_address = host_address;

	//TMR0 might not be equal to SENTINEL due to noise re-write this
	//instead wait for a short time, if there is no change in SENTINEL
	//Initiate Sending and introduce TOLERANCE to SENTINEL
	if (TMR0 == SENTINEL)//No device is transmitting
	{

		send_preamble();
		if (direct_send(&Fb) != SUCCESS)
		{
			status_reset();
			return;
		}
		ENABLE_RECEIVING;

		do
			result = chaos_UART_Read(&error);
		while (error);
		status_reset();
		waiting_to_feedback = FALSE;
	}
}

void handle_broadcast()
{
	recipient_address = Tx.value;
	waiting_to_send = TRUE;
	msg_type = on_my_list();
	result = SUCCESS;
}

void register_node()
{
	if (num_of_reg_nodes < MAX_NODES)
	{
		registered_nodes[num_of_reg_nodes] = Rx.value;
		result = ++num_of_reg_nodes;
		return;
	}
	result = FAILED;
}

signed char chaos_Register_me()
{
	recipient_address = host_address;
	Tx.command = REGISTER_ME;
	Tx.value = my_address;
	Tx.extra = 0x07;
	waiting_to_send = TRUE;
	msg_status = STATUS_MESSAGE_OUTGOING;
	msg_type = DIRECT;
	chaos_send();

	return result;
}

signed char on_my_list()
{
	for (i = 0;i < num_of_reg_nodes;++i)
	{
		if (registered_nodes[i] == recipient_address)
			return DIRECT;
	}
	return BROADCAST;
}


void chaos_UART_Init(unsigned char my_add, unsigned char host_add)
{
	//Set TMR0 clock to external clock input
	// rising edge <--------- Important
	OPTION_REG = BIT5_ON | BIT3_ON | BIT2_ON | BIT0_ON | BIT0_ON;
	INTCON = BIT7_ON | BIT5_ON; // Enable interrupt TMR0 and Global Interrupts
	TMR0 = SENTINEL;
	my_address = my_add;
	host_address = host_add;
	Fb.command = HOST_FEEDBACK;
	toggle_pin = 0;
	speed_bit0_pin = 0;
	speed_bit1_pin = 0;
	speed_bit2_pin = 0;
}


void chaos_Post(unsigned char msg, unsigned char extra, unsigned char recipient)
{
	if (msg > GET_DEVICE_SPEED)
		return;

	waiting_to_send = TRUE;
	Tx.command = msg;
	Tx.extra = extra;
	Tx.value = recipient;
	recipient_address = recipient;
	msg_type = on_my_list();
}

void serial_out(char c)
{
	char j;
	Delay_us(SERIAL_OUT_DELAY);

	//Start
	tx_pin = 1;
	for (j = 0; j < SERIAL_OUT_LOOP; tx_pin = 1, ++j)
	{
		Delay_us(U_DURATION_PER_BIT);
		if (j < c)
		{
			tx_pin = 1;
		}
		Delay_us(U_DURATION_PER_BIT);
	}

	tx_pin = 0;
	for (j = 0; j < SERIAL_OUT_LOOP; tx_pin = 0, ++j)
	{
		Delay_us(U_DURATION_PER_BIT);
		if (j < c)
		{
			tx_pin = 1;
		}
		Delay_us(U_DURATION_PER_BIT);
	}

}

char serial_in(void)
{
	char count = 0, curLevel = 0;
	char duration = 0;

	SET_CLOCK_SRC_TMR0_INTERNAL;
	APPLY_PRESCALER_TO_TMR0;
	Delay_us(SERIAL_IN_DELAY);

	while (rx_pin == 0)continue;

	TMR0 = 0;
	while (rx_pin == 1)continue;
	duration = TMR0;

	TMR0 = 0;
	do
	{
		if (curLevel != rx_pin)
		{
			if (rx_pin == 1)
			{
				++count;
			}
			curLevel = rx_pin;
		}

	} while (TMR0 < duration);

	SET_CLOCK_SRC_TMR0_EXTERNAL;
	APPLY_PRESCALER_TO_WDT;
	return count;
}

void chaos_UART_Write(char c)
{
	char j;
	tx_pin = 0;
	for (j = 0;j < BYTE;++j)
	{
		if (c & (BIT7_ON >> j))
		{
			serial_out(SERIAL_OUT_ONE);
		}
		else
		{
			serial_out(SERIAL_OUT_ZERO);
		}

	}
}

char chaos_UART_Read(char* error)
{
	char j, c = 0;
	for (j = 0;j < BYTE;++j)
	{
		if (serial_in() >= SERIAL_ONE_THRESHOLD)
			c += (BIT7_ON >> j);
	}
	*error = 0;
	return c;
}