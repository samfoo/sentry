#include <HardwareSerial.h>

#include "sentry.h"
#include "log.h"

Sentry::Sentry(Servo& l, Servo& r, Servo& p, Servo& t) {
    left = l;
    right = r;
    panner = p;
    tilter = t;

    _throttle = 0;
    _pan_angle = 90;
    _tilt_angle = 90;

    LOG("Writing 1000us (minimum throttle) to prime ESC's");
    throttle(0);
}

void Sentry::pan(uint8_t p) {
    if (p >= 0 && p <= 180) {
        LOG("TODO: set pan to %d degrees", p);
    }
    else {
        ERR("invalid pan := %d (must be 0 <= d <= 180)", p);
    }
}

void Sentry::tilt(uint8_t t) {
    if (t >= 0 && t <= 180) {
        LOG("TODO: set tilt to %d degrees", t);
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
