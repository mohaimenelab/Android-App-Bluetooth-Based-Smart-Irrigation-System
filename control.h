/*
 * control.h
 *
 * Created: 11/26/2020 6:55:40 PM
 *  Author: Mohaimen
 */ 


#ifndef CONTROL_H_
#define CONTROL_H_


#define   W_pump_pin     PB0
#define   valve_F1_pin   PB1
#define   valve_F2_pin   PB2

void system_control();
void pump_control();
void valve_control();
void MOSFET_CONTROL();

extern bool waterPump;
extern bool valve_F1, valve_F2;

#endif /* CONTROL_H_ */