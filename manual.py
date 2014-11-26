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
    stdscr.addstr(0, 0, "connecting")
    gun = sentry.Sentry()

    i = 0
    while True:
        i += 1
        stdscr.addstr(1, 0, "{pan: %d, tilt: %d)" % (gun.pan, gun.tilt))

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

        stdscr.refresh()

    gun.ser.close()

