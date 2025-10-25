/*
 * software_timer.h
 *
 *  Created on: Oct 11, 2025
 *      Author: ADMIN
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_


#define TIMER_TICK_MS 1;
extern int timer1_flag;
extern int timer2_flag;
extern int timer1s_flag;

void setTimer1(int duration);
void setTimer2(int duration);
void setTimer1s(int duration);

extern int timer3_counter;
extern int timer3_flag;
void setTimer3(int duration);

void timerRun(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
