#include <Arduino.h>
#include "Led.h"

namespace Led {
    int red_pin = 25;
    int green_pin = 27;
    int yellow_pin = 12;

    void init() {
        // 设定引脚为输出模式
        pinMode(red_pin, OUTPUT);
        pinMode(green_pin, OUTPUT);
        pinMode(yellow_pin, OUTPUT);
    }

    void update() {
        delay(1000);
        digitalWrite(yellow_pin, LOW);
        digitalWrite(red_pin, HIGH);
        delay(1000);
        digitalWrite(red_pin, LOW);
        digitalWrite(green_pin, HIGH);
        delay(1000);
        digitalWrite(green_pin, LOW);
        digitalWrite(yellow_pin, HIGH);
    }
} // namespace Led