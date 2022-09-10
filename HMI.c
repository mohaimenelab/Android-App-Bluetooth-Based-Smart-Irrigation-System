/*
 * HMI.c
 *
 * Created: 11/26/2020 10:01:26 PM
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
#include "FTOA.h"		//--- float to string conversion >> float_to_string.c
#include "auto.h"		//--- Automation				 >> Automation.c
#include "control.h"	//--- system control			 >> control.c
#include "HMI.h"

char data;
void page_one();
void page_two();
void page_three(); 
void wtr_lvl_cndn(); 
void wtr_pump_cndn();
void page_four();
void valve_status_cndn();
void page_five();

void HMI()
{	
	auto_manual_select();
	manual_valve_command();
	manual_pump_command();
	BT_Command();
	show_display();
}

void auto_manual_select()
{
	Button__Command();
	BT_Command();
}

void Button__Command()
{
  if(PIND & (1 << overwrite_toggle_input))
  {
	  manual_overwrite = !manual_overwrite;
	  while(PIND & (1 << overwrite_toggle_input))
	  _delay_ms(20);
  }	
}

void manual_valve_command()
{
	if(manual_overwrite == true)
	{
		if(PINC & (1 << valve_1_toggle_input))
		{
		   valve_F1 = !valve_F1;
		   while(PINC & (1 << valve_1_toggle_input))
		   _delay_ms(20);
	    }
			 
		if(PINC & (1 << valve_2_toggle_input))
		{
		   valve_F2 = !valve_F2;
		   while(PINC & (1 << valve_2_toggle_input))
		   _delay_ms(20);
		}
	}
}

void manual_pump_command()
{
  if(manual_overwrite == true)
  {
	if(PINC & (1 << W_pump_toggle_input))
	{
	  waterPump = !waterPump;	
	  while(PINC & (1 << W_pump_toggle_input))
	  _delay_ms(20);	 
    }
  }
}
   
void BT_Command()
{	
  data = receiveByte();  
	  	  
  switch (data)
  { 
	case 'A':
		 {if(manual_overwrite == true)
			 {valve_F1 = !valve_F1; data = ' '; TX_NEWLINE;}
		  else
		     {valve_F1 = valve_F1; data = ' '; TX_NEWLINE;}		 
	     }
	  break;
	  
	case 'B':
		 {if(manual_overwrite == true)
			 {valve_F2 = !valve_F2; data = ' '; TX_NEWLINE;}
		  else
		     {valve_F2 = valve_F2; data = ' '; TX_NEWLINE;}
		 }
	  break;
	  
	case 'P':
		 {if(manual_overwrite == true)
			 {waterPump = !waterPump; data = ' '; TX_NEWLINE;}
		  else
		     {waterPump = waterPump; data = ' '; TX_NEWLINE;}
		 }
	  break;
	  	
	case 'O':
		 {manual_overwrite = !manual_overwrite; data = ' '; TX_NEWLINE;}
	  break;
	  
	case '?':
	     {data = ' '; TX_NEWLINE;}
	  break; 
	   	 
	default :
	     {data = ' '; TX_NEWLINE;}
	  break; 	 
  }
}

void show_display()
{
  if(page_count <= 3)
   {page_one();}	
			
  if(page_count > 3 && page_count <= 6)
   {page_two();}	
			
  if(page_count > 6 && page_count <= 9)
   {page_three();}	
			
  if(page_count > 9 && page_count <= 12)
   {page_four();}
	   
  if(page_count > 12 && page_count <= 15)
   {page_five();}
}

void page_one()
{
  LCDWriteStringXY(0, 0,"Temp: "); LCDWriteIntXY(6, 0, I_Temp, 2); 
  LCDWriteStringXY(8, 0,"."); LCDWriteIntXY(9, 0, D_Temp, 2); LCDWriteStringXY(11, 0," Deg ");  
  LCDWriteStringXY(0, 1,"Hmdt: "); LCDWriteIntXY(6, 1, I_RH, 2);
  LCDWriteStringXY(8, 1,"."); LCDWriteIntXY(9, 1, D_RH, 2); LCDWriteStringXY(11, 1," %   ");
}

void page_two()
{
  LCDWriteStringXY(0, 0,"A1:"); LCDWriteIntXY(3, 0, imois_sen_val[1], 4);
  LCDWriteStringXY(7, 0," A2:"); LCDWriteIntXY(11, 0, imois_sen_val[2], 4);
  LCDWriteStringXY(0, 1,"B1:"); LCDWriteIntXY(3, 1, imois_sen_val[3], 4);
  LCDWriteStringXY(7, 1," B2:"); LCDWriteIntXY(11, 1, imois_sen_val[4], 4);
  LCDWriteStringXY(15, 1," ");
}

void page_three()
{
 LCDWriteStringXY(0, 0,"Water Lvl : "); 
 wtr_lvl_cndn();
 LCDWriteStringXY(0, 1,"Water Pump: ");
 wtr_pump_cndn();
}

void wtr_lvl_cndn()
{
	if(iwater_level_sen_val <= water_level_low_threshold)
	 {LCDWriteStringXY(12, 0,"LOW ");}
	 
	if((iwater_level_sen_val > water_level_low_threshold) && (iwater_level_sen_val < water_level_high_threshold))
	 {LCDWriteStringXY(12, 0,"MED ");}
	 
	if((iwater_level_sen_val >= water_level_high_threshold) && (iwater_level_sen_val < water_tank_full))
	 {LCDWriteStringXY(12, 0,"HIGH")};
	 
	if(iwater_level_sen_val >= water_tank_full)
	 {LCDWriteStringXY(12, 0,"FULL");}	
}

void wtr_pump_cndn()
{
 if(waterPump == false)
  {LCDWriteStringXY(12, 1,"OFF ");}
	 
 if(waterPump == true)
  {LCDWriteStringXY(12, 1,"ON  ");}	
}

void page_four()
{
 LCDWriteStringXY(0, 0,"Valve_F_A : ");
 LCDWriteStringXY(0, 1,"Valve_F_B : ");
 valve_status_cndn();
}

void valve_status_cndn()
{
 if(valve_F1 == false)
  {LCDWriteStringXY(12, 0,"OFF ");}
	
 if(valve_F1 == true)
  {LCDWriteStringXY(12, 0,"ON  ");}
	
 if(valve_F2 == false)
  {LCDWriteStringXY(12, 1,"OFF ");}
	
 if(valve_F2 == true)
  {LCDWriteStringXY(12, 1,"ON  ");}
}

void page_five()
{
 int vbat   = fbattery_voltage * 100;
 int dc_vbat = (vbat % 100);
 
 int vpv = fpanel_voltage * 100;
 int dc_vpv = (vpv % 100);
 
 LCDWriteStringXY(0, 0,"V_BAT: ");
 LCDWriteIntXY(7, 0, fbattery_voltage, 2);
 LCDWriteStringXY(9, 0,".");
 LCDWriteIntXY(10, 0, dc_vbat, 2);
 LCDWriteStringXY(12, 0," vlt");
 
 LCDWriteStringXY(0, 1,"V_PV : ");
 LCDWriteIntXY(7, 1, fpanel_voltage, 2);
 LCDWriteStringXY(9, 1,".");
 LCDWriteIntXY(10, 1, dc_vpv, 2);
 LCDWriteStringXY(12, 1," vlt");
}