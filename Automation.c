/*
 * Automation.c
 *
 * Created: 11/26/2020 3:16:08 PM
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

//-- mois_sen_A1_pin = 1, mois_sen_A2_pin = 2, 
//-- mois_sen_B1_pin = 3, mois_sen_B2_pin = 4,  water_level_sen_pin = 7

#define VPV_SEN_PIN					5
#define VBAT_SEN_PIN				6
#define VBAT_Offset_val             0.0152	
#define VPV_Offset_val              0.0161	
	
#define water_level_sen_pin		    7
#define water_level_low_threshold   200
#define water_level_high_threshold  700
#define water_tank_full             950
#define average_moisture_low_val    300
#define average_moisture_high_val   850



void ADC_Init()
{
	ADMUX  |= 1 << REFS0;		//Reference is AVCC with external capacitor at AREF pin. Right adjust the result
	ADCSRA |= 1 << ADPS1;		//Division factor: 4, single conversion mode
	ADCSRA |= (1 << ADEN);		//Enable ADC
}

void Automation()
{
  read_field_moisture();
  Gen_valve_command();
  monitor_water_level();
  Gen_pump_command();
  VBAT_VPV();
}

void read_field_moisture()
{
	for(int mois_sen_pin = 1; mois_sen_pin < 5; mois_sen_pin++)
	{
		char mois_sen_pin_Str[10];
		imois_sen_val[mois_sen_pin] = readADCValue(mois_sen_pin);
		itoa(imois_sen_val[mois_sen_pin], moisStr, 10);
		transmitString("Moisture sensor_"); 
		itoa(mois_sen_pin, mois_sen_pin_Str, 10); transmitString(mois_sen_pin_Str);
		transmitString(" value = "); transmitString(moisStr);
		TX_NEWLINE;
	}
	
	{
	 iavg_moisture_FA = ((imois_sen_val[1] + imois_sen_val[2]) / 2);
	 itoa(iavg_moisture_FA, moisStr, 10);
	 transmitString("Average Moisture of Field_A = "); transmitString(moisStr);
	
	 iavg_moisture_FB = ((imois_sen_val[3] + imois_sen_val[4]) / 2);
	 itoa(iavg_moisture_FB, moisStr, 10);
	 transmitString("  Average Moisture of Field_B = "); transmitString(moisStr);
	 TX_NEWLINE;	
	}	
}

void Gen_valve_command()
{
	if(manual_overwrite == false)
	{
		if(iavg_moisture_FA <= average_moisture_low_val)
		  {valve_F1 = true; transmitString(" valve_F1 = true "); TX_NEWLINE;}		
		if(iavg_moisture_FA >= average_moisture_high_val)
		  {valve_F1 = false; transmitString(" valve_F1 = false "); TX_NEWLINE;}		
		if(iavg_moisture_FB <= average_moisture_low_val)
		  {valve_F2 = true; transmitString(" valve_F2 = true "); TX_NEWLINE;}		
		if(iavg_moisture_FB >= average_moisture_high_val)
		  {valve_F2 = false; transmitString(" valve_F2 = false "); TX_NEWLINE;}		
	}
	else {}	
}

void monitor_water_level()
{
	iwater_level_sen_val = readADCValue(water_level_sen_pin);
	itoa(iwater_level_sen_val, wtrStr, 10);
	transmitString("Water Level sensor value = "); transmitString(wtrStr); TX_NEWLINE;
	
	if(iwater_level_sen_val <= water_level_low_threshold)
	  {transmitString("Tank Water Level : "); transmitString("LOW"); TX_NEWLINE;}
	else if((iwater_level_sen_val > water_level_low_threshold) && (iwater_level_sen_val < water_level_high_threshold))
	  {transmitString("Tank Water Level : "); transmitString("MEDIUM"); TX_NEWLINE;}
	else if((iwater_level_sen_val >= water_level_high_threshold) && (iwater_level_sen_val < water_tank_full)) 
	  {transmitString("Tank Water Level : "); transmitString("HIGH"); TX_NEWLINE;}
	else if(iwater_level_sen_val >= water_tank_full)
	  {transmitString("Tank FULL"); TX_NEWLINE;}		  
}

void Gen_pump_command()
{
	if(manual_overwrite == false)
	{
		if(iwater_level_sen_val <= water_level_low_threshold)
		  {waterPump = true; transmitString(" waterPump = true "); TX_NEWLINE;} 
		if(iwater_level_sen_val >= water_tank_full)
		  {waterPump = false; transmitString(" waterPump = false "); TX_NEWLINE;}
	}
	else {}	
}

void VBAT_VPV()
{
	VBAT_SEN_VAL = readADCValue(VBAT_SEN_PIN);
	fbattery_voltage = (VBAT_SEN_VAL * VBAT_Offset_val);
	
	VPV_SEN_VAL = readADCValue(VPV_SEN_PIN);
	fpanel_voltage = (VPV_SEN_VAL * VPV_Offset_val);	
}

/*int MAP(int input, int input_start, int input_end, int output_start, int output_end)
{
	int map_val = ((input - input_start) * (output_end - output_start));
	map_val     = (int)(map_val / (input_end - input_start));
	map_val     = (map_val * output_start);
	return map_val;
}*/
