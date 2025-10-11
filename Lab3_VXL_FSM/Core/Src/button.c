/*
 * button.c
 *
 *  Created on: Oct 11, 2025
 *      Author: ADMIN
 */

#include "button.h"
#include "main.h"

int button1_flag = 0;
int button2_flag = 0;
int button3_flag = 0;

// Biến debounce cho 3 nút
int KeyReg0[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg1[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg2[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int KeyReg3[3] = {NORMAL_STATE, NORMAL_STATE, NORMAL_STATE};
int TimeForKeyPress[3] = {200,200,200};

// Kiểm tra nút nhấn
int isButton1Pressed(){ if(button1_flag){ button1_flag=0; return 1; } return 0; }
int isButton2Pressed(){ if(button2_flag){ button2_flag=0; return 1; } return 0; }
int isButton3Pressed(){ if(button3_flag){ button3_flag=0; return 1; } return 0; }

// Hàm xử lý khi nút được nhấn
void subKeyProcess(int button_index){
    if(button_index==0) button1_flag=1;
    else if(button_index==1) button2_flag=1;
    else if(button_index==2) button3_flag=1;
}

// Quét tất cả nút
void getKeyInput() {
    GPIO_TypeDef* ports[3] = {BUTTON_1_GPIO_Port, BUTTON_2_GPIO_Port, BUTTON_3_GPIO_Port};
    uint16_t pins[3] = {BUTTON_1_Pin, BUTTON_2_Pin, BUTTON_3_Pin};

    for(int i=0; i<3; i++){
        KeyReg0[i] = KeyReg1[i];
        KeyReg1[i] = KeyReg2[i];
        KeyReg2[i] = HAL_GPIO_ReadPin(ports[i], pins[i]);

        if((KeyReg0[i]==KeyReg1[i]) && (KeyReg1[i]==KeyReg2[i])){
            if(KeyReg3[i]!=KeyReg2[i]){
                KeyReg3[i]=KeyReg2[i];
                if(KeyReg2[i]==PRESSED_STATE) subKeyProcess(i);
                TimeForKeyPress[i]=200;
            }
        } else {
            TimeForKeyPress[i]--;
            if(TimeForKeyPress[i]==0){
                subKeyProcess(i);
                TimeForKeyPress[i]=200;
            }
        }
    }
}
