#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <WiFi.h>
#include "time.h"
#include "secrets.h"
#include "SmartHub.h"

/*
电路图 (SmartHub 交互终端):
      ESP32 开发板                    外围器件
    +--------------+                +------------------+
    |              |                | SSD1306 OLED     |
    |       GPIO21 |----------------| SDA              |
    |       GPIO22 |----------------| SCL              |
    |         3.3V |----------------| VCC              |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | HC-SR04 超声波    |
    |        GPIO5 |----------------| Trig             |
    |       GPIO18 |----------------| Echo             |
    |           5V |----------------| VCC (必须接5V)    |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | DHT11 温湿度      |
    |       GPIO13 |----------------| S (信号)          |
    |         3.3V |----------------| (中间引脚)        |
    |          GND |----------------| -                |
    +--------------+                +------------------+
    |              |                | 光敏电阻 (LDR)    |
    |       GPIO35 |-------+--------| S (信号)          |
    |              |     [10kΩ]     |                  |
    |          GND |-------+        |                  |
    |         3.3V |----------------| VCC              |
    +--------------+                +------------------+
    |              |                | 摇杆 (Joystick)   |
    |       GPIO34 |----------------| VRx (X轴)        |
    |       GPIO32 |----------------| VRy (Y轴)        |
    |       GPIO14 |----------------| SW (按键)         |
    |           5V |----------------| VCC (必须接5V)    |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | 电位器 (Threshold)|
    |       GPIO33 |----------------| S (信号)          |
    |         3.3V |----------------| VCC              |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | 蜂鸣器 (Buzzer)   |
    |       GPIO12 |----------------| S (信号)          |
    |         3.3V |----------------| VCC              |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
    |              |                | RGB LED (共阴)    |
    |        GPIO4 |--- [220Ω] ---| R                |
    |       GPIO16 |--- [220Ω] ---| G                |
    |       GPIO17 |--- [220Ω] ---| B                |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
*/

namespace SmartHub
{
    // 引脚定义
    const int TRIG_PIN = 5;
    const int ECHO_PIN = 18;
    const int DHT_PIN = 13;
    const int LDR_PIN = 35;
    const int JOY_X_PIN = 34;  // 摇杆 X 轴
    const int JOY_Y_PIN = 32;  // 摇杆 Y 轴
    const int JOY_SW_PIN = 14; // 摇杆按键
    const int POT_PIN = 33;    // 电位器引脚 (调节报警距离)
    const int BUZZER_PIN = 12;
    const int RGB_R_PIN = 4;
    const int RGB_G_PIN = 16;
    const int RGB_B_PIN = 17;

