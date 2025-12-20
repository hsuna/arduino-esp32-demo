#include "Led/Led.h"

void setup()
{
    Serial.begin(115200); // 初始化串口，波特率 115200
    Serial.println("ESP32 启动成功！");
    Led::init();
}

void loop()
{
    Led::update();
}