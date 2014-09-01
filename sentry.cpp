#include <HardwareSerial.h>

#include "sentry.h"
#include "log.h"

Sentry::Sentry(Servo& l, Servo& r, Servo& p, Servo& t) {
    left = l;
    right = r;
    panner = p;
    tilter = t;

    throttle(MIN_THROTTLE);
    pan(NEUTRAL_PAN_TILT);
    tilt(NEUTRAL_PAN_TILT);
}

void Sentry::pan(uint8_t p) {
    if (p >= MIN_PAN_TILT && p <= MAX_PAN_TILT) {
        _pan = p;
        LOG("pan := %d degrees", t);
        panner.write(p);
    }
    else {
        ERR("invalid pan := %d (must be 0 <= d <= 180)", p);
    }
}

void Sentry::tilt(uint8_t t) {
    if (t >= MIN_PAN_TILT && t <= MAX_PAN_TILT) {
        _tilt = t;
        LOG("tilt := %d degrees", t);
        tilter.write(t);
    }
    else {
        ERR("invalid tilt := %d (must be 0 <= d <= 180)", t);
    }
}

void Sentry::throttle(uint16_t t) {
    if (t >= MIN_THROTTLE && t <= MAX_THROTTLE) {
        _throttle = t;
        LOG("throttle := %d (%d usec PWM)", _throttle, _throttle + 1000);

        left.writeMicroseconds(_throttle + 1000);
        right.writeMicroseconds(_throttle + 1000);
    }
    else {
        ERR("invalid throttle := %d (must be 0 <= t <= 1000)", t);
    }
}
