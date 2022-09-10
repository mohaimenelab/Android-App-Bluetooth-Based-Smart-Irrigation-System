/*
 * auto.h
 *
 * Created: 11/26/2020 3:19:31 PM
 *  Author: pc
 */ 


#ifndef AUTO_H_
#define AUTO_H_

extern bool manual_overwrite;
extern bool valve_F1, valve_F2;
extern bool waterPump;

extern int iwater_level_sen_val;
extern int imois_sen_val[7];
extern int iavg_moisture_FA, iavg_moisture_FB;
extern char wtrStr[10]; extern char moisStr[10];

extern int VPV_SEN_VAL;
extern int VBAT_SEN_VAL;

float fpanel_voltage;
float fbattery_voltage;

void ADC_Init();
void Automation();
void read_field_moisture();
void Gen_valve_command();
void monitor_water_level();
void Gen_pump_command();
void VBAT_VPV();

#endif /* AUTO_H_ */