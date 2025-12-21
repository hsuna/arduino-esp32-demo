#include <Arduino.h>
#include "Pwm.h"

namespace Pwm {
    int LED_PIN = 27;

    void init() {
        // 设定引脚为输出模式
        pinMode(LED_PIN, OUTPUT);
    }

    void update() {
        for (int duty = 0; duty <= 255; duty++) {
            analogWrite(LED_PIN, duty); // 设置 PWM 占空比
            delay(10);
        }
        for (int duty = 255; duty >= 0; duty--) {
            analogWrite(LED_PIN, duty); // 设置 PWM 占空比
            delay(10);
        }
    }
} // namespace Pwm