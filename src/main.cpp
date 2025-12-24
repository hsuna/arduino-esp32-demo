#include <Arduino.h>
#include "Led/Led.h"
#include "Button/Button.h"
#include "Pwm/Pwm.h"
#include "Ledc/Ledc.h"
#include "Adc/Adc.h"
#include "Adc2/Adc2.h"
#include "WifiTest/WifiTest.h"
#include "HeartBratTest/HeartBratTest.h"
#include "Exti/Exti.h"
#include "Timeout/Timeout.h"
#include "OledTemp/OledTemp.h"
#include "SmartMonitor/SmartMonitor.h"
#include "SmartHub/SmartHub.h"
#include "SmartHubTft/SmartHubTft.h"
#include "TftTest/TftTest.h"

void setup()
{
  Serial.begin(115200); // 初始化串口，波特率 115200
  Serial.println("ESP32 启动成功！");
  SmartHubTft::init();
}

void loop()
{
  SmartHubTft::update();
}