    // NTP 服务器设置
    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 8 * 3600; // 中国时区 (UTC+8)
    const int daylightOffset_sec = 0;

    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/22, /* data=*/21);
    DHT dht(DHT_PIN, DHT11);

    // 状态变量
    int currentMenu = 0;
    float temp = 0, hum = 0;
    int light = 0, dist = 0;
    int alarmThreshold = 20; // 报警距离阈值
    bool alarmActive = false;
    unsigned long lastSenseTime = 0;
    unsigned long lastMenuMoveTime = 0; // 记录上次摇杆移动时间，防止切换过快

    // 获取超声波距离
    int getDistance()
    {
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);
        long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms 超时
        return duration * 0.034 / 2;
    }

    void setRGB(int r, int g, int b)
    {
        analogWrite(RGB_R_PIN, r);
        analogWrite(RGB_G_PIN, g);
        analogWrite(RGB_B_PIN, b);
    }

    void init()
    {
        pinMode(TRIG_PIN, OUTPUT);
        pinMode(ECHO_PIN, INPUT);
        pinMode(LDR_PIN, INPUT);
        pinMode(JOY_X_PIN, INPUT);
        pinMode(JOY_Y_PIN, INPUT);
        pinMode(JOY_SW_PIN, INPUT_PULLUP);
        pinMode(POT_PIN, INPUT);
        pinMode(BUZZER_PIN, OUTPUT);
        pinMode(RGB_R_PIN, OUTPUT);
        pinMode(RGB_G_PIN, OUTPUT);
        pinMode(RGB_B_PIN, OUTPUT);

        dht.begin();
        u8g2.begin();
        u8g2.enableUTF8Print();

        // 1. 连接 Wi-Fi 并同步时间
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.setCursor(10, 30);
        u8g2.print("正在连接 Wi-Fi...");
        u8g2.sendBuffer();

        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        int retry = 0;
        while (WiFi.status() != WL_CONNECTED && retry < 20)
        {
            delay(500);
            Serial.print(".");
            retry++;
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
            u8g2.setCursor(10, 50);
            u8g2.print("时间同步成功!");
        }
        else
        {
            u8g2.setCursor(10, 50);
            u8g2.print("Wi-Fi 连接失败");
        }
        u8g2.sendBuffer();
        delay(1000);

        // 2. 启动动画
        for (int i = 0; i <= 100; i += 10)
        {
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_wqy12_t_gb2312);
            u8g2.setCursor(30, 30);
            u8g2.print("X-Center 系统");
            u8g2.drawFrame(20, 45, 88, 8);
            u8g2.drawBox(22, 47, i * 0.84, 4);
            u8g2.sendBuffer();
            delay(50);
        }
    }

    void drawHeader(const char *title)
    {
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.setCursor(0, 12);
        u8g2.print(title);
        u8g2.drawLine(0, 15, 128, 15);
    }

    void update()
    {

        // 1. 菜单选择 (通过摇杆 X 轴)
        int xVal = analogRead(JOY_X_PIN);
        int yVal = analogRead(JOY_Y_PIN);
        if (millis() - lastMenuMoveTime > 300) // 300ms 冷却时间，防止菜单飞速切换
        {
            if (xVal < 1000 || yVal < 1000) // 摇杆向左推或上推
            {
                currentMenu--;
                if (currentMenu < 0)
                    currentMenu = 3;
                lastMenuMoveTime = millis();
            }
            else if (xVal > 3000 || yVal > 3000) // 摇杆向右推或下推
            {
                currentMenu++;
                if (currentMenu > 3)
                    currentMenu = 0;
                lastMenuMoveTime = millis();
            }
        }

        // 2. 传感器采样 (每 500ms)
        if (millis() - lastSenseTime > 500)
        {
            lastSenseTime = millis();
            temp = dht.readTemperature();
            hum = dht.readHumidity();
            light = analogRead(LDR_PIN);
            dist = getDistance();

            // 读取电位器并映射为报警距离 (5cm - 100cm)
            int potVal = analogRead(POT_PIN);
            alarmThreshold = map(potVal, 0, 4095, 5, 100);

            // 自动安防逻辑
            if (dist > 0 && dist < alarmThreshold)
            { // 距离小于阈值报警
                alarmActive = true;
                digitalWrite(BUZZER_PIN, HIGH);
                setRGB(255, 0, 0);
            }
            else
            {
                alarmActive = false;
                digitalWrite(BUZZER_PIN, LOW);
                setRGB(0, 255 * (light < 1000), 255 * (light >= 1000)); // 根据光照变色
            }
        }

        // 3. 渲染界面
        u8g2.clearBuffer();
        switch (currentMenu)
        {
        case 0: // 环境概览
        {
            drawHeader("1. 环境监测");
            u8g2.setCursor(0, 35);
            u8g2.printf("温度: %.1f C", temp);
            u8g2.setCursor(0, 55);
            u8g2.printf("湿度: %.1f %%", hum);

            // 获取并显示当前时间
            struct tm timeinfo;
            u8g2.drawCircle(105, 45, 18, U8G2_DRAW_ALL);
            u8g2.setFont(u8g2_font_6x10_tf);
            if (getLocalTime(&timeinfo))
            {
                u8g2.setCursor(92, 48);
                u8g2.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
            }
            else
            {
                u8g2.setCursor(92, 48);
                u8g2.print("--:--");
            }
            break;
        }

        case 1: // 距离雷达
        {
            drawHeader("2. 距离雷达");
            u8g2.setCursor(0, 30);
            u8g2.printf("当前距离: %d cm", dist);
            u8g2.setCursor(0, 42);
            u8g2.printf("报警阈值: %d cm", alarmThreshold);
            // 绘制距离条
            u8g2.drawFrame(0, 48, 128, 10);
            if (dist > 0)
            {
                int barWidth = map(constrain(dist, 2, 100), 2, 100, 124, 0);
                u8g2.drawBox(2, 50, barWidth, 6);
            }
            // 绘制阈值标记线
            int thresholdPos = map(constrain(alarmThreshold, 2, 100), 2, 100, 124, 0);
            u8g2.drawLine(2 + thresholdPos, 46, 2 + thresholdPos, 60);
            break;
        }

        case 2: // 光感控制
            drawHeader("3. 光感与灯光");
            u8g2.setCursor(0, 35);
            u8g2.printf("光照强度: %d", light);
            u8g2.setCursor(0, 55);
            u8g2.print(light < 1000 ? "状态: 黑暗 (开启夜灯)" : "状态: 明亮");
            break;

        case 3: // 系统设置/关于
            drawHeader("4. 系统信息");
            u8g2.setCursor(0, 35);
            u8g2.print("ESP32 核心: 240MHz");
            u8g2.setCursor(0, 55);
            u8g2.printf("运行时间: %lu s", millis() / 1000);
            break;
        }

        // 报警提示
        if (alarmActive)
        {
            u8g2.setFont(u8g2_font_wqy12_t_gb2312);
            u8g2.setCursor(80, 12);
            u8g2.print("![警告]");
        }

        u8g2.sendBuffer();
    }
}
