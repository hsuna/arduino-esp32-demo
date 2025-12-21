#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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
    |              |                +------------------+
    |              |                | DHT11 传感器      |
    |         3.3V |----------------| (中间引脚)        |
    |          GND |----------------| -                |
    |       GPIO13 |----------------| S                |
    +--------------+                +------------------+
*/

namespace OledTemp {
    Adafruit_SSD1306 display(128, 64, &Wire, -1);

    DHT dht(13, DHT11); // DHT11 传感器连接到 GPIO13

    unsigned long lastUpdateTime = 0;

    void init() {
        Serial.println("OledTemp 初始化...");
        
        // 初始化 I2C 总线 (GPIO21=SDA, GPIO22=SCK)
        Wire.begin(21, 22);
        Wire.setClock(100000); // 设置为 100kHz 提高稳定性

        // 初始化 DHT 传感器
        dht.begin();

        // 初始化 OLED 显示屏
        if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // I2C 地址 0x3C
            Serial.println(F("SSD1306 初始化失败"));
            for(;;); // 停止运行
        }

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println("System Ready");
        display.display();
        delay(1000);
    }

    void update() {
        unsigned long currentTime = millis();
        
        // 每 2 秒读取并更新一次
        if (currentTime - lastUpdateTime >= 2000) {
            lastUpdateTime = currentTime;

            float h = dht.readHumidity();
            float t = dht.readTemperature();

            if (isnan(h) || isnan(t)) {
                Serial.println(F("读取 DHT 传感器失败!"));
                display.clearDisplay();
                display.setCursor(0,0);
                display.println("Sensor Error!");
                display.display();
                return;
            }

            // 串口输出
            Serial.print(F("湿度: "));
            Serial.print(h);
            Serial.print(F("%  温度: "));
            Serial.print(t);
            Serial.println(F("°C"));

            // OLED 显示
            display.clearDisplay();
            display.setTextSize(1);
            display.setCursor(0,0);
            display.println("Temp & Humidity");
            
            display.setTextSize(2);
            display.setCursor(0,20);
            display.print("T: ");
            display.print(t);
            display.print(" C");

            display.setCursor(0,45);
            display.print("H: ");
            display.print(h);
            display.print(" %");

            display.display();
        }
    }
} // namespace OledTemp
