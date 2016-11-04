#!/usr/bin/env python3
#  -*- coding: utf-8 -*-
# File: beerme.py
# Author: Landon Bouma
# Last Modified: 2016.11.04
# Project Page: https://github.com/landonb/raugupatis
# License: GPLv3

# SETUP
#
# Easy Setup with Pip.
# 
# NOTE: These are pre-installed on Raspbian Jessie with Pixel.
#
#   #sudo pip install --upgrade pip
#   pip install pyserial
#   pip install requests

# USAGE
#
# ./beerme.py --host "https://path.to.sphinx" --token "sphinx-token"

import os
import sys

import optparse
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
import time

# The serial read timeout defaults to None/no timeout.
# We'll use a half-second to start.
serial_timeout = 0.25

# FIXME: 
sphinx_addr = 'https://'
sphinx_token = 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'

#def trace(*args, **kwargs):
#    if True:
#        print(*args, **kwargs)
tracef = open('/home/pi/raugupatis/pi-beer/beer.log', 'w')
def trace(msg):
	if True:
		trace.fwrite(msg + '\n')

def warn(msg):
	trace(msg)
	print(msg)

class BeermeSerialException(Exception):
   '''Raised on Serial error to cleanup current connection.'''
   pass

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

		self.add_option('-h', '--host', dest='sphinx_host',
		    action='store', type=str, required=True,
		    help='Sphinx host address')

		self.add_option('-t', '--token', dest='sphinx_token',
		    action='store', type=str, required=True,
		    help='Sphinx authentication token')

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
				else:
					time.sleep(0.2)
			except BeermeSerialException as err:
				warn('WARNING: BeermeSerialException: %s' % (str(err),))
				if self.serial is not None:
					self.clear_serial()
			except serial.serialutil.SerialException as err:
				# E.g, serial.serialutil.SerialException: device reports readiness
				#           to read but returned no data (device disconnected?)
				self.serial = None

			except Exception as err:
				warn("WARNING: Unhandled Exception: %s" % (str(err),))

	def connect_serial(self):
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
			warn("WARNING: Found more than 1 port! Guessing and grabbing the first port.")

		if len(comports) > 0:
			comport = comports[0][0]
			trace("Opening com port: %s" % (comport,))

			try:
				self.serial = serial.Serial(comport)
			except FileNotFoundError as err:
				# E.g., FileNotFoundError: [Errno 2] No such file or directory: '/dev/ttyACM0'
				warning_msg = (
					"WARNING: Unable to open com port (%s) / err: %s"
					% (comport, str(err),)
				)
				warn(warning_msg)
			else:
				self.clear_serial()
				# The read timeout defaults to None/no timeout.
				self.serial.timeout = serial_timeout

	def clear_serial(self):
		# Just mop up whatever drippings we find and pour 'em out.
		next_ch_ = self.serial.read(1)
		if len(next_ch_):
			assert(len(next_ch_) == 1)
			next_ch = next_ch_.decode('utf-8')
			trace("clear_serial: discard char: %s" % (next_ch,))
		# else, nothing returned, assume buffer is empty.

	def look_for_work(self):
		# What's our protocol?
		#   Should we just make up something simple for now?
		#   How about a 1 byte header 0x00
		#   And then a 1 byte command
		#   And then data
		#   And then checksum (command + data bytes)
		#   And then 2 byte 0xFF 0xFF tail
		# Commands:
		#   1: Authenticate user
		#   2: Flow?
		if self.look_for_header():
			cmd_byte = self.look_for_command()
			if cmd_byte == 1:
				self.handle_cmd_authenticate(cmd_byte)
			else:
				raise BeermeSerialException("WARNING: Unrecognized command: %s" % (cmd_byte,))
				
	def look_for_header(self):
		found_header = False
		next_ch_ = self.serial.read(1)
		if len(next_ch_):
			assert(len(next_ch_) == 1)
			# next_ch_ is a bytes, which we can access like a list.
			if next_ch_[0] != 0:
				raise BeermeSerialException("WARNING: Unexpected request: Header not NULL byte: %s" % (next_ch_,))
			found_header = True
		return found_header

	def look_for_command(self):
		cmd_byte = None
		# Second byte is command byte.
		next_ch_ = self.serial.read(1)
		if len(next_ch_):
			assert(len(next_ch_) == 1)
			cmd_byte = next_ch_[0]
		return cmd_byte

	def handle_cmd_authenticate(self, cmd_byte):
		expectsum = 0
		expectsum += cmd_byte

# FIXME: token_len...
		# Next XXXXXX bytes are token.
		token_len = 4 

		token_ = self.serial.read(token_len)
		if not len(token_):
			raise BeermeSerialException("WARNING: Token read failed: %s" % (token_,))
		assert(len(token_) == token_len)

		for tok_byte in token_:
			expectsum += tok_byte
		token = token_.decode('utf-8')
		trace("look_for_work: command: Authenticate / token: %s" % (token,))

		checksum_ = self.serial.read(1)
		if len(checksum_):
			assert(len(checksum_) == 1)
			if checksum_[0] != expectsum:
				warning_msg = (
					"WARNING: checksum mismatch: ours: %s / theirs: %s [%s]",
					expectsum, checksum_, checksum_[0],
				)

		self.look_for_footer()

		# Now that we've tediously parsed the serial command
		# from the 'duino, talk to Sphinx across the net.
		authenticated = self.sphinx_authenticate(token)
		resp = "T" if authenticated else "F"
		self.serial.write(resp.encode('utf-8'))
		self.serial.flush()

	def look_for_footer(self):
		# Footer is 2 null bytes. Don't ask why. Just because.
		footer_ = self.serial.read(2)
		if len(footer_):
			assert(len(footer_) == 2)
			if (footer_[0] != 0) || (footer_[1] != 0):
				raise BeermeSerialException("WARNING: Footer not 2 nulls: %s" % (footer_,))

	def sphinx_authenticate(self, token):
		authenticated = False
		trace("Connecting to sphinx at %s using token %s"
			% (self.cli_opts.sphinx_host, self.cli_opts.sphinx_token,)
		)

# FIXME: This guy
		self.cli_opts.sphinx_host
		self.cli_opts.sphinx_token
		# etc.
		FOR_EXAMPLE="""
			>>> r.status_code
			200
			>>> r.headers['content-type']
			'application/json; charset=utf8'
			>>> r.encoding
			'utf-8'
			>>> r.text
			u'{"type":"User"...'
			>>> r.json()
			{u'private_gists': 419, u'total_private_repos': 77, ...}
		"""
		if False:
			authenticated = True

		return authenticated

# ***

if (__name__ == "__main__"):

   parser = Pibeer_Parser()
   cli_opts = parser.get_opts()

   pbeer = Pibeer()
   pbeer.go(cli_opts)

# vim:tw=0:ts=4:sw=4:noet:norl:

