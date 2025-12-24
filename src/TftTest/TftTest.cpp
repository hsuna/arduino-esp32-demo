#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "TftTest.h"

/*
电路图:
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
*/

namespace TftTest
{
    const int TFT_CS = 5;
    const int TFT_DC = 2;
    const int TFT_RST = 15;

    // 实例化对象
    Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

    void init()
    {
        Serial.begin(115200);
        Serial.println("--- TFT 硬件测试开始 ---");

        // 初始化屏幕
        // 尝试不同的初始化参数，1.54 寸通常是 240x240
        tft.init(240, 240);

        // 某些模块需要反转颜色才能正常显示
        tft.invertDisplay(true);

        tft.setRotation(0);
        tft.fillScreen(ST77XX_BLACK);

        Serial.println("初始化完成，开始颜色循环...");
    }

    void drawRobot(bool blink, bool smile)
    {
        // 绘制头部
        tft.fillRoundRect(40, 40, 160, 160, 20, ST77XX_CYAN);
        tft.drawRoundRect(40, 40, 160, 160, 20, ST77XX_WHITE);

        // 绘制眼睛
        if (blink)
        {
            // 闭眼 (直线)
            tft.fillRect(70, 95, 30, 5, ST77XX_BLACK);
            tft.fillRect(140, 95, 30, 5, ST77XX_BLACK);
        }
        else
        {
            // 开眼 (圆圈)
            tft.fillCircle(85, 100, 15, ST77XX_WHITE);
            tft.fillCircle(85, 100, 7, ST77XX_BLUE);
            tft.fillCircle(155, 100, 15, ST77XX_WHITE);
            tft.fillCircle(155, 100, 7, ST77XX_BLUE);
        }

        // 绘制嘴巴
        if (smile)
        {
            // 微笑 (圆弧模拟)
            tft.fillRoundRect(80, 150, 80, 10, 5, ST77XX_RED);
        }
        else
        {
            // 惊讶 (圆圈)
            tft.fillCircle(120, 160, 10, ST77XX_BLACK);
        }
    }

    void update()
    {
        static bool state = false;
        tft.fillScreen(ST77XX_BLACK);

        // 绘制卡通机器人
        drawRobot(false, state);
        tft.setCursor(60, 220);
        tft.setTextColor(ST77XX_MAGENTA);
        tft.setTextSize(2);
        tft.print(state ? "I'm Happy!" : "Oh! Hello!");
        delay(1500);

        // 眨眼动画
        tft.fillScreen(ST77XX_BLACK);
        drawRobot(true, state);
        delay(200);

        state = !state;
    }
}
