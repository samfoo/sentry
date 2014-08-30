#include <Servo.h>

#ifndef SENTRY_H
#define SENTRY_H

#define MAX_THROTTLE 2000
#define MIN_THROTTLE 1000

class Sentry {
    Servo left;
    Servo right;
    Servo panner;
    Servo tilter;

    uint16_t _throttle;
    uint8_t _pan;
    uint8_t _tilt;

public:
    Sentry(Servo& l, Servo& r, Servo& p, Servo& t);

    void pan(uint8_t p);
    void tilt(uint8_t t);
    void throttle(uint16_t t);
};

#endif
