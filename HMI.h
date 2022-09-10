/*
 * HMI.h
 *
 * Created: 11/26/2020 10:01:48 PM
 *  Author: Mohaimen
 */ 


#ifndef HMI_H_
#define HMI_H_

char data; 
extern page_count;
extern uint8_t I_Temp; 
extern uint8_t D_Temp;
extern uint8_t I_RH; 
extern uint8_t D_RH;

#define overwrite_toggle_input	    PD3
#define valve_1_toggle_input	    PC1
#define valve_2_toggle_input	    PC2
#define W_pump_toggle_input	        PC3
#define water_level_sen_pin		    7
#define water_level_low_threshold   200
#define water_level_high_threshold  700
#define water_tank_full             950
#define average_moisture_low_val    300
#define average_moisture_high_val   850

void HMI();
void auto_manual_select();
void Button__Command();
void BT_Command();
void manual_valve_command();
void manual_pump_command();
void show_display();

#endif /* HMI_H_ */