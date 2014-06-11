#define F_CPU 12000000UL  /* 12 MHz External Oscillator */
#define	NULL	0

#define STATE_OFF		0x00
#define STATE_ON		0x01
#define STATE_FLASH		0x02
#define STATE_PULSE		0x03
#define STATE_INVALID	0x04

#define RED		_BV(PB4)
#define ORANGE	_BV(PB3)
#define GREEN	_BV(PB2)

#define LED _BV(PD5)

#define ENABLE(port,line)	port |= line
#define DISABLE(port,line)	port &= ~line

extern uint8_t red_mode;
extern uint8_t orange_mode;
extern uint8_t green_mode;
