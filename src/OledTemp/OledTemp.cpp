#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include "OledTemp.h"

/*
电路图:
      ESP32 开发板                    外围器件
    +--------------+                +------------------+
    |              |                | SSD1306 OLED     |
    |         3.3V |----------------| VDD              |
    |          GND |----------------| GND              |
    |       GPIO21 |----------------| SDA              |
    |       GPIO22 |----------------| SCK              |
    +--------------+                +------------------+
    |              |                | DHT11 传感器      |
    |         3.3V |----------------| (中间引脚)        |
    |          GND |----------------| -                |
    |       GPIO13 |----------------| S                |
    +--------------+                +------------------+
*/

namespace OledTemp
{
    // 使用 U8g2 库支持中文显示
    // U8G2_SSD1306_128X64_NONAME_F_HW_I2C: 完整帧缓冲, 硬件 I2C
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/22, /* data=*/21);

    DHT dht(13, DHT11); // DHT11 传感器连接到 GPIO13

    unsigned long lastUpdateTime = 0;

    void init()
    {
        Serial.println("OledTemp 初始化...");

        // 初始化 DHT 传感器
        dht.begin();

        // 初始化 U8g2 OLED 显示屏
        u8g2.begin();
        u8g2.enableUTF8Print(); // 启用 UTF-8 支持

        // 设置中文字体 (wqy12 是文泉驿 12 像素点阵字体)
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.setFontDirection(0);

        // 显示启动信息
        u8g2.clearBuffer();
        u8g2.setCursor(20, 30);
        u8g2.print("系统准备就绪...");
        u8g2.sendBuffer();
        delay(1000);
    }

    void update()
    {
        unsigned long currentTime = millis();

        // 每 2 秒读取并更新一次
        if (currentTime - lastUpdateTime >= 2000)
        {
            lastUpdateTime = currentTime;

            float h = dht.readHumidity();
            float t = dht.readTemperature();

            if (isnan(h) || isnan(t))
            {
                Serial.println(F("读取 DHT 传感器失败!"));
                u8g2.clearBuffer();
                u8g2.setCursor(0, 15);
                u8g2.print("传感器错误!");
                u8g2.sendBuffer();
                return;
            }

            // 串口输出
            Serial.print(F("湿度: "));
            Serial.print(h);
            Serial.print(F("%  温度: "));
            Serial.print(t);
            Serial.println(F("°C"));

            // OLED 显示中文
            u8g2.clearBuffer();

            // 标题
            u8g2.setFont(u8g2_font_wqy12_t_gb2312);
            u8g2.setCursor(0, 12);
            u8g2.print("温湿度监测");

            // 温度显示
            u8g2.setCursor(0, 32);
            u8g2.print("温度: ");
            u8g2.print(t, 1);
            u8g2.print(" °C");

            // 湿度显示
            u8g2.setCursor(0, 52);
            u8g2.print("湿度: ");
            u8g2.print(h, 1);
            u8g2.print(" %");

            u8g2.sendBuffer();
        }
    }
} // namespace OledTemp
