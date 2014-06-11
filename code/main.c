#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#include "globals.h"
#include <util/delay.h>

#include "serial.h"

uint8_t red_mode;
uint8_t orange_mode;
uint8_t green_mode;

uint8_t toggle=0;
int16_t pulse_state=1;
int16_t pulse_level=0;

uint16_t timer_count=0;

void initialise()
{
	cli();

	// enable timer 0 overflow interrupt
	TIMSK|=(1<<TOIE0);

	// setup PWM
	TCCR0A = _BV(COM0A1) | _BV(WGM00);// PWM mode
	OCR0A  = 0x00;
	TCCR0B = _BV(CS00);   // PWM clock source
	
	OCR1A = 0x00; 
	OCR1B = 0x00; 

	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM10); 
	TCCR1B = (1<<CS10);

}

void set_brightness(int brightness, int channel)
{
	if(brightness<0x00 || brightness>0xff)
		return;
		
	// set PWM level
	if(channel==2)
		OCR0A = brightness;
	if(channel==1)
		OCR1A = brightness;
	if(channel==0)
		OCR1B = brightness;
}

void set_state(uint8_t state, uint8_t channel)
{
	uint8_t pwm_register;
	
	if(state==STATE_OFF)
		pwm_register=0x00;
	
	if(state==STATE_ON)
		pwm_register=0xff;
	
	if(state==STATE_FLASH)
		pwm_register=toggle?0xff:0x00;
	
	if(state==STATE_PULSE)
		pwm_register=pulse_level>>7;

	set_brightness(pwm_register, channel);
}

// timer interrupt called every few hundred microseconds
ISR(TIMER0_OVF_vect)
{
	rx_byte();// TODO: don't poll the UART, use the UART interrupt

	set_state(red_mode,0);
	set_state(orange_mode,1);
	set_state(green_mode,2);

	// manage pulsing effect
	pulse_level+=pulse_state;
	if(pulse_level>32700 || pulse_level<0)
		pulse_state=-pulse_state;

	// flash status LED
	timer_count++;
	if(timer_count>18000)
	{
		timer_count=0;
		toggle=~toggle;
		
		if(toggle)
			ENABLE(PORTD,LED);
		else
			DISABLE(PORTD,LED);
	}
}

int main (void)
{	
	wdt_disable();

	// set up i/o pins

    // PD3 is status LED
    DDRD = LED;
    DDRB = RED | GREEN | ORANGE;

    initialise();

	// initialise USART
	init_usart();

	//lamp test
	set_state(STATE_ON,2);
	_delay_ms(500);

	set_state(STATE_OFF,2);
	set_state(STATE_ON,1);
	_delay_ms(500);

	set_state(STATE_OFF,1);
	set_state(STATE_ON,0);
	_delay_ms(500);

	set_state(STATE_OFF,0);

	sei();	

    /* loop forever - everything is handled in interrupts */
    while(1);
}


