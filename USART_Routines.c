#include "USART_Routines.h"

//************************************************
//        Function to initialize USART
//************************************************
void USART_Init()
{
	UCSRB = (1 << RXEN)   | (1 << TXEN );						//Turn on the transmission and reception circuitry
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0 );	// Use 8- bit character sizes
	
	UBRRH = (BAUD_PRESCALE >> 8);							//Load upper 8- bits of the baud rate value into the high byte of the UBRR register
	UBRRL = BAUD_PRESCALE;									//Load lower 8 - bits of the baud rate value into the low byte of the UBRR register	
}

//************************************************
//     Function to receive a single byte
//************************************************
unsigned char receiveByte(void)
{
	char data;
	
	//while((UCSRA & (1 << RXC)) == 0); //Do nothing until data have been received and is ready to be read from UDR
	if((UCSRA & (1 << RXC)) == 0); //Do nothing until data have been received and is ready to be read from UDR
	{
	 data = UDR;
	}
		
	return (data);
}

//***************************************************
//       Function to transmit a single byte
//***************************************************
void transmitByte(unsigned char data)
{
	while((UCSRA & (1 << UDRE)) == 0); //Do nothing until UDR is ready for more data to be written to it
	
	UDR = data;
}

//***************************************************
//     Function to transmit a string in Flash
//***************************************************
void transmitString_P(const char* string)
{
	while(pgm_read_byte( & (*string)))
	{
		transmitByte(pgm_read_byte(&(*string++)));
	}
}

//***************************************************
//      Function to transmit a string in RAM
//***************************************************
void transmitString(char* string)
{
	while(*string)
	{
		transmitByte(*string++);
	}
}

//***************************************************
//Function to transmit hex format data.
//First argument indicates type: CHAR, INT or LONG. 
//Second argument is the data to be displayed
//***************************************************
void transmitHex(unsigned char dataType, unsigned long data) 
{
	unsigned char count, i, temp;
	unsigned char dataString[] = "0x        ";
	
	if(dataType == CHAR) count = 2;
	if(dataType == INT) count = 4;
	if(dataType == LONG) count = 8;
	
	for(i = count; i > 0; i--)
	{
		temp = data % 16;
		
		if((temp >= 0) && (temp < 10)) dataString [i + 1] = temp + 0x30;
		else dataString [i + 1] = (temp - 10) + 0x41;
		
		data = data / 16;
	}
	
	transmitString(dataString);
}