#!/usr/bin/env python3
#  -*- coding: utf-8 -*-
"""beerme.py
"""
# File: beerme.py
# Author: Landon Bouma
# Last Modified: 2016.11.10
# Project Page: https://github.com/landonb/raugupatis
# License: GPLv3

# SETUP
#
# Easy Setup with Pip.
#
# NOTE: The pyserial and requests modules are
#       pre-installed on Raspbian Jessie with Pixel.
#
#   #sudo pip install --upgrade pip
#   pip install pyserial
#   pip install requests
#
# On your dev machine, just alias this code
#
#   cd ~
#   /bin/ln -s path/to/raugupatis
#
# On your Pi, clone this code
#
#   cd ~
#   git clone https://github.com/landonb/raugupatis
#
# Raspbian Jessie Lite!
#
#   # So, install git fails on a 404 error unless you update first. I know, right.
#   sudo apt-get update
#   sudo apt-get install -y git
#   sudo apt-get install -y python3
#   sudo apt-get install -y python3-pip
#   sudo pip3 install pyserial
#   # Already installed:
#   sudo pip3 install requests
#   # Then you can:
#   cd ~
#   git clone https://github.com/landonb/raugupatis
#
#  NOTE: Sometimes you gotta run `git` again.
#    pi@hellapi:~/raugupatis/pi-beer $ git clone http://github.com/landonb/raugupatis
#    fatal: unable to access 'http://github.com/landonb/raugupatis/': Could not resolve host: github.com
#    pi@hellapi:~/raugupatis/pi-beer $ git clone http://github.com/landonb/raugupatis
#    remote: Counting objects: 8, done.
#    ...

# USAGE
#
#  ./beerme.py --host "https://path.to.sphinx"

# DEVs
#
#  pylint beerme.py

import os
import sys

import argparse
import binascii
import json
import shutil
import time
import traceback

# Requests refs:
#  http://docs.python-requests.org/en/master/
import requests
# Serial refs:
#  https://github.com/pyserial/pyserial
#  https://pythonhosted.org/pyserial/pyserial_api.html
import serial
# See also:
#  /usr/lib/python2.7/dist-packages/serial/tools/list_ports.py
from serial.tools import list_ports_posix
#from serial.tools.list_ports_posix import comports

# The serial read timeout defaults to None/no timeout.
# I tried 1/4 second and seemed to get a stray character.
SERIAL_TIMEOUT = 0.25
#SERIAL_TIMEOUT = 0.5
#SERIAL_TIMEOUT = 1.0

# In hella-ps-ino/bluedot.h, see: IBUTTON_LEN.
IBUTTON_LEN = 8

#def trace(*args, **kwargs):
#    if True:
#        print(*args, **kwargs)
USER_HOME = os.path.expanduser("~")
BEER_LOG = '%s/raugupatis/pi-beer/beer.log' % (USER_HOME,)
BEER_LOG_0 = '%s.0' % (BEER_LOG,)
# Trying 10 MB for now, should be plenty?
BEER_LOG_MAX_BYTES = 10000000
# 2016-11-09: Let's go 1MB again; debugging a truncate issue.
BEER_LOG_MAX_BYTES = 987654
# 2016-11-09: Getting a weird error on the Pi:
#  pi@hellapi:~/raugupatis/.git $ ls -la
#  ls: cannot access index: Structure needs cleaning
#  ls: cannot access ORIG_HEAD: Structure needs cleaning
#  total 56
#  drwxr-xr-x   8 pi pi 4096 Nov  9 01:26 .
#  drwxr-xr-x   5 pi pi 4096 Nov  4 19:36 ..
#  drwxr-xr-x   2 pi pi 4096 Nov  4 18:54 branches
#  -rwxr--r--   1 pi pi  261 Nov  4 18:54 config
#  -rw-r--r--   1 pi pi   73 Nov  4 18:54 description
#  -rw-r--r--   1 pi pi 4147 Nov  9 01:26 FETCH_HEAD
#  -rw-r--r--   1 pi pi   23 Nov  4 18:54 HEAD
#  drwxr-xr-x   2 pi pi 4096 Nov  4 18:54 hooks
#  -?????????   ? ?  ?     ?            ? index
#  drwxr-xr-x   2 pi pi 4096 Nov  4 18:54 info
#  drwxr-xr-x   3 pi pi 4096 Nov  4 18:54 logs
#  drwxr-xr-x 140 pi pi 4096 Nov  9 01:26 objects
#  -?????????   ? ?  ?     ?            ? ORIG_HEAD
#  -rw-r--r--   1 pi pi  107 Nov  4 18:54 packed-refs
#  drwxr-xr-x   5 pi pi 4096 Nov  4 18:54 refs
#  pi@hellapi:~/raugupatis/.git $ chmod 644 index
#  chmod: cannot access ‘index’: Structure needs cleaning
# and the log is truncated and beerme.log.0 was 11MB.
# I wonder if I screwed something up rotating the file.

