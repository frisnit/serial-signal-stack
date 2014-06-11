// RS232 serial driver
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include "globals.h"
#include <util/delay.h>
#include <avr/pgmspace.h>

#define BAUD 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)

#define RX_BUFFER_LEN	5

unsigned char data[RX_BUFFER_LEN];
uint8_t buffer_ptr=0;

// transmit a byte via the UART
void tx_byte(unsigned char data)
{
	// wait for tx buffer to clear
	while(!(UCSRA & (1<<UDRE)));

	// put data in tx buffer
	UDR = data;
}

// send 'OK'
void tx_ok(void)
{
	tx_byte('O');
	tx_byte('K');
	tx_byte('\n');
}

// send 'ERROR'
void tx_error(void)
{
	tx_byte('E');
	tx_byte('R');
	tx_byte('R');
	tx_byte('O');
	tx_byte('R');
	tx_byte('\n');
}

// get a byte from the UART if one is waiting
void rx_byte(void)
{
	uint8_t value=STATE_OFF;
	unsigned char byte;

	// see if there's data waiting for us
	if(!(UCSRA & (1<<RXC)))
		return;

	// get the data from the receive buffer
	byte = UDR;
	
	// and put it in our RX buffer
	data[buffer_ptr]=byte;
	buffer_ptr++;
	buffer_ptr%=RX_BUFFER_LEN;// circular buffer
	
	// trigger a command when a '\n' is received
	if(byte=='\n')
	{
		// might be CRLF or just LF
		if(buffer_ptr>=3)
		{
			// look for the 'state' character
			if(data[1]=='0')
				value=STATE_OFF;
			else if(data[1]=='1')
				value=STATE_ON;
			else if(data[1]=='p')
				value=STATE_PULSE;
			else if(data[1]=='f')
				value=STATE_FLASH;
			else
				value=STATE_INVALID;
			
			// determine which output the state is referring to
			if(value!=STATE_INVALID)
			{
				if(data[0]=='r')
				{
					red_mode=value;
					tx_ok();
				}
				else if(data[0]=='o')
				{
					orange_mode=value;
					tx_ok();
				}
				else if(data[0]=='g')
				{
					green_mode=value;
					tx_ok();
				}
				else
				{
					tx_error();
				}
			}
			else
			{
				tx_error();
			}
		}
		else
		{
			tx_error();
		}
		
		// reset buffer pointer
		buffer_ptr=0;
	}
}

// send a string of characters
void tx_data(const unsigned char *data, unsigned char len)
{
	int n;

	for(n=0;n<len;n++)
	{
		tx_byte(pgm_read_byte(&data[n]));
	}
}

// initialise the UART
void init_usart()
{
	// could make this dynamic if necessary
	uint8_t baud = BAUD_PRESCALE;
	
	// set baud rate
	UBRRH = (unsigned char) (baud>>8);
	UBRRL = (unsigned char) baud;

	UCSRC = 0;

	// enable tx/rx
	UCSRB = (1<<RXEN)|(1<<TXEN);

	// set frame format (8 data, 1 stop, no parity)
	UCSRC = (1<<UCSZ1)|(1<<UCSZ0);
}


