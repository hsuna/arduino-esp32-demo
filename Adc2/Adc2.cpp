#include <Arduino.h>
#include "Adc2.h"

namespace Adc2 {
    int POT_PIN = 34; // 电位器连接到 GPIO34
    int LED_PIN = 2;  // LED 连接到 GPIO2 
    int CHANNEL = 0; // ADC 通道

    int FREQ = 2000;     // PWM 频率
    int RESOLUTION = 12; // 分辨率

    int sensorValue = 0; // 存储模拟值

    void init() {
        analogReadResolution(RESOLUTION); // 设置 ADC 分辨率为 12 位
        analogSetAttenuation(ADC_11db); // 设置 ADC 衰减为 11dB

        ledcAttach(LED_PIN, FREQ, RESOLUTION); // 设置 LED 引脚为 PWM 输出模式
    }

    void update() {
        // 读取电位器的模拟值（范围 0-4095）
        sensorValue = analogRead(POT_PIN);

        // 设置 LED 亮度
        ledcWrite(LED_PIN, sensorValue);

        // 可选：打印调试信息到串口监视器
        Serial.printf("Potentiometer Value: %d", sensorValue);
        Serial.println("");

        delay(100); // 稍作延时，避免过于频繁的读取
    }
} // namespace Adc2