TRACEF = open(BEER_LOG, 'w')
def trace(msg):
    """trace"""
    global TRACEF
    # MEH. We could just print to stdout and let the caller
    #      decide to redirect output to a file, since otherwise
    #      we have to manage the log file so it doesn't grow unwieldy
    #      in size. But it seems easier to do the logging here so we
    #      don't have to mess around with the init.d file. We'll just
    #      be thoughtful and make sure to rotate the log occasionally.
    statinfo = os.stat(BEER_LOG)
    if statinfo.st_size > BEER_LOG_MAX_BYTES:
        shutil.copy(BEER_LOG, BEER_LOG_0)
        TRACEF.truncate(0)
        # 2016-11-09: Will this fix the problem I had?
        TRACEF.close()
        TRACEF = open(BEER_LOG, 'w')
    TRACEF.write(msg + '\n')
    TRACEF.flush()

def warn(msg):
    trace(msg)
    print(msg)

class BeermeSerialException(Exception):
    '''Raised on Serial error to cleanup current connection.'''
    pass

class PibeerParser(argparse.ArgumentParser):

    def __init__(self):
        argparse.ArgumentParser.__init__(self)
        self.cli_opts = None

    def get_opts(self):
        self.prepare()
        self.parse()
        assert(self.cli_opts is not None)
        return self.cli_opts

    def prepare(self):

        self.add_argument(
            '-H', '--host', dest='sphinx_host',
            action='store', type=str, required=True,
            help='Sphinx host address'
        )

        # FIXME: Use a token to identify with Sphinx.
        #        For now, *anyone* can talk to Sphinx
        #         if they know the URL and API.
        #        (We could also use an IP whitelist.)
        #
        #self.add_argument(
        #    '-t', '--token', dest='sphinx_token',
        #    action='store', type=str, required=True,
        #    help='Sphinx authentication token'
        #)

        self.add_argument(
            '-D', '--device', dest='serial_dev',
            action='store', type=str, required=False,
            help='Serial device, e.g., /dev/ttyACM0'
        )

    def parse(self):
        '''Parse the command line arguments.'''
        # parse_args halts execution if user specifies:
        #  (a) '-h', (b) '--help', or (c) unknown option.
        self.cli_opts = self.parse_args()

class Pibeer(object):

    def __init__(self):
        self.cli_opts = None
        self.serial = None
        self.found_work_last_time = True

    def go(self, cli_opts_):
        self.cli_opts = cli_opts_

        trace("Welcome to beer!")

        loop_n = 0
        while True:

            loop_n += 1
            if (loop_n % 100) == 0:
                trace("On loop # %d" % (loop_n,))
            try:
                if self.serial is None:
                    self.connect_serial()
                if self.serial is not None:
                    self.look_for_work()
                    # MAYBE: sleep if we don't find work? Or just loop fastly?
                else:
                    time.sleep(0.2)
            except BeermeSerialException as err:
                warn('WARNING: BeermeSerialException: %s' % (str(err),))
                if self.serial is not None:
                    self.clear_serial()
            except serial.serialutil.SerialException as err:
                # E.g, serial.serialutil.SerialException: device reports readiness
                #           to read but returned no data (device disconnected?)
                warn('WARNING: SerialException: %s' % (str(err),))
                self.serial = None

            except Exception as err:
                warn("WARNING: Unhandled Exception: %s" % (str(err),))
                warn(traceback.format_exc())
                # Breathe a little for if you're debugging.
                time.sleep(0.5)

    def connect_serial(self):
        # Get list of communication ports.
        #
        # NOTE: We assume there's at most one active port.
        #       Also, [lb] sees the device number increment when you unplug and
        #       replug the USB cable, so we can't just hardcode the device name.

        comport = None

        if not self.cli_opts.serial_dev:
            trace("Looking for comports...")
            comports = list_ports_posix.comports()

            # Returns a dict-list, e.g.,
            #  [('/dev/ttyACM0', 'ttyACM0', '3f201000.uart')]
            #  [('/dev/ttyACM1', 'Arduino Uno', 'USB VID:PID=2341:0001 SNR=64935343633351905211')]

            trace("Found %d comport(s)" % (len(comports),))

            for cport in comports:
                if cport[1] == 'Arduino Uno':
                    comport = cport[0]
                    break
            #if comport is None:
            #    if len(comports) > 0:
            #        comport = comports[0][0]
            #        if len(comports) > 1:
            #            warn("WARNING: Found more than 1 port! Guessing and grabbing the first port.")

        else:
            comport = self.cli_opts.serial_dev

        if comport is not None:
            trace("Opening com port: %s" % (comport,))

            try:
                #self.serial = serial.Serial(comport, baudrate='9600')
                self.serial = serial.Serial(comport, baudrate='115200')
                trace("connect_serial: connected: serial: %s" % (self.serial,))
            except FileNotFoundError as err:
                # E.g., FileNotFoundError: [Errno 2] No such file or directory: '/dev/ttyACM0'
                warning_msg = (
                    "WARNING: Unable to open com port (%s) / err: %s"
                    % (comport, str(err),)
                )
                warn(warning_msg)
            else:
                # The read timeout defaults to None/no timeout.
                self.serial.timeout = SERIAL_TIMEOUT
                trace("connect_serial: self.serial.timeout: %s" % (self.serial.timeout,))

                self.clear_serial()

    def clear_serial(self):
        trace("Clearing the serial buffer...")
        # Just mop up whatever drippings we find and pour 'em out.
        # HRMM: After opening the port, the first read is coming back
        #       empty, and then second read finds crud. So fail twice
        #       before failing for good.
        n_repeats = 0
        while True:
            #trace("Calling self.serial.read...")
            next_ch_ = self.serial.read(1)
            if len(next_ch_):
                try:
                    next_ch = next_ch_.decode('utf-8')
                except Exception as err:
                    # Not valid utf-8 character.
                    next_ch = 0
                trace("clear_serial: discard char: %s" % (next_ch_,))

