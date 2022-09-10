/*
 * 2020_12_01_Smart_Irrigation_V1.0.05.c
 *
 * Created: 11/30/2020 3:36:37 PM
 * Author : Mohaimen
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "lcd_lib.h"
#include "USART_Routines.h"
#include "DHT.h"
#include "FTOA.h"		//--- float to string conversion >> float_to_string.c
#include "auto.h"		//--- Automation				 >> Automation.c
#include "control.h"	//--- system control			 >> control.c
#include "HMI.h"

#define PWM_PIN		    PB3  
#define set_duty_95_P 	245
#define set_duty_10_P	27
#define DHT_call_time	30

bool manual_overwrite = false;	//--- this flag decide >> system run at manually or auto, and this flag control by pressing any button
bool valve_F1 = false, valve_F2 = false;
bool waterPump = false;

int counter = 0, page_count = 0; 
int VPV_SEN_VAL  = 0;
int VBAT_SEN_VAL = 0;
int iwater_level_sen_val = 0;
int imois_sen_val[7];
int iavg_moisture_FA = 0, iavg_moisture_FB = 0;

float fpanel_voltage = 0.00;
float fbattery_voltage = 0.00;

char wtrStr[10]; char moisStr[10];

void Fixed_Operation();

int main(void)
{			
	DDRB |= (1 << PWM_PIN);                  DDRB |= (1 << W_pump_pin);	
	DDRB |= (1 << valve_F1_pin);             DDRB |= (1 << valve_F2_pin);
	
	DDRC &= (~(1 << valve_1_toggle_input));  DDRC &= (~(1 << valve_2_toggle_input));
	DDRC &= (~(1 << W_pump_toggle_input));   DDRD &= (~(1 << overwrite_toggle_input));
	
	PORTB &= (~(1 << W_pump_pin));  PORTB &= (~(1 << valve_F1_pin));  PORTB &= (~(1 << valve_F2_pin));
				
	ADC_Init();	LCDinit(); LCDclr(); USART_Init();
	LCDWriteString("   iPAGE Demo   ");
	transmitString("Smart Irrigation System"); TX_NEWLINE;	
	
	OCR0 = set_duty_10_P;	//--- initial duty cycle 10%
	TCCR0 |= (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS00) | (1 << CS02);
	
	TCCR1B |= (1 << CS12);	//Timer1, Normal mode, 256 prescaler
	TIMSK |= (1 << TOIE1);	//Enable Timer1 overflow interrupt
	TCNT1 = 34285;			//Input value to TCNT1
	sei();					//Enable Global interrupt
	transmitString_P(PSTR("Timer configured for 1-second interrupt...")); TX_NEWLINE;		
		
	testBlink (); //--- Used to skip multiple pulse while sending request signal to DHT
	LCDclr();
	
    while(1)
	{			
		TX_NEWLINE; transmitString("---In main loop---"); TX_NEWLINE; TX_NEWLINE;	
		
	    Fixed_Operation();
		Automation();	
		system_control();
		HMI();
	}
}

void Fixed_Operation()
{	
	if(counter >= DHT_call_time)
	{
		Read_DHT();
		counter = 0;
	}
}

int readADCValue(unsigned char channel)		//Read ADC value from channel
{
	unsigned int ADCH_temp = 0, ADC_temp = 0;
	
	ADMUX = 0b01000000 | channel;			 //Select channel
	ADCSRA |= (1 << ADSC);					 //Start conversion
	
	while(!(ADCSRA & (1 << ADIF)));			 //wait for conversion done
	
	ADC_temp = ADCL;						 //read out ADCL register
	ADCH_temp = ADCH;						 //read out ADCH register
	ADC_temp = ADC_temp + (ADCH_temp << 8);	 //Merge 10 bit result
	
	return ADC_temp;
}

ISR(TIMER1_OVF_vect)		//ISR for Timer1 overflow
{
	counter++;	
	
	page_count++;
	if(page_count > 15)
	{
	  page_count = 0;
	}
	
	TCNT1 = 34285;			//Input value to TCNT1
	TIFR |= 1 << TOV1;		//Clear Timer1 Overflow Flag
}
