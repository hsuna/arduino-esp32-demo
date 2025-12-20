#include <Arduino.h>
#include "Button.h"

namespace Button {
    int led_pin = 27;
    int button_pin = 12;

    // LED 初始状态为关闭
    int led_logic = LOW;

    // LED 当前状态
    bool status = false;

    void init() {
        // 设定引脚为输出模式
        pinMode(led_pin, OUTPUT);
        pinMode(button_pin, INPUT_PULLDOWN);
    }

    void update() {
        if (digitalRead(button_pin) == HIGH) {
            delay(100); // 消抖延时
            if (digitalRead(button_pin) == HIGH && !status) {
                led_logic = !led_logic;
                digitalWrite(led_pin, led_logic);
                status = true;
            }
        } else {
            status = false;
        }
    }
} // namespace Button