# FIXME: Need to figure this out!
#        If Arduino keeps talking, we keep clearing!!!
                if next_ch == '\n':
                    # Let's give up here?
                    trace("clear_serial: found newline")
                    break
            else:
                # Nothing returned, assume buffer is empty.
                trace("clear_serial: all clear")
                n_repeats += 1
                if n_repeats > 1:
                    break

    def read_line_serial(self):
        line = ''
        while True:
            next_ch_ = self.serial.read(1)
            if len(next_ch_):
                #char = chr(next_ch_[0])
                try:
                    char = next_ch_.decode('utf-8')
                except Exception as err:
                    # Not valid utf-8 character.
                    char = 0
                else:
                    if char == '\r':
                        # Skip carriage return.
                        continue
                    if char == '\n':
                        # All done.
                        break
                    line += char
            else:
                # Timeout.
                if line:
                    trace("WARNING: read_line_serial: timeout on line: %s" % (line,))
                    pass
                line = None
                break
        #trace("read_line_serial: line: %s" % (line,))
        return line

    def look_for_work(self):
        found_work = True
        if self.found_work_last_time:
            trace("Looking for work")
        intro = self.read_line_serial()
        if intro:
            if intro != 'hi':
                raise BeermeSerialException("WARNING: Unrecognized intro: %s" % (intro,))
            else:
                cmd = self.read_line_serial()
                # See also hella-ps-ino/comm.cpp for list of commands.
                if cmd == 'auth':
                    self.handle_cmd_authenticate()
                elif cmd == 'state':
                    self.handle_cmd_update_flow()
                elif cmd == 'trace':
                    self.handle_cmd_trace()
                else:
                    raise BeermeSerialException("WARNING: Unrecognized command: %s" % (cmd,))
                # If we're here, the command was processed.
        else:
            # No intro; no work.
            found_work = False
        self.found_work_last_time = found_work
        return found_work

    def look_for_header(self):
        found_header = False
        next_ch_ = self.serial.read(1)
        if len(next_ch_):
            assert(len(next_ch_) == 1)
            # next_ch_ is a bytes, which we can access like a list.
            if next_ch_[0] != 0:
                raise BeermeSerialException(
                    "WARNING: Unexpected request: Header not NULL byte: %s" % (next_ch_,)
                )
            found_header = True
        return found_header

    def look_for_footer(self):
        trace("Looking for outro")
        outro = self.read_line_serial()
        if outro:
            if outro != 'bye':
                raise BeermeSerialException("WARNING: Unrecognized outro: %s" % (outro,))
        else:
            raise BeermeSerialException("WARNING: Did not find outro after command")

    def look_for_command(self):
        cmd_byte = None
        # Second byte is command byte.
        next_ch_ = self.serial.read(1)
        if len(next_ch_):
            assert(len(next_ch_) == 1)
            cmd_byte = next_ch_[0]
        return cmd_byte

    def handle_cmd_authenticate(self):
        token = self.serial.read(IBUTTON_LEN)
        if not len(token):
            raise BeermeSerialException("WARNING: Token read failed: %s" % (token,))
        assert(len(token) == IBUTTON_LEN)

        next_chs_ = self.serial.read(2)
        if len(next_chs_):
            assert(len(next_chs_) == 2)
            next_chs = next_chs_.decode('utf-8')
