#include <Arduino.h>
#include <Ticker.h>
#include "Timeout.h"

/*
电路图:
      ESP32 开发板                    外围器件
    +--------------+                +------------------+
    |              |                |                  |
    |        GPIO2 | --- [220Ω] --- | LED -------- GND |
    |              |                |                  |
    |        GPIO4 | --- [220Ω] --- | LED -------- GND |
    |              |                |                  |
    |       GPIO15 | --- [220Ω] --- | LED -------- GND |
    |              |                |                  |
    +--------------+                +------------------+
*/
namespace Timeout {
    int LED_PIN = 2;
    int LED2_PIN = 4;
    int LED3_PIN = 15;

    hw_timer_t* timer = NULL;
    hw_timer_t* timer2 = NULL;

    Ticker ticker;
 
    // 中断服务函数 (ISR)
    void handle_interrupt() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // 切换 LED 状态
    }

    void handle_interrupt2() {
        digitalWrite(LED2_PIN, !digitalRead(LED2_PIN)); // 切换 LED2 状态
    }

    void handle_interrupt3() {
        digitalWrite(LED3_PIN, !digitalRead(LED3_PIN)); // 切换 LED3 状态
    }

    void init() {
        // 设定引脚为输出模式
        pinMode(LED_PIN, OUTPUT);
        pinMode(LED2_PIN, OUTPUT);
        pinMode(LED3_PIN, OUTPUT);

        // 配置定时器 (Arduino ESP32 v3.0+ 新 API)
        // timerBegin(频率): 1,000,000 Hz = 1us 计数一次
        timer = timerBegin(1000000); 
        timerAttachInterrupt(timer, handle_interrupt);
        // timerAlarm(定时器, 触发值, 是否自动重载, 重载次数)
        timerAlarm(timer, 1000000, true, 0); // 每 1,000,000us (1秒) 触发一次

        timer2 = timerBegin(1000000);
        timerAttachInterrupt(timer2, handle_interrupt2);
        timerAlarm(timer2, 500000, true, 0); // 每 500,000us (0.5秒) 触发一次

        ticker.attach(2, handle_interrupt3); // 每 2 秒触发一次
    }

    void update() {
    }
} // namespace Timeout