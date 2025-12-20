#include <Arduino.h>
#include "Adc.h"

namespace Adc {
    int POT_PIN = 34; // 电位器连接到 GPIO34
    int LED_PIN = 2;  // LED 连接到 GPIO2 

    int sensorValue = 0; // 存储模拟值
    int ledBrightness = 0; // 存储 LED 亮度值

    void init() {
        pinMode(LED_PIN, OUTPUT); // 设置 LED 引脚为输出模式
        pinMode(POT_PIN, INPUT);  // 设置电位器引脚为输入模式
    }

    void update() {
        // 读取电位器的模拟值（范围 0-4095）
        sensorValue = analogRead(POT_PIN);

        // 将模拟值映射到 LED 亮度范围（0-255）
        ledBrightness = map(sensorValue, 0, 4095, 0, 255);

        // 设置 LED 亮度
        analogWrite(LED_PIN, ledBrightness);

        // 可选：打印调试信息到串口监视器
        Serial.printf("Potentiometer Value: %d，LED Brightness: %d", sensorValue, ledBrightness);
        Serial.println("");

        delay(1000); // 稍作延时，避免过于频繁的读取
    }
} // namespace Adc