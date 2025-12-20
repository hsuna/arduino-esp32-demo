#include <Arduino.h>
#include "Led/Led.cpp"
#include "Button/Button.cpp"
#include "Pwm/Pwm.cpp"
#include "Ledc/Ledc.cpp"
#include "Adc/Adc.cpp"
#include "Adc2/Adc2.cpp"

void setup()
{
    Serial.begin(115200); // 初始化串口，波特率 115200
    Serial.println("ESP32 启动成功！");
    Adc2::init();
}

void loop()
{
    Adc2::update();
}