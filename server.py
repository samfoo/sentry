import socket
import struct
import sys
import serial

import sentry

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

s.bind(("", 7997))

print "listening on 7997"

def throttle():
    pass

def pan(gun, v):
    gun.pan = v
    gun._pan()

def tilt(gun, v):
    gun.tilt = v
    gun._tilt()

def server(sock):
    gun = sentry.Sentry()

    while True:
        data, addr = sock.recvfrom(128)

        command, arg = struct.unpack("cI", data)

        commands = {
            "-": pan,
            "|": tilt,
            "=": throttle
        }

        commands[command](gun, arg)

server(s)
