/*
 * DHT.c
 *
 * Created: 11/25/2020 6:30:19 PM
 *  Author: Mohaimen
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "lcd_lib.h"
#include "USART_Routines.h"
#include "FTOA.h"

#define DHT11_PIN  2
char tmpStr[20]; char hdtStr[20];
uint8_t c=0, I_RH, D_RH, I_Temp, D_Temp, CheckSum;
float tmp = 0.00, hdt = 0.00;

void Request()				//--- Micro-controller send start pulse/request
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);	//--- set to low pin
	_delay_ms(20);			    //--- wait for 20ms
	PORTD |= (1<<DHT11_PIN);	//--- set to high pin
}

void Response()				    //--- receive response from DHT11
{
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}

uint8_t Receive_data()							//--- receive data
{
	for (int q=0; q<8; q++)
	{
		while((PIND & (1<<DHT11_PIN)) == 0);	//--- check received bit 0 or 1
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))				//--- if high pulse is greater than 30ms
		c = (c<<1)|(0x01);						//--- then its logic HIGH
		else									//--- otherwise its logic LOW
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}

void testBlink ()
{
	
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);		//--- set to low pin
	_delay_ms(20);					//--- wait for 20ms
	PORTD |= (1<<DHT11_PIN);		//--- set to high pin
	DDRD &= ~(1<<DHT11_PIN);
	_delay_ms(2000);				//--- wait for 2000ms
}


void Read_DHT()
{	
	Request();					//--- send start pulse
	Response();					//--- receive response
	I_RH = Receive_data();		//--- store first eight bit in I_RH
	D_RH = Receive_data();		//--- store next eight bit in D_RH
	I_Temp = Receive_data();    //--- store next eight bit in I_Temp
	D_Temp = Receive_data();	//--- store next eight bit in D_Temp
	CheckSum = Receive_data();	//--- store next eight bit in CheckSum
	
	if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
	{
		transmitString("Data not valid"); TX_NEWLINE;
	}
	
	else
	{		
		tmp = I_Temp + (D_Temp/10);
		hdt = I_RH + (D_RH/10);
		
		ftoa(tmp, tmpStr, 2);
		transmitString_P(PSTR("Temperature = "));
		transmitString(tmpStr);
		transmitString_P(PSTR(" Deg"));
		
		ftoa(hdt, hdtStr, 2);
		transmitString_P(PSTR("  Humidity = "));
		transmitString(hdtStr); transmitString_P(PSTR(" %")); TX_NEWLINE;
	}
	_delay_ms(2000);	//--- This 1 second delay is must either in this function or in main loop
}