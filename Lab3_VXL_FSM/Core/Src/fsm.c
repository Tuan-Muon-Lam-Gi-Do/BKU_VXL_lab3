#include "fsm.h"
#include "button.h"
#include "main.h"
#include "scheduler.h" // Sử dụng bộ lập lịch

// Định nghĩa các biến toàn cục (chỉ ở đây)
SystemMode mode = MODE_1;
int red_time = 5;
int amber_time = 2;
int green_time = 3;
int temp_red_time = 5;
int temp_amber_time = 2;
int temp_green_time = 3;
TrafficState state = S_RED_GREEN;
int counter = 0; // Biến này có vẻ không còn được dùng, nhưng giữ lại
int led_counter_left = 0;
int led_counter_right = 0;

// Biến cho nhấp nháy
int blink_flag = 0;

// Các biến đếm nội bộ thay thế cho software_timer
// Đơn vị là 'ticks' (1 tick = 10ms)
static int fsm_timer_counter = 0;   // Thay thế cho timer1 (chuyển trạng thái FSM)
static int fsm_1sec_counter = 0;    // Thay thế cho timer1s (đếm lùi 7-seg)
static int fsm_blink_counter = 0; // Thay thế cho timer3 (nháy đèn)

// --- Di chuyển từ main.c ---
// Buffer 7-segment cho 4 LED (2 cặp)
uint8_t led7segBuffer[4];
const uint8_t SEGMENTS[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};

void updateLedBuffer(uint8_t number_left, uint8_t number_right) {
    if(number_left > 99) number_left = 99;
    if(number_right > 99) number_right = 99;

    led7segBuffer[0] = SEGMENTS[number_left / 10];
    led7segBuffer[1] = SEGMENTS[number_left % 10];
    led7segBuffer[2] = SEGMENTS[number_right / 10];
    led7segBuffer[3] = SEGMENTS[number_right % 10];
}

// --- Di chuyển từ main.c ---
void display_state(void)
{
  // Tắt hết
  HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin|Amber_2_Pin|Green_2_Pin, GPIO_PIN_SET);

  switch (state)
  {
    case S_RED_GREEN:
      HAL_GPIO_WritePin(GPIOA, Red_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, Green_2_Pin, GPIO_PIN_RESET);
      break;

    case S_RED_AMBER:
      HAL_GPIO_WritePin(GPIOA, Red_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, Amber_2_Pin, GPIO_PIN_RESET);
      break;

    case S_GREEN_RED:
      HAL_GPIO_WritePin(GPIOA, Green_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, Red_2_Pin, GPIO_PIN_RESET);
      break;

    case S_AMBER_RED:
      HAL_GPIO_WritePin(GPIOA, Amber_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, Red_2_Pin, GPIO_PIN_RESET);
      break;
  }
}

// --- Di chuyển từ main.c và Sửa đổi ---
// Hàm FSM cập nhật trạng thái dựa trên counter
void fsm_update(void) {
    switch (state) {
        case S_RED_GREEN:
            led_counter_left = amber_time;
            led_counter_right = amber_time;
            counter = amber_time;
            state = S_RED_AMBER;
            fsm_timer_counter = amber_time * 100; // Thay thế setTimer1
            break;

        case S_RED_AMBER:
            led_counter_left = green_time;
            led_counter_right = amber_time + green_time;
            counter = green_time;
            state = S_GREEN_RED;
            fsm_timer_counter = green_time * 100; // Thay thế setTimer1
            break;

        case S_GREEN_RED:
            led_counter_left = amber_time;
            led_counter_right = amber_time;
            counter = amber_time;
            state = S_AMBER_RED;
            fsm_timer_counter = amber_time * 100; // Thay thế setTimer1
            break;

        case S_AMBER_RED:
            led_counter_left = amber_time + green_time;
            led_counter_right = green_time;
            counter = green_time;
            state = S_RED_GREEN;
            fsm_timer_counter = green_time * 100; // Thay thế setTimer1
            break;
    }
    display_state();

    // CẬP NHẬT NGAY LED SAU KHI CHUYỂN TRẠNG THÁI
    updateLedBuffer(led_counter_left, led_counter_right);
}

// --- Sửa đổi ---
void fsm_init(void) {

    // Reset biến tạm
    temp_red_time = red_time;
    temp_amber_time = amber_time;
    temp_green_time = green_time;

    // Đặt trạng thái ban đầu (từ main.c)
    mode = MODE_1;
    state = S_RED_GREEN;
    counter = green_time;
    led_counter_left = green_time + amber_time;  // 5
    led_counter_right = green_time;              // 3

    // Cập nhật đèn và 7-seg
    display_state();
    updateLedBuffer(led_counter_left, led_counter_right);

    // Đặt giá trị ban đầu cho các biến đếm của scheduler
    // (Đơn vị là ticks, 1 tick = 10ms)
    fsm_timer_counter = green_time * 100; // 300 ticks = 3s
    fsm_1sec_counter = 100;               // 100 ticks = 1s
    fsm_blink_counter = 50;               // 50 ticks = 0.5s (cho nháy)
    blink_flag = 0;                       // Bắt đầu ở trạng thái TẮT
}

