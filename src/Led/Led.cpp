#include <Arduino.h>
#include "Led.h"

namespace Led {
    int RED_PIN = 25;
    int GREEN_PIN = 27;
    int YELLOW_PIN = 12;

    void init() {
        // 设定引脚为输出模式
        pinMode(RED_PIN, OUTPUT);
        pinMode(GREEN_PIN, OUTPUT);
        pinMode(YELLOW_PIN, OUTPUT);
    }

    void update() {
        delay(1000);
        digitalWrite(YELLOW_PIN, LOW);
        digitalWrite(RED_PIN, HIGH);
        delay(1000);
        digitalWrite(RED_PIN, LOW);
        digitalWrite(GREEN_PIN, HIGH);
        delay(1000);
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(YELLOW_PIN, HIGH);
    }
} // namespace Led