#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "WifiTest.h"
#include "../secrets.h"

/*
电路图:
ESP32 正常供电 (USB 或 3.3V/GND)
*/
namespace WifiTest {
    const char* SSID = WIFI_SSID;
    const char* PASSWORD = WIFI_PASSWORD;

    int sensorValue = 0; // 存储模拟值
    int ledBrightness = 0; // 存储 LED 亮度值

    void init() {
        delay(1000);

        // 连接 Wi-Fi
        Serial.println("Connecting to WiFi...");
        WiFi.begin(SSID, PASSWORD);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }

        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }

    void update() {
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;

            // 配置 HTTP 请求
            http.begin("http://www.baidu.com");  // 注意：是 http，不是 https（ESP32 默认不支持 HTTPS 证书验证）

            // 发送 GET 请求
            int httpResponseCode = http.GET();

            if (httpResponseCode > 0) {
            Serial.printf("HTTP Response code: %d\n", httpResponseCode);
            String payload = http.getString();
            Serial.println("Response from Baidu:");
            Serial.println(payload);  // 输出 HTML 内容（可能很长）
            } else {
            Serial.printf("Error on HTTP request: %s\n", http.errorToString(httpResponseCode).c_str());
            }

            // 关闭连接
            http.end();

            // 等待 30 秒再请求一次（避免频繁请求）
            delay(30000);
        }
    }
} // namespace WifiTest