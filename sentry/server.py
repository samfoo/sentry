import socket
import struct
import sys
import serial

import sentry

MAX_SPEED = 20

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

s.bind(("", 7997))

print "listening on 7997"

def throttle():
    pass

current_pan = 90
current_tilt = 90

def pan(gun, v):
    global current_pan
    vel = abs(current_pan - v)
    if v < current_pan:
        current_pan -= min(MAX_SPEED, vel)
    else:
        current_pan += min(MAX_SPEED, vel)

    gun.pan_to(current_pan)

def tilt(gun, v):
    global current_tilt
    vel = abs(current_tilt - v)
    if v < current_tilt:
        current_tilt -= min(MAX_SPEED, vel)
    else:
        current_tilt += min(MAX_SPEED, vel)

    gun.tilt_to(current_tilt)

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
