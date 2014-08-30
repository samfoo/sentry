#include <HardwareSerial.h>

#include "sentry.h"
#include "command.h"
#include "log.h"

Command readCommand() {
    Command command = Serial.read();

    return command;
}

int readInt() {
    int i = 0;

#ifdef DEBUG
    do {
#endif
        i = Serial.parseInt();
#ifdef DEBUG
    } while (i == 0);
#endif

    // Throw away the next character, it's just a separator to be
    // able to parse the integer
    Serial.read();

    return i;
}

void execute(Command c, Sentry & sentry) {
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

