#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include "SmartMonitor.h"

/*
电路图:
      ESP32 开发板                    外围器件
    +--------------+                +------------------+
    |              |                | SSD1306 OLED     |
    |         3.3V |----------------| VCC              |
    |          GND |----------------| GND              |
    |       GPIO21 |----------------| SDA              |
    |       GPIO22 |----------------| SCL              |
    +--------------+                +------------------+
    |              |                | DHT11 传感器      |
    |       GPIO13 |----------------| S                |
    |         3.3V |----------------| VCC              |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | 光敏电阻 (LDR)    |
    |       GPIO35 |-------+--------| S (信号)          |
    |              |       |        |                  |
    |              |     [10kΩ]     | (如果是2针脚LDR,  |
    |              |       |        |  需加10kΩ下拉电阻) |
    |          GND |-------+        |                  |
    |         3.3V |----------------| VCC              |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | 电位器            |
    |       GPIO34 |----------------| S (信号)          |
    |         3.3V |----------------| VCC              |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | 4脚按键 (Switch)  |
    |       GPIO14 |----------------| 引脚 1 (或 A)     |
    |          GND |----------------| 引脚 3 (或 B)     |
    |              |                | (对角线连接最稳妥) |
    +--------------+                +------------------+
    |              |                | 蜂鸣器 (Buzzer)   |
    |       GPIO12 |----------------| S (信号)          |
    |         3.3V |----------------| VCC              |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | RGB LED (共阴)    |
    |        GPIO4 | --- [220Ω] --- | R                |
    |       GPIO16 | --- [220Ω] --- | G                |
    |       GPIO17 | --- [220Ω] --- | B                |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
*/

namespace SmartMonitor
{
    // 引脚定义
    const int DHT_PIN = 13;    // DHT11 传感器引脚
    const int LDR_PIN = 35;    // 光敏电阻引脚
    const int POT_PIN = 34;    // 电位器引脚
    const int BTN_PIN = 14;    // 按键引脚
    const int BUZZER_PIN = 12; // 蜂鸣器引脚
    const int RGB_R_PIN = 4;   // RGB LED 红色引脚
    const int RGB_G_PIN = 16;  // RGB LED 绿色引脚
    const int RGB_B_PIN = 17;  // RGB LED 蓝色引脚

    // 实例化对象
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/22, /* data=*/21);
    DHT dht(DHT_PIN, DHT11);

    // 变量
    float temperature = 0;
    float humidity = 0;
    int lightLevel = 0;
    int threshold = 0;
    bool displayMode = 0; // 0: 环境数据, 1: 系统状态
    unsigned long lastUpdateTime = 0;
    unsigned long lastBtnPress = 0;

    void setRGB(int r, int g, int b)
    {
        analogWrite(RGB_R_PIN, r);
        analogWrite(RGB_G_PIN, g);
        analogWrite(RGB_B_PIN, b);
    }

    void init()
    {
        Serial.println("SmartMonitor 初始化...");

        // 引脚模式
        pinMode(LDR_PIN, INPUT);
        pinMode(POT_PIN, INPUT);
        pinMode(BTN_PIN, INPUT_PULLUP);
        pinMode(BUZZER_PIN, OUTPUT);
        pinMode(RGB_R_PIN, OUTPUT);
        pinMode(RGB_G_PIN, OUTPUT);
        pinMode(RGB_B_PIN, OUTPUT);

        // 初始化传感器
        dht.begin();
        u8g2.begin();
        u8g2.enableUTF8Print();
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);

        // 欢迎界面
        u8g2.clearBuffer();
        u8g2.setCursor(15, 35);
        u8g2.print("智能管家启动中...");
        u8g2.sendBuffer();
        delay(1500);
    }

    void update()
    {
        // 1. 处理按键 (切换显示模式)
        if (digitalRead(BTN_PIN) == LOW)
        {
            if (millis() - lastBtnPress > 300)
            { // 消抖
                displayMode = !displayMode;
                lastBtnPress = millis();
                Serial.println("切换显示模式");
            }
        }

        // 2. 定时读取传感器 (每 1 秒)
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= 1000)
        {
            lastUpdateTime = currentTime;

            temperature = dht.readTemperature();
            humidity = dht.readHumidity();
            lightLevel = analogRead(LDR_PIN);
            threshold = analogRead(POT_PIN);

            // 3. 逻辑处理：报警与 LED 颜色
            // 报警逻辑：如果光线太暗 (LDR 值大) 且超过电位器设定的阈值
            if (lightLevel > threshold)
            {
                digitalWrite(BUZZER_PIN, HIGH);
                setRGB(255, 0, 0); // 红色警告
            }
            else
            {
                digitalWrite(BUZZER_PIN, LOW);
                // 根据温度显示颜色
                if (temperature < 20)
                    setRGB(0, 0, 255); // 蓝色 (冷)
                else if (temperature < 28)
                    setRGB(0, 255, 0); // 绿色 (舒适)
                else
                    setRGB(255, 165, 0); // 橙色 (热)
            }

            // 4. 刷新 OLED
            u8g2.clearBuffer();
            if (displayMode == 0)
            {
                // 模式 0: 环境数据
                u8g2.setCursor(0, 12);
                u8g2.print("--- 环境监测 ---");
                u8g2.setCursor(0, 30);
                u8g2.printf("温度: %.1f °C", temperature);
                u8g2.setCursor(0, 45);
                u8g2.printf("湿度: %.1f %%", humidity);
                u8g2.setCursor(0, 60);
                u8g2.printf("光照: %d", lightLevel);
            }
            else
            {
                // 模式 1: 系统状态
                u8g2.setCursor(0, 12);
                u8g2.print("--- 系统状态 ---");
                u8g2.setCursor(0, 30);
                u8g2.printf("报警阈值: %d", threshold);
                u8g2.setCursor(0, 45);
                u8g2.print(lightLevel > threshold ? "状态: 警告!" : "状态: 正常");
                u8g2.setCursor(0, 60);
                u8g2.printf("运行时间: %lu s", millis() / 1000);
            }
            u8g2.sendBuffer();

            // 串口调试
            Serial.printf("T:%.1f H:%.1f L:%d Th:%d\n", temperature, humidity, lightLevel, threshold);
        }
    }
} // namespace SmartMonitor