// --- Sửa đổi ---
void fsm_handle_buttons(void) {
    // Nút 1: Chuyển mode
    if(isButton1Pressed()) {
        mode++;
        if(mode > MODE_4){
            fsm_init(); // Gọi hàm init để reset mọi thứ về MODE_1
            mode = MODE_1; // Đảm bảo mode là 1
            return; // Thoát ngay sau khi reset
        }

        // Reset biến tạm khi chuyển mode
        temp_red_time = red_time;
        temp_amber_time = amber_time;
        temp_green_time = green_time;

        // Reset bộ đếm nhấp nháy
        fsm_blink_counter = 50; // Đặt lại 0.5s
        blink_flag = 0;         // Luôn bắt đầu ở trạng thái TẮT
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

// --- Sửa đổi ---
void fsm_update_display(void) {
    // Hàm này bây giờ chỉ chịu trách nhiệm BẬT/TẮT LED
    // và cập nhật 7-seg dựa trên 'blink_flag'
    // 'blink_flag' được cập nhật trong fsm_run()

    switch(mode) {
        case MODE_1:
            // Không làm gì, MODE_1 được xử lý riêng
            break;

        case MODE_2: // Nhấp nháy đèn đỏ
            if(blink_flag) {
                HAL_GPIO_WritePin(GPIOA, Red_1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, Red_2_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, Amber_1_Pin|Amber_2_Pin|Green_1_Pin|Green_2_Pin, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin|Amber_2_Pin|Green_2_Pin, GPIO_PIN_SET);
            }
            // Cập nhật LED 7 đoạn
            updateLedBuffer(temp_red_time, 2);
            break;

        case MODE_3: // Nhấp nháy đèn vàng
            if(blink_flag) {
                HAL_GPIO_WritePin(GPIOA, Amber_1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, Amber_2_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Red_2_Pin|Green_1_Pin|Green_2_Pin, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin|Amber_2_Pin|Green_2_Pin, GPIO_PIN_SET);
            }
            updateLedBuffer(temp_amber_time, 3);
            break;

        case MODE_4: // Nhấp nháy đèn xanh
            if(blink_flag) {
                HAL_GPIO_WritePin(GPIOA, Green_1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, Green_2_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Red_2_Pin|Amber_1_Pin|Amber_2_Pin, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(GPIOA, Red_1_Pin|Amber_1_Pin|Green_1_Pin|Red_2_Pin|Amber_2_Pin|Green_2_Pin, GPIO_PIN_SET);
            }
            updateLedBuffer(temp_green_time, 4);
            break;
    }
}

// --- HÀM MỚI (Task chính) ---
// Hàm này được gọi mỗi 10ms (1 tick) bởi bộ lập lịch
void fsm_run(void) {

    // 1. Luôn kiểm tra nút nhấn
    fsm_handle_buttons();

    // 2. Chạy FSM chính dựa trên mode
    if (mode == MODE_1) {

        // Kiểm tra điều kiện lỗi (phải được đảm bảo)
        if(green_time + amber_time != red_time){
            fsm_init(); // Reset lại toàn bộ hệ thống về mặc định
        }

        // Xử lý đếm 1 giây (thay cho timer1s)
        if (fsm_1sec_counter > 0) {
            fsm_1sec_counter--;
        }
        if (fsm_1sec_counter <= 0) {
            // Đã qua 1 giây
            fsm_1sec_counter = 100; // Đặt lại 100 ticks (1s)

            if (led_counter_left > 1) led_counter_left--;
            if (led_counter_right > 1) led_counter_right--;

            updateLedBuffer(led_counter_left, led_counter_right);
        }

        // Xử lý chuyển trạng thái đèn (thay cho timer1)
        if (fsm_timer_counter > 0) {
            fsm_timer_counter--;
        }
        if (fsm_timer_counter <= 0) {
            // Hết giờ, chuyển trạng thái
            fsm_update(); // Hàm này sẽ tự đặt lại fsm_timer_counter
        }
    }
    else {
        // Đây là MODE 2, 3, hoặc 4 (Mode cài đặt)

        // Xử lý nhấp nháy (thay cho timer3)
        if (fsm_blink_counter > 0) {
            fsm_blink_counter--;
        }
        if (fsm_blink_counter <= 0) {
            fsm_blink_counter = 50; // Đặt lại 50 ticks (0.5s)
            blink_flag = !blink_flag; // Đảo cờ nháy
        }

        // Cập nhật hiển thị (LED nháy và 7-seg)
        fsm_update_display();
    }
}
