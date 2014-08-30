#include <HardwareSerial.h>

#include "sentry.h"
#include "log.h"

Sentry::Sentry(Servo& l, Servo& r, Servo& p, Servo& t) {
    left = l;
    right = r;
    panner = p;
    tilter = t;

    throttle(0);
    pan(90);
    tilt(90);
}

void Sentry::pan(uint8_t p) {
    if (p >= 0 && p <= 180) {
        _pan = p;
        LOG("pan := %d degrees", t);
        panner.write(p);
    }
    else {
        ERR("invalid pan := %d (must be 0 <= d <= 180)", p);
    }
}

void Sentry::tilt(uint8_t t) {
    if (t >= 0 && t <= 180) {
        _tilt = t;
        LOG("tilt := %d degrees", t);
        tilter.write(t);
    }
    else {
        ERR("invalid tilt := %d (must be 0 <= d <= 180)", t);
    }
}

void Sentry::throttle(uint16_t t) {
    if (t >= 0 && t <= 1000) {
        _throttle = t;
        LOG("throttle := %d (%d usec PWM)", _throttle, _throttle + 1000);

        left.writeMicroseconds(_throttle + 1000);
        right.writeMicroseconds(_throttle + 1000);
    }
    else {
        ERR("invalid throttle := %d (must be 0 <= t <= 1000)", t);
    }
}
