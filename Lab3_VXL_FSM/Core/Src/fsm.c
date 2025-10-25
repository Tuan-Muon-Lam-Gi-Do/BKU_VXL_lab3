#include "fsm.h"
#include "button.h"
#include "software_timer.h"
#include "main.h"

// Định nghĩa các biến toàn cục (chỉ ở đây)
SystemMode mode = MODE_1;
int red_time = 5;
int amber_time = 2;
int green_time = 3;
int temp_red_time = 5;
int temp_amber_time = 2;
int temp_green_time = 3;
TrafficState state = S_RED_GREEN;
int counter = 0;
int led_counter_left = 0;
int led_counter_right = 0;
// Thêm vào fsm.c
int error_flag = 0;
int error_counter = 0;

// Biến cho nhấp nháy
int blink_flag = 0;
int blink_counter = 0;

void fsm_init(void) {
    mode = MODE_1;
    temp_red_time = red_time;
    temp_amber_time = amber_time;
    temp_green_time = green_time;
}

void fsm_handle_buttons(void) {
    // Nút 1: Chuyển mode
    if(isButton1Pressed()) {
        mode++;
        if(mode > MODE_4){
            state = S_RED_GREEN;
            counter = green_time;

            led_counter_left = green_time + amber_time;
            led_counter_right = green_time;
            setTimer1(green_time * 100);
            setTimer1s(100);
            display_state();
            updateLedBuffer(led_counter_left, led_counter_right);
            mode = MODE_1;
        }

        // Reset biến tạm khi chuyển mode
        temp_red_time = red_time;
        temp_amber_time = amber_time;
        temp_green_time = green_time;
        blink_flag = 0;
        blink_counter = 0;
        blink_flag = 0; // Luôn bắt đầu ở trạng thái TẮT
        setTimer3(50);  // Đặt hẹn giờ 50 * 10ms = 500ms (0.5s)
            // ---------------

    }

    // Nút 2: Tăng thời gian (trong mode cài đặt)
    if(isButton2Pressed()) {
        switch(mode) {
            case MODE_2:
                if(temp_red_time < 99) temp_red_time++;
                break;
            case MODE_3:
                if(temp_amber_time < 99) temp_amber_time++;
                break;
            case MODE_4:
                if(temp_green_time < 99) temp_green_time++;
                break;
            default:
                break;
        }
    }

    // Nút 3: Lưu giá trị (trong mode cài đặt)
    if(isButton3Pressed()) {
        switch(mode) {
            case MODE_2:
                red_time = temp_red_time;
                break;
            case MODE_3:
                amber_time = temp_amber_time;
                break;
            case MODE_4:
                green_time = temp_green_time;
                break;
            default:
                break;
        }
    }
}

void fsm_update_display(void) {
    static int last_mode = 0;

    // Chỉ cập nhật khi mode thay đổi
    if(mode != last_mode) {
        last_mode = mode;
        blink_flag = 0;
        blink_counter = 0;
    }

    switch(mode) {
        case MODE_1:
            // Hiển thị bình thường - 2 LED 7 đoạn hiển thị counter
            // (giữ nguyên logic hiển thị từ main.c)
            break;

        case MODE_2:
            // Nhấp nháy đèn đỏ và hiển thị thời gian đèn đỏ
        	if(timer3_flag == 1) {
        	            timer3_flag = 0;   // Xóa cờ
        	            setTimer3(50);     // Đặt lại hẹn giờ cho 0.5s tiếp theo
        	            blink_flag = !blink_flag; // Đảo trạng thái công tắc
        	        }

        	        // 2. Cập nhật LED dựa trên trạng thái 'blink_flag'
        	        // Logic này chạy liên tục, giữ trạng thái TẮT hoặc BẬT
        	        if(blink_flag) {
        	            // Bật đèn đỏ cả 2 hướng
        	            HAL_GPIO_WritePin(GPIOA, Red_1_Pin, GPIO_PIN_RESET);
        	            HAL_GPIO_WritePin(GPIOA, Red_2_Pin, GPIO_PIN_RESET);
        	            // Tắt các đèn khác
        	            HAL_GPIO_WritePin(GPIOA, Amber_1_Pin|Amber_2_Pin|Green_1_Pin|Green_2_Pin, GPIO_PIN_SET);
        	        } else {
        	            // Tắt tất cả đèn
        	            HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin|Amber_2_Pin|Green_2_Pin, GPIO_PIN_SET);
        	        }
            // Cập nhật LED 7 đoạn với thời gian đèn đỏ
            updateLedBuffer(2, 0);
            updateLedBuffer(temp_red_time, 2);
            break;

        case MODE_3:
            // Nhấp nháy đèn vàng và hiển thị thời gian đèn vàng
        	if(timer3_flag == 1) {
        	            timer3_flag = 0;
        	            setTimer3(50);
        	            blink_flag = !blink_flag;
        	        }

        	        if(blink_flag) {
        	            // Bật đèn vàng cả 2 hướng
        	            HAL_GPIO_WritePin(GPIOA, Amber_1_Pin, GPIO_PIN_RESET);
        	            HAL_GPIO_WritePin(GPIOA, Amber_2_Pin, GPIO_PIN_RESET);
        	            // Tắt các đèn khác
        	            HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Red_2_Pin|Green_1_Pin|Green_2_Pin, GPIO_PIN_SET);
        	        } else {
        	            // Tắt tất cả đèn
        	            HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin|Amber_2_Pin|Green_2_Pin, GPIO_PIN_SET);
        	        }
            updateLedBuffer(3, 0);
            updateLedBuffer(temp_amber_time, 3);
            break;

        case MODE_4:
            // Nhấp nháy đèn xanh và hiển thị thời gian đèn xanh
        	if(timer3_flag == 1) {
        	            timer3_flag = 0;
        	            setTimer3(50);
        	            blink_flag = !blink_flag;
        	        }

        	        if(blink_flag) {
        	            // Bật đèn xanh cả 2 hướng
        	            HAL_GPIO_WritePin(GPIOA, Green_1_Pin, GPIO_PIN_RESET);
        	            HAL_GPIO_WritePin(GPIOA, Green_2_Pin, GPIO_PIN_RESET);
        	            // Tắt các đèn khác
        	            HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Red_2_Pin|Amber_1_Pin|Amber_2_Pin, GPIO_PIN_SET);
        	        } else {
        	            // Tắt tất cả đèn
        	            HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin|Amber_2_Pin|Green_2_Pin, GPIO_PIN_SET);
        	        }
            // Cập nhật LED 7 đoạn với thời gian đèn xanh
            updateLedBuffer(4, 0);
            updateLedBuffer(temp_green_time, 4);
            break;
    }
}

