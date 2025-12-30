#include <Arduino.h>
#include "JoystickTest.h"

/*
电路图 (摇杆测试):
      ESP32 开发板                    外围器件
    +--------------+                +------------------+
    |              |                | 摇杆 (Joystick)   |
    |       GPIO34 |----------------| VRx (X轴)        |
    |       GPIO32 |----------------| VRy (Y轴)        |
    |       GPIO14 |----------------| SW (按键)         |
    |           5V |----------------| VCC (必须接5V)    |
    |          GND |----------------| GND              |
    +--------------+                +------------------+
*/

namespace JoystickTest
{
    // 引脚定义 (参考 SmartHub.cpp)
    const int JOY_X_PIN = 34;  // 摇杆 X 轴
    const int JOY_Y_PIN = 32;  // 摇杆 Y 轴
    const int JOY_SW_PIN = 14; // 摇杆按键

    void init()
    {
        Serial.begin(115200);
        pinMode(JOY_X_PIN, INPUT);
        pinMode(JOY_Y_PIN, INPUT);
        pinMode(JOY_SW_PIN, INPUT_PULLUP);

        Serial.println("========================================");
        Serial.println("摇杆测试程序启动");
        Serial.println("请移动摇杆并按下按键查看输出");
        Serial.println("========================================");
    }

    void update()
    {
        int xVal = analogRead(JOY_X_PIN);
        int yVal = analogRead(JOY_Y_PIN);
        int swVal = digitalRead(JOY_SW_PIN);

        // 打印原始值
        Serial.print("X: ");
        Serial.print(xVal);
        Serial.print(" | Y: ");
        Serial.print(yVal);
        Serial.print(" | SW: ");
        Serial.println(swVal == LOW ? "按下" : "松开");

        // 简单的方向判断
        if (xVal < 500)
            Serial.println("  [方向: 左]");
        else if (xVal > 3500)
            Serial.println("  [方向: 右]");

        if (yVal < 500)
            Serial.println("  [方向: 上]");
        else if (yVal > 3500)
            Serial.println("  [方向: 下]");

        delay(100); // 采样频率
    }
}