# next_chs_ is a bytes, which we can access like a list.
            if next_chs[0] != '\r':
                raise BeermeSerialException(
                    "WARNING: Expected carriage return after token: %s / next_ch: %s"
                    % (token, next_chs_,)
                )
            if next_chs[1] != '\n':
                raise BeermeSerialException(
                    "WARNING: Expected newline after carriage return after token: %s / next_ch: %s"
                    % (token, next_chs_,)
                )
        else:
            raise BeermeSerialException(
                "WARNING: Expected newline after token: %s / next_ch: <timeout>"
                % (token,)
            )

        self.look_for_footer()

        # Now that we've parsed the serial command from
        # the 'duino, talk to Sphinx across the net.
        authenticated = self.sphinx_authenticate(token)
# FIXME/2016-11-04: The Arduino is not detected the 'ok' response correctly.
        resp = 'ok' if authenticated else 'no'
        trace("handle_cmd_authenticate: token: %s / resp: %s" % (token, resp,))

# 2016-11-09: TEST: I think I was missing the newline!
        resp += '\n'

        self.serial.write(resp.encode('utf-8'))
#        self.serial.write(resp)
#        self.serial.flush()

    def sphinx_authenticate(self, token):
        authenticated = False

        trace(
            "Connecting to sphinx at %s using token %s"
            % (self.cli_opts.sphinx_host, token,)
        )

        token_bin = binascii.hexlify(token)
        trace(
            "Connecting to sphinx at %s using token_bin %s"
            % (self.cli_opts.sphinx_host, token_bin,)
        )
        token_hex = token_bin.decode('utf-8')
        trace(
            "Connecting to sphinx at %s using token_hex %s"
            % (self.cli_opts.sphinx_host, token_hex,)
        )

        headers = {'content-type': 'application/json',}
        req = requests.post(
            self.cli_opts.sphinx_host + '/api/token/authorize',
            data=json.dumps({'token': token_hex,}),
            headers=headers,
        )
        trace("sphinx_authenticate: req: %s" % (req,))

        trace("sphinx_authenticate: req.text: %s" % (req.text,))

        resp = json.loads(req.text)
        trace("sphinx_authenticate: resp: %s" % (resp,))

        if resp['authorized']:
            trace("sphinx_authenticate: AUTHENTICATED")
            authenticated = True

        return authenticated

    def handle_cmd_update_flow(self):
        # FIXME: Finish implementing. Consume this data.

        line = self.read_line_serial()
        if line != 'state':
            raise BeermeSerialException(
                "WARNING: handle_cmd_update_flow: expected: state / got: %s"
                % (line,)
            )

        state = self.read_line_serial()
        # FIXME: Do what with this data?

        line = self.read_line_serial()
        if line != 'blips':
            raise BeermeSerialException(
                "WARNING: handle_cmd_update_flow: expected: blips / got: %s"
                % (line,)
            )

        blips = self.read_line_serial()
        # FIXME: Do what with this data?

        line = self.read_line_serial()
        if line != 'msecs':
            raise BeermeSerialException(
                "WARNING: handle_cmd_update_flow: expected: msecs / got: %s"
                % (line,)
            )

        msecs = self.read_line_serial()
        # FIXME: Do what with this data?

        trace('handle_cmd_update_flow: state: %s / blips: %s / msecs: %s', state, blips, msecs)

    def handle_cmd_trace(self):
        # MAYBE: Should we loop until footer found?
        #        Either the protocol says just one msg (newline terminated)
        #        per trace command; or zero of more newline terminated lines
        #        followed by "bye" on its own line (so obviously "bye" cannot
        #        be sent as a trace message).
        while True:
            msg = self.read_line_serial()
            # In lieu of calling self.look_for_footer(),
            # we'll just spin until we find the outro.
            if msg == 'bye':
                break
            elif msg:
                trace("INO says: %s" % (msg,))
            else:
                #raise BeermeSerialException("WARNING: handle_cmd_trace: No msg found")
                trace("WARNING: INO says: NOTHING")

# ***

if __name__ == "__main__":

    parser = PibeerParser()
    cli_opts = parser.get_opts()

    pbeer = Pibeer()
    pbeer.go(cli_opts)

# Oh. PEP8 says use spaces, no tabs.
# vim:tw=0:ts=4:sw=4:et:norl:

