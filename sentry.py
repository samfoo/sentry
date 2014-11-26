import serial

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
        self.throttle = 0

    def _pan(self):
        self.pan = min(180, self.pan)
        self.pan = max(0, self.pan)

        self.ser.write("-%d " % self.pan)

    def _tilt(self):
        self.tilt = min(180, self.tilt)
        self.tilt = max(0, self.tilt)

        self.ser.write("|%d " % self.tilt)

    def _throttle(self):
        self.throttle = min(200, self.throttle)
        self.tilt = max(0, self.throttle)

        self.ser.write("=%d " % self.throttle)

    def pan_to(self, deg):
        self.pan = deg
        self._pan()

    def tilt_to(self, deg):
        self.tilt = deg
        self._tilt()

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

    def throttle_down(self, coeff=100):
        self.throttle -= coeff
        self._throttle()

    def throttle_up(self, coeff=100):
        self.throttle += coeff
        self._throttle()

