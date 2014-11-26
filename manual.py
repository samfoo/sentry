import io
import sys
import time

import sentry

import curses
stdscr = curses.initscr()
curses.noecho()
curses.cbreak()
stdscr.keypad(1)

if __name__ == "__main__":
    gun = sentry.Sentry()

    while True:
        key = stdscr.getch()

        if key == curses.KEY_RIGHT:
            gun.pan_right()
        elif key == curses.KEY_LEFT:
            gun.pan_left()
        elif key == curses.KEY_UP:
            gun.tilt_down()
        elif key == curses.KEY_DOWN:
            gun.tilt_up()
        elif key == ord('='):
            gun.throttle_up()
        elif key == ord('-'):
            gun.throttle_down()

    gun.ser.close()

