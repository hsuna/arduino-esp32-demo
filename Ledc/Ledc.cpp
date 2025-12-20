#include <Arduino.h>
#include "Ledc.h"

namespace Ledc {
    int LED_PIN = 27;    // PWM 输出引脚

    int freq = 2000;     // PWM 频率
    int resolution = 8; // PWM 分辨率

    void init() {
        ledcAttach(LED_PIN, freq, resolution);
    }

    void update() {
        // 渐变亮度
        for (int duty = 0; duty < pow(2, resolution); duty++) {
            ledcWrite(LED_PIN, duty); // 设置 PWM 占空比
            delay(10);
        }
        // 渐变熄灭
        for (int duty = pow(2, resolution) - 1; duty >= 0; duty--) {
            ledcWrite(LED_PIN, duty); // 设置 PWM 占空比
            delay(10);
        }
    }
} // namespace Ledc