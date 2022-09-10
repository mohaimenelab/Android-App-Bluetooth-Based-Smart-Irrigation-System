#ifndef _UART_ROUTINES_H_

	#define _UART_ROUTINES_H_

	#define F_CPU 8000000UL
	
	#include <avr/io.h>
	#include <avr/pgmspace.h>

	#define USART_BAUDRATE 9600
	#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

	#define TX_NEWLINE {transmitByte(0x0D); transmitByte(0x0A);}
	#define CHAR 0
	#define INT  1
	#define LONG 2

	void USART_Init(void);
	void transmitByte(unsigned char);
	void transmitString_P(const char*);
	void transmitString(char*);
	unsigned char receiveByte(void);
	void transmitHex(unsigned char dataType, unsigned long data);

#endif