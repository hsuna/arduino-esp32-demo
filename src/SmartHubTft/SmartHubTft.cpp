#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <U8g2_for_Adafruit_GFX.h>
#include <SPI.h>
#include <DHT.h>
#include "SmartHubTft.h"

/*
电路图 (TFT 版本):
      ESP32 开发板                    外围器件
    +--------------+                +------------------+
    |              |                | 1.54 TFT (ST7789)|
    |          GND |----------------| GND              |
    |         3.3V |----------------| VCC              |
    |       GPIO18 |----------------| SCL (SCK)        |
    |       GPIO23 |----------------| SDA (MOSI)       |
    |       GPIO15 |----------------| RES (RST)        |
    |        GPIO2 |----------------| DC               |
    |        GPIO5 |----------------| CS               |
    |         3.3V |----------------| BLK (背光)        |
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

namespace SmartHubTft
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

    // TFT 引脚 (同步 TftTest 的成功配置)
    const int TFT_CS = 5;
    const int TFT_DC = 2;
    const int TFT_RST = 15;

    // 实例化对象
    Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
    U8G2_FOR_ADAFRUIT_GFX u8g2_gfx;
    DHT dht(DHT_PIN, DHT11);

    // 变量
    float temperature = 0;
    float humidity = 0;
    int lightLevel = 0;
    int threshold = 0;
    bool displayMode = 0;        // 0: 环境数据, 1: 系统状态
    bool needsFullRedraw = true; // 标记是否需要全屏刷新
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
        Serial.println("SmartHubTft 初始化开始...");

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

        // 初始化 TFT
        Serial.println("正在初始化 ST7789 (同步 TftTest 配置)...");
        tft.init(240, 240);      // 使用 TftTest 的默认初始化
        tft.invertDisplay(true); // 使用 TftTest 的反转设置

        // 初始化 U8g2 字体支持
        u8g2_gfx.begin(tft);
        u8g2_gfx.setFontMode(0);                    // 设置为不透明模式 (0)，避免闪烁
        u8g2_gfx.setBackgroundColor(ST77XX_BLACK);  // 设置字体背景为黑色
        u8g2_gfx.setFontDirection(0);               // 水平
        u8g2_gfx.setFont(u8g2_font_wqy16_t_gb2312); // 使用 16 像素中文字体

        delay(200);
        tft.setRotation(0);

        // 闪烁测试：确认通信是否正常
        Serial.println("执行颜色填充测试...");
        tft.fillScreen(ST77XX_RED);
        delay(500);
        tft.fillScreen(ST77XX_BLACK);

        // 欢迎界面
        u8g2_gfx.setCursor(40, 100);
        u8g2_gfx.setForegroundColor(ST77XX_WHITE);
        u8g2_gfx.print("智能管家启动中...");
        u8g2_gfx.setCursor(60, 130);
        u8g2_gfx.print("请稍候...");
        Serial.println("SmartHubTft 初始化完成");
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
                needsFullRedraw = true; // 切换模式时标记需要全屏刷新
                Serial.println("切换显示模式");
            }
        }

        // 2. 定时读取传感器 (每 1 秒)
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= 1000 || needsFullRedraw)
        {
            if (!needsFullRedraw)
                lastUpdateTime = currentTime;

            temperature = dht.readTemperature();
            humidity = dht.readHumidity();
            lightLevel = analogRead(LDR_PIN);
            threshold = analogRead(POT_PIN);

            // 3. 逻辑处理：报警与 LED 颜色
            if (lightLevel > threshold)
            {
                digitalWrite(BUZZER_PIN, HIGH);
                setRGB(255, 0, 0); // 红色警告
            }
            else
            {
                digitalWrite(BUZZER_PIN, LOW);
                if (temperature < 20)
                    setRGB(0, 0, 255); // 蓝色 (冷)
                else if (temperature < 28)
                    setRGB(0, 255, 0); // 绿色 (舒适)
                else
                    setRGB(255, 165, 0); // 橙色 (热)
            }

            // 4. 刷新 TFT
            if (needsFullRedraw)
            {
                tft.fillScreen(ST77XX_BLACK);
                needsFullRedraw = false;
            }

            if (displayMode == 0)
            {
                // 模式 0: 环境数据
                u8g2_gfx.setForegroundColor(ST77XX_CYAN);
                u8g2_gfx.setCursor(40, 30);
                u8g2_gfx.print("--- 环境监测 ---");

                u8g2_gfx.setForegroundColor(ST77XX_WHITE);
                u8g2_gfx.setCursor(10, 70);
                u8g2_gfx.printf("温度: %.1f °C  ", temperature); // 末尾加空格防止残影
                u8g2_gfx.setCursor(10, 110);
                u8g2_gfx.printf("湿度: %.1f %%  ", humidity);
                u8g2_gfx.setCursor(10, 150);
                u8g2_gfx.printf("光照: %d    ", lightLevel);
            }
            else
            {
                // 模式 1: 系统状态
                u8g2_gfx.setForegroundColor(ST77XX_YELLOW);
                u8g2_gfx.setCursor(40, 30);
                u8g2_gfx.print("--- 系统状态 ---");

                u8g2_gfx.setForegroundColor(ST77XX_WHITE);
                u8g2_gfx.setCursor(10, 70);
                u8g2_gfx.printf("报警阈值: %d    ", threshold);

                u8g2_gfx.setCursor(10, 110);
                if (lightLevel > threshold)
                {
                    u8g2_gfx.setForegroundColor(ST77XX_RED);
                    u8g2_gfx.print("状态: 警告!  ");
                }
                else
                {
                    u8g2_gfx.setForegroundColor(ST77XX_GREEN);
                    u8g2_gfx.print("状态: 正常  ");
                }

                u8g2_gfx.setForegroundColor(ST77XX_WHITE);
                u8g2_gfx.setCursor(10, 150);
                u8g2_gfx.printf("运行时间: %lu s  ", millis() / 1000);
            }

            // 串口调试
            Serial.printf("T:%.1f H:%.1f L:%d Th:%d\n", temperature, humidity, lightLevel, threshold);
        }
    }
} // namespace SmartHubTft
