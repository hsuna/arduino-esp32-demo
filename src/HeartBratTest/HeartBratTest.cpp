#include <Arduino.h>
#include "HeartBratTest.h"

/*
电路图:
      ESP32 开发板                    心率传感器 (KY-039)
    +--------------+                +------------------+
    |              |                |                  |
    |         3.3V |----------------| + (VCC)          |
    |              |                |                  |
    |          GND |----------------| - (GND)          |
    |              |                |                  |
    |       GPIO34 |----------------| S (Signal/信号)  |
    |              |                |                  |
    +--------------+                +------------------+
*/
namespace HeartBratTest {
    const int SENSOR_PIN = 34;    // 传感器连接的模拟引脚
    const int SAMPLE_RATE = 20;   // 采样间隔 (ms)
    
    int sensorValue = 0;          // 当前传感器读数
    float signalFiltered = 0;     // 过滤后的信号
    float signalAvg = 0;          // 信号平均值 (用于动态阈值)
    
    unsigned long lastSampleTime = 0;
    unsigned long lastOutputTime = 0; // 上次输出时间
    bool isBeat = false;
    int bpm = 0;

    void init() {
        pinMode(SENSOR_PIN, INPUT);
        Serial.println("心跳检测模块初始化完成 (引脚: 34)");
    }

    void update() {
        unsigned long currentTime = millis();
        
        // 1. 采样与心跳检测
        if (currentTime - lastSampleTime >= SAMPLE_RATE) {
            lastSampleTime = currentTime;
            
            sensorValue = analogRead(SENSOR_PIN);

            // 简单的低通滤波器，减少噪声
            signalFiltered = signalFiltered * 0.9 + sensorValue * 0.1;
            
            // 动态计算信号平均值，作为基准线
            signalAvg = signalAvg * 0.99 + signalFiltered * 0.01;

            // 过滤掉不合理的心率 (40 - 200 BPM)
            if (signalAvg > 40 && signalAvg < 200) {
                bpm = signalAvg;
            } else {
                bpm = 0; // 无效心率
            }
        }

        // 2. 每秒输出一次结果
        if (currentTime - lastOutputTime >= 1000) {
            lastOutputTime = currentTime;

            Serial.print("当前心率 BPM: ");
            if (bpm > 0) {
                Serial.println(bpm);
            } else {
                Serial.println("等待信号...");
            }
        }
    }
} // namespace HeartBratTest