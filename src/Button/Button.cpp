#include <Arduino.h>
#include "Button.h"

namespace Button {
    int LED_PIN = 27;
    int BUTTON_PIN = 12;

    // LED 初始状态为关闭
    int led_logic = LOW;

    // LED 当前状态
    bool status = false;

    void init() {
        // 设定引脚为输出模式
        pinMode(LED_PIN, OUTPUT);
        pinMode(BUTTON_PIN, INPUT_PULLDOWN);
    }

    void update() {
        if (digitalRead(BUTTON_PIN) == HIGH) {
            delay(100); // 消抖延时
            if (digitalRead(BUTTON_PIN) == HIGH && !status) {
                led_logic = !led_logic;
                digitalWrite(LED_PIN, led_logic);
                status = true;
            }
        } else {
            status = false;
        }
    }
} // namespace Button