#include <Arduino.h>
#include "Led/Led.h"
#include "Button/Button.h"
#include "Pwm/Pwm.h"
#include "Ledc/Ledc.h"
#include "Adc/Adc.h"
#include "Adc2/Adc2.h"
#include "WifiTest/WifiTest.h"
#include "HeartBratTest/HeartBratTest.h"

void setup() {
  Serial.begin(115200); // 初始化串口，波特率 115200
  Serial.println("ESP32 启动成功！");
  // WifiTest::init();
  HeartBratTest::init();
}

void loop() {
  // WifiTest::update();
  HeartBratTest::update();
}
