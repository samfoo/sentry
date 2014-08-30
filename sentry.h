#define MAX_THROTTLE 2000
#define MIN_THROTTLE 1000

#include "log.h"

class Sentry {
    Servo left;
    Servo right;
    Servo panner;
    Servo tilter;

    uint16_t _throttle;
    uint8_t _pan_angle;
    uint8_t _tilt_angle;

public:

    Sentry(Servo& l, Servo& r, Servo& p, Servo& t) {
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

    void pan(uint8_t p) {
        if (p >= 0 && p <= 180) {
            LOG("TODO: set pan to %d degrees", p);
        }
        else {
            ERR("invalid pan := %d (must be 0 <= d <= 180)", p);
        }
    }

    void tilt(uint8_t t) {
        if (t >= 0 && t <= 180) {
            LOG("TODO: set tilt to %d degrees", t);
        }
        else {
            ERR("invalid tilt := %d (must be 0 <= d <= 180)", t);
        }
    }

    void throttle(uint16_t t) {
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
};
