#!/usr/bin/env python3
#  -*- coding: utf-8 -*-
# File: beerme.py
# Author: Landon Bouma
# Last Modified: 2016.11.04
# Project Page: https://github.com/landonb/raugupatis
# License: GPLv3

import os
import sys

import optparse

# https://github.com/pyserial/pyserial
import serial

# See also:
#  /usr/lib/python2.7/dist-packages/serial/tools/list_ports.py
from serial.tools import list_ports_posix
#from serial.tools.list_ports_posix import comports

def trace(*args, **kwargs):
    if True:
        print(*args, **kwargs)

class Pibeer_Parser(optparse.OptionParser):

    def __init__(self):
        optparse.OptionParser.__init__(self)
        self.cli_opts = None
        self.cli_args = None

    def get_opts(self):
        self.prepare();
        self.parse();
        assert(self.cli_opts is not None)
        return self.cli_opts

    def prepare(self):
        # MAYBE: Do we need any args?
        pass
        # E.g.,
        #self.add_option('-x', '--x-position', dest='position_x',
        #    action='store', default=0, type=int,
        #    help='override the target placement and use this x instead')

    def parse(self):
        '''Parse the command line arguments.'''
        (opts, args) = self.parse_args()
        # parse_args halts execution if user specifies:
        #  (a) '-h', (b) '--help', or (c) unknown option.
        self.cli_opts = opts
        self.cli_args = args

class Pibeer(object):

    def __init__(self):
        self.serial = None

    def go(self, cli_opts):
        self.cli_opts = cli_opts

        trace("Welcome to beer!")

        while True:

            self.ensure_connected_serial()



    def ensure_connected_serial(self):
        # Get list of communication ports.
        #
        # NOTE: We assume there's at most one active port.
        #       Also, [lb] sees the device number increment when you unplug and
        #       replug the USB cable, so we can't just hardcode the device name.

        comports = list_ports_posix.comports()

        # Returns a dict-list, e.g.,
        #  [('/dev/ttyACM1', 'ttyACM1', 'USB VID:PID=2341:0001 SNR=64935343633351905211')]

        trace("Found %d comports" % (comports,))
        if len(comports) > 1:
            print("WARNING: Found more than 1 port! Guessing and grabbing the first port.")

        if len(comports) > 0:
            comport = comports[0][0]
            trace("Opening com port: %s" % (comport,))

            self.serial = serial.Serial('/dev/ttyACM0')



# ***

if (__name__ == "__main__"):

   parser = Pibeer_Parser()
   cli_opts = parser.get_opts()

   pbeer = Pibeer()
   pbeer.go(cli_opts)

# vim:tw=0:ts=4:sw=4:et:norl:

