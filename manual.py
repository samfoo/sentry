import io
import sys
import time
import serial

import curses
stdscr = curses.initscr()
curses.cbreak()
stdscr.keypad(1)

class Sentry:
    def __init__(self):
        self.ser = serial.Serial('/dev/ttyACM0', 9600)

        while True:
            line = self.ser.readline().strip()
            print line
            if line == "--- ready ---":
                break

        self.ser.write("-90 ")
        self.ser.write("|90 ")

        self.pan = 90
        self.tilt = 90

    def _pan(self):
        self.pan = min(180, self.pan)
        self.pan = max(0, self.pan)

        self.ser.write("-%d " % self.pan)

    def _tilt(self):
        self.tilt = min(180, self.tilt)
        self.tilt = max(0, self.tilt)

        self.ser.write("|%d " % self.tilt)

    def pan_right(self, coeff=5):
        self.pan -= coeff
        self._pan()

    def pan_left(self, coeff=5):
        self.pan += coeff
        self._pan()

    def tilt_down(self, coeff=4):
        self.tilt -= coeff
        self._tilt()

    def tilt_up(self, coeff=4):
        self.tilt += coeff
        self._tilt()

if __name__ == "__main__":
    sentry = Sentry()

    while True:
        key = stdscr.getch()

        if key == curses.KEY_RIGHT:
            sentry.pan_right()
        elif key == curses.KEY_LEFT:
            sentry.pan_left()
        elif key == curses.KEY_UP:
            sentry.tilt_down()
        elif key == curses.KEY_DOWN:
            sentry.tilt_up()

    sentry.ser.close()

