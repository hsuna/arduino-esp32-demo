#include <Arduino.h>
#include "Ledc.h"

namespace Ledc {
    int freq = 2000;     // PWM 频率
    int channel = 0;   // PWM 通道
    int resolution = 8; // PWM 分辨率
    int led_pin = 27;    // PWM 输出引脚

    void init() {
        ledcSetup(channel, freq, resolution);
        ledcAttachPin(led_pin, channel);
    }

    void update() {
        // 渐变亮度
        for (int duty = 0; duty < pow(2, resolution); duty++) {
            ledcWrite(channel, duty); // 设置 PWM 占空比
            delay(10);
        }
        // 渐变熄灭
        for (int duty = pow(2, resolution) - 1; duty >= 0; duty--) {
            ledcWrite(channel, duty); // 设置 PWM 占空比
            delay(10);
        }
    }
} // namespace Ledc