#include <Servo.h>

#include "log.h"

Servo left;
Servo right;

void calibrate() {
    LOG("Writing 2000us");
    left.writeMicroseconds(2000);
    right.writeMicroseconds(2000);

    LOG("Waiting for go-ahead to calibrate");
    while (true) {
        if (Serial.available() > 0) {
            Serial.println("Done with highest");
            delay(100);
            break;
        }
    }

    delay(2000);

    LOG("Writing 1000us");
    left.writeMicroseconds(1000);
    right.writeMicroseconds(1000);

    delay(1000);

    LOG("Done!");
}

void setup() {
    Serial.begin(9600);

    left.attach(8);
    right.attach(9);

    calibrate();
}

