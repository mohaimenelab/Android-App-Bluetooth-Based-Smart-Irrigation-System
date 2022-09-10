/*
 * control.c
 *
 * Created: 11/26/2020 6:56:28 PM
 *  Author: Mohaimen
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>
#include "lcd_lib.h"
#include "USART_Routines.h"
#include "DHT.h"
#include "FTOA.h"
#include "auto.h"
#include "control.h"

#define LOW_VBAT                    10
#define HIGH_VBAT                   13.5
#define set_duty_95_P 				245
#define set_duty_10_P				27

void system_control()
{
	pump_control();
	valve_control();
	MOSFET_CONTROL();
}

void pump_control()
{
	if(waterPump == false)
		{PORTB &= (~(1 << W_pump_pin)); transmitString("Water Pump state : OFF"); TX_NEWLINE;}
		 
	if(waterPump == true)
		{PORTB |= (1 << W_pump_pin); transmitString("Water Pump state : ON"); TX_NEWLINE;}
}

void valve_control()
{
	if(valve_F1 == false)
		{PORTB &= (~(1 << valve_F1_pin)); transmitString("valve_1 state : OFF"); TX_NEWLINE;}
	
	if(valve_F1 == true)
		{PORTB |= (1 << valve_F1_pin); transmitString("valve_1 state : ON"); TX_NEWLINE;}
		
    if(valve_F2 == false)
		{PORTB &= (~(1 << valve_F2_pin)); transmitString("valve_2 state : OFF"); TX_NEWLINE;}
    
    if(valve_F2 == true)
		{PORTB |= (1 << valve_F2_pin); transmitString("valve_2 state : ON"); TX_NEWLINE;}
}

void MOSFET_CONTROL()
{
	if(fbattery_voltage <= LOW_VBAT)
	{
		if(fpanel_voltage > fbattery_voltage)
		{
			OCR0 = set_duty_95_P;
		}
		else{OCR0 = 0;}
	}
		
	else if((fbattery_voltage > LOW_VBAT)  && (fbattery_voltage < HIGH_VBAT))
	{
	    if(fpanel_voltage > fbattery_voltage)
		{
		  OCR0 = set_duty_95_P;
		} 
		else {OCR0 = 0;}
	}
			
	else if(fbattery_voltage >= HIGH_VBAT)
	{
		if(fpanel_voltage > fbattery_voltage)
		 {OCR0 = set_duty_10_P;}
	}
			
	else if(fpanel_voltage < fbattery_voltage)
	  {OCR0 = 0;}
}