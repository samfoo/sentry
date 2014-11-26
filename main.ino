#define DEBUG

#include <Servo.h>

#include "sentry.h"
#include "log.h"

Servo left;
Servo right;
Servo pan;
Servo tilt;
Sentry sentry = Sentry(left, right, pan, tilt);

#define  NO_DATA   -1
#define  NONE      0
#define  PING      '!'
#define  PONG      '?'
#define  PAN       '-'
#define  TILT      '|'
#define  THROTTLE  '='

void setup() {
    Serial.begin(9600);

    left.attach(8);
    right.attach(9);
    pan.attach(10);
    tilt.attach(11);

    sentry.throttle(MIN_THROTTLE);
    sentry.pan(NEUTRAL_PAN_TILT);
    sentry.tilt(NEUTRAL_PAN_TILT);
}

int getCommand() {
    if (Serial.available() > 0) {
        int in = Serial.read();
        if (in == NO_DATA) return NONE;

        switch(in) {
            case PING:
                Serial.write(PONG);
                break;
            case PAN:
            case TILT:
            case THROTTLE:
                return in;
        }
    }

    return NONE;
}

int getArgument() {
    char in[4];
    uint32_t *val = (uint32_t*)in;

    while (true) {
        if (Serial.available() >= 4) {
            Serial.readBytes(in, 4);

            return *val;
        }
    }
}

void loop() {
    int command = getCommand();

    int arg = NO_DATA;
    if (command == PAN ||
        command == TILT ||
        command == THROTTLE) {
        arg = getArgument();
    }

    switch (command) {
        case PAN:
            sentry.pan(arg);
            break;

        case TILT:
            sentry.tilt(arg);
            break;

        case THROTTLE:
            sentry.throttle(arg);
            break;
    }
}
