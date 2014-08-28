#include <Servo.h>

Servo motor1;
Servo motor2;

void calibrate() {
    Serial.println("Writing 2000us");
    motor1.writeMicroseconds(2000);
    motor2.writeMicroseconds(2000);

    Serial.println("Waiting for go-ahead to calibrate");
    while (true) {
        if (Serial.available() > 0) {
            Serial.println("Done with highest");
            delay(100);
            break;
        }
    }

    delay(2000);

    Serial.println("Writing 1000us");
    motor1.writeMicroseconds(1000);
    motor2.writeMicroseconds(1000);
    delay(1000);

    Serial.println("Done!");
}

void startup() {
    Serial.println("Writing 1000us");
    motor1.writeMicroseconds(1000);
    motor2.writeMicroseconds(1000);

    Serial.println("Waiting for go-ahead to increase throttle");
    while (true) {
        if (Serial.available() > 0) {
            Serial.println("Go-ahead received!");
            delay(100);
            break;
        }
    }

    for (int i=0; i < 6; i++) {
        int throttle = 1000 + 100 * i;

        Serial.print("Increasing throttle to ");
        Serial.print(throttle, DEC);
        Serial.print("\r\n");

        motor1.writeMicroseconds(throttle);
        motor2.writeMicroseconds(throttle);

        delay(3000);
    }

    delay(10000);

    Serial.println("Idling...");
    motor1.writeMicroseconds(1000);
    motor2.writeMicroseconds(1000);
}

void setup()
{
    Serial.begin(9600);
    motor1.attach(8);
    motor2.attach(9);

    startup();
    //calibrate();
}

void loop()
{
}
