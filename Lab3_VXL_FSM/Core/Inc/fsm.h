#ifndef FSM_H
#define FSM_H

#include "main.h"

// Định nghĩa các mode
typedef enum {
    MODE_1 = 1,  // Mode hoạt động bình thường
    MODE_2,      // Mode cài đặt thời gian đèn đỏ
    MODE_3,      // Mode cài đặt thời gian đèn vàng
    MODE_4       // Mode cài đặt thời gian đèn xanh
} SystemMode;

// Khai báo extern cho các biến toàn cục (chỉ khai báo, không định nghĩa)
extern SystemMode mode;
extern int red_time;
extern int amber_time;
extern int green_time;
extern int temp_red_time;
extern int temp_amber_time;
extern int temp_green_time;

// Hàm xử lý FSM
void fsm_init(void);
void fsm_handle_buttons(void);
void fsm_update_display(void);

#endif
