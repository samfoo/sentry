#define DEBUG true

#include <string.h>
#include <stdio.h>

#include <Servo.h>

#include "sentry.h"
#include "command.h"
#include "log.h"

Servo left;
Servo right;
Servo pan;
Servo tilt;
Sentry sentry = Sentry(left, right, pan, tilt);

void helpText() {
    LOG("Welcome to Sentry Gun 0.1!");
    LOG("  \"Preparing to dispense product\" - Aperture Science Turret\n");
    LOG("Available serial commands\n");
    LOG("\t%25s %c[packed unsigned int]\n\n", "Set throttle", THROTTLE);
}

void setup() {
    Serial.begin(9600);

    helpText();

    left.attach(8);
    right.attach(9);

    LOG("Waiting for commands...\n");
}

inline Command readCommand() {
    Command command = Serial.read();

    return command;
}

int readInt() {
    int i = 0;

#if DEBUG
    do {
#endif
        i = Serial.parseInt();
#if DEBUG
    } while (i == 0);
#endif

    // Throw away the next character, it's just a separator to be
    // able to parse the integer
    Serial.read();

    return i;
}

inline void execute(Command c) {
    switch (c) {
        case THROTTLE:
        {
            int throttle = readInt();
            sentry.throttle(throttle);

            break;
        }

        case PAN:
        {
            int degrees = readInt();
            sentry.pan(degrees);

            break;
        }

        case TILT:
        {
            int degrees = readInt();
            sentry.tilt(degrees);

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
