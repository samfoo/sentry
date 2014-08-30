#define DEBUG true

#include <string.h>
#include <stdio.h>

#include <Servo.h>

#include "sentry.h"
#include "command.h"
#include "log.h"

Servo left;
Servo right;

void setThrottle(Servo & motor, int throttle) {
    motor.writeMicroseconds(throttle);
}

void startup(Servo & left, Servo & right) {
    LOG("Writing 1000us (minimum throttle) to prime ESC's");

    setThrottle(left, 1000);
    setThrottle(right, 1000);

    LOG("Waiting for commands...\n");
}

void helpText() {
    LOG("Welcome to Sentry Gun 0.1!");
    LOG("  \"Preparing to dispense product\" - Aperture Science Turret\n");
    LOG("Available serial commands\n");
    LOG("\t%25s %c[packed unsigned int]\n\n", "Set throttle", THROTTLE);
}

void setup() {
    Serial.begin(9600);

    left.attach(8);
    right.attach(9);

    helpText();

    startup(left, right);
}

inline Command readCommand() {
    Command command = Serial.read();

    return command;
}

int readThrottle() {
    int throttle = 0;

#if DEBUG
    do {
#endif
        throttle = Serial.parseInt();
#if DEBUG
    } while (throttle == 0);
#endif

    return throttle;
}

inline void execute(Command c) {
    switch (c) {
        case THROTTLE:
        {
            int throttle = readThrottle();

            if (throttle > 0 && throttle <= 1000) {
                LOG("throttle := %d (%d usec PWM)", throttle, throttle + 1000);

                setThrottle(left, throttle + 1000);
                setThrottle(right, throttle + 1000);
            }
            else {
                ERR("INVALID throttle := %d (must be 0 > t <= 1000)", throttle);
            }

            break;
        }

        default:
            LOG("No handler for command '0x%02x'/'%c'", c, c);
            break;
    }
}

void loop() {
    Command command = readCommand();

    switch (command) {
        case NO_DATA: break;
        default:
            execute(command);
            break;
    }
}
