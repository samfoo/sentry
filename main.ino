#define DEBUG

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
    pan.attach(10);
    tilt.attach(11);

    LOG("Waiting for commands...\n");
    LOG("--- ready ---\n");
}

void loop() {
    Command command = readCommand();

    switch (command) {
        case NO_DATA: break;
        default:
            execute(command, sentry);
            break;
    }
}
