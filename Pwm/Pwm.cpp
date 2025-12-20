#include <Arduino.h>
#include "Pwm.h"

namespace Pwm {
    int led_pin = 27;

    void init() {
        // 设定引脚为输出模式
        pinMode(led_pin, OUTPUT);
    }

    void update() {
        for (int duty = 0; duty <= 255; duty++) {
            analogWrite(led_pin, duty); // 设置 PWM 占空比
            delay(10);
        }
        for (int duty = 255; duty >= 0; duty--) {
            analogWrite(led_pin, duty); // 设置 PWM 占空比
            delay(10);
        }
    }
} // namespace Pwm