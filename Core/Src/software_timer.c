/*
 * software_timer.c
 *
 *  Created on: Sep 24, 2023
 *      Author: HaHuyen
 */

#include "software_timer.h"

#define TIMER_CYCLE_2 1


uint16_t flag_timer2 = 0;
uint16_t flag_timer3 = 0;
uint16_t flag_timer_MOD = 0;
uint16_t flag_timer_alarm = 0;

uint16_t timer2_counter = 0;
uint16_t timer2_MUL = 0;

uint16_t timer3_counter = 0;
uint16_t timer3_MUL = 0;

uint16_t timer_MOD_counter = 0;
uint16_t timer_MOD_MUL = 0;

uint16_t timer_alarm_counter = 0;
uint16_t timer_alarm_MUL = 0;

void timer_init(){
	HAL_TIM_Base_Start_IT(&htim2);
}

void setTimer2(uint16_t duration){
	timer2_MUL = duration/TIMER_CYCLE_2;
	timer2_counter = timer2_MUL;
	flag_timer2 = 0;
}

void setTimer3(uint16_t duration){
	timer3_MUL = duration/TIMER_CYCLE_2;
	timer3_counter = timer3_MUL;
	flag_timer3 = 0;
}

void setTimer_alarm(uint16_t duration){
	timer_alarm_MUL = duration/TIMER_CYCLE_2;
	timer_alarm_counter = timer3_MUL;
	flag_timer_alarm = 0;
}

void setTimer_MOD(uint16_t duration){
	timer_MOD_MUL = duration/TIMER_CYCLE_2;
	timer_MOD_counter = timer_MOD_MUL;
	flag_timer_MOD = 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM2){
		if(timer2_counter > 0){
			timer2_counter--;
			if(timer2_counter == 0) {
				flag_timer2 = 1;
				timer2_counter = timer2_MUL;
			}
		}

		if(timer3_counter > 0){
					timer3_counter--;
					if(timer3_counter == 0) {
						flag_timer3 = 1;
						timer3_counter = timer3_MUL;
					}
				}

		if(timer_alarm_counter > 0){
							timer_alarm_counter--;
							if(timer_alarm_counter == 0) {
								flag_timer_alarm = 1;
								timer_alarm_counter = timer_alarm_MUL;
							}
						}

		if(timer_MOD_counter > 0){
							timer_MOD_counter--;
							if(timer_MOD_counter == 0) {
								flag_timer_MOD = 1;
								timer_MOD_counter = timer_MOD_MUL;
							}
						}
		led7_Scan();
	}
}

