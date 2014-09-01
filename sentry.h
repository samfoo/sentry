#include <Servo.h>

#ifndef SENTRY_H
#define SENTRY_H

#define  MIN_THROTTLE      0
#define  MAX_THROTTLE      1000
#define  MIN_PAN_TILT      0
#define  NEUTRAL_PAN_TILT  90
#define  MAX_PAN_TILT      180

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
