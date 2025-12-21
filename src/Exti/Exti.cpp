#include <Arduino.h>
#include "Exti.h"

/*
电路图:
      ESP32 开发板                    外围器件
    +--------------+                +------------------+
    |              |                |                  |
    |        GPIO2 | --- [220Ω] --- | LED -------- GND |
    |              |                |                  |
    |       GPIO14 |----------------| 按键 ------- 3.3V |
    |              |                | (内部上拉)        |
    +--------------+                +------------------+
*/
namespace Exti {
    int LED_PIN = 2;
    int BUTTON_PIN = 14;

    // 使用 volatile 确保 ISR 和主循环可见性
    volatile bool led_logic = false;

    // 中断服务函数
    void IRAM_ATTR handle_interrupt() {
        led_logic = !led_logic;
    }

    void init() {
        // 设定引脚为输出模式
        pinMode(LED_PIN, OUTPUT);
        pinMode(BUTTON_PIN, INPUT_PULLDOWN);

        attachInterrupt(BUTTON_PIN, handle_interrupt, FALLING);
    }

    void update() {
        if (led_logic) {
            digitalWrite(LED_PIN, HIGH);
        } else {
            digitalWrite(LED_PIN, LOW);
        }
    }
} // namespace Exti