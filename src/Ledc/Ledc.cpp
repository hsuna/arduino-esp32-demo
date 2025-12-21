#include <Arduino.h>
#include "Ledc.h"

/*
电路图:
      ESP32 开发板                    LED 电路
    +--------------+                +------------------+
    |              |                |                  |
    |       GPIO27 | --- [220Ω] --- | LED -------- GND |
    |              |                |                  |
    +--------------+                +------------------+
*/
namespace Ledc {
    int LED_PIN = 27;    // PWM 输出引脚

    int FREQ = 2000;     // PWM 频率
    int RESOLUTION = 8; // PWM 分辨率

    void init() {
        ledcAttach(LED_PIN, FREQ, RESOLUTION);
    }

    void update() {
        // 渐变亮度
        for (int duty = 0; duty < pow(2, RESOLUTION); duty++) {
            ledcWrite(LED_PIN, duty); // 设置 PWM 占空比
            delay(10);
        }
        // 渐变熄灭
        for (int duty = pow(2, RESOLUTION) - 1; duty >= 0; duty--) {
            ledcWrite(LED_PIN, duty); // 设置 PWM 占空比
            delay(10);
        }
    }
} // namespace Ledc