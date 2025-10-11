/*
 * button.h
 *
 *  Created on: Oct 11, 2025
 *      Author: ADMIN
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#define NORMAL_STATE GPIO_PIN_SET
#define PRESSED_STATE GPIO_PIN_SET


int isButton1Pressed();
void getkeyInput();
#endif /* INC_BUTTON_H_ */
