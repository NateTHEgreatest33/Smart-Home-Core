#*********************************************************************
#
#   MODULE NAME:
#       pi_pico.py - Raspberry Pi Pico Testing Class
#
#   DESCRIPTION:
#       Class to provide functionaility for testing with the Raspberry
#		pi pico.
#
#   Copyright 2024 by Nate Lenze
#*********************************************************************

#---------------------------------------------------------------------
#                              IMPORTS
#---------------------------------------------------------------------
from msgAPI import messageAPI

import serial
import time

#---------------------------------------------------------------------
#                          CLASSES
#---------------------------------------------------------------------
class pi_pico:
	msg_conn = None

	# ==================================
	# Constructor
	# ==================================
	def __init__(self, test_mode = True ):
		self.msg_conn = messageAPI( 
								bus = 0, 
								chip_select = 0, 
								currentModule = 0x00, 
								listOfModules=[0x00,0x01,0x02] 
								)
		self.__uart_conn = serial.Serial(
								port='/dev/ttyS0'
								baudrate = 115200,
								parity=serial.PARITY_NONE,
								stopbits=serial.STOPBITS_ONE,
								bytesize=serial.EIGHTBITS,
								timeout=1
								)
		self.msg_conn.InitAPI()
		self.set_test_mode( test_mode )
	
	# ==================================
	# write_read_uart()
	# ==================================
	def write_read_uart(self, str, return_carrige = True, read_limit = 200 ):
		# ------------------------------------
		# convert string to hex & append carrige
		# return if expected
		# ------------------------------------
		hex_list = self.__str_to_hex( str )
	
		if( return_carrige ):
			hex_list.append( hex( ord('\r') ) )
		
		# ------------------------------------
		# write message
		# ------------------------------------
		self.__uart_conn.write( hex_list )
		time.sleep( .1 )
		
		# ------------------------------------
		# read up to limit specified, decode,
		# and return string
		# ------------------------------------
		rtn_data = self.__uart_conn.read( read_limit )
		rtn_data = rtn_data.decode( "utf-8" )
		
		return rtn_data
	
	# ==================================
	# set_test_mode()
	# ==================================
	def set_test_mode( self, enabled ):
		# ------------------------------------
		# attempt to place pico into test mode
		# ------------------------------------
		rtn = self.write_read_uart( "testmode")
		if self.__verify_test_mode( rtn, enabled ):
			return True

		# ------------------------------------
		# since first attempt failed, try again
		# (toggle behavior)
		# ------------------------------------
		rtn = self.write_read_uart( "testmode")
		if self.__verify_test_mode( rtn, enabled ):
			return False
		
		# ------------------------------------
		# if still unable to enter test mode
		# there must be another issue, print
		# error and exit
		# ------------------------------------
		print("Test mode currently broken")
		return False

	# ==================================
	# helper function: __verify_test_mode()
	# ==================================
	def __verify_test_mode( self, rtn_str, enabled ):
		expected_str = ( "testmode\rTest Mode: enabled\n" if enabled  else "testmode\rTest Mode: disabled\n")
		return ( rtn_str == expected_str )

	# ==================================
	# helper function: str_to_hex()
	# ==================================
	def __str_to_hex_list(self, str):
		hex_list = []
		for i in str:
			hex_list.append( hex( ord( i ) ) )

		return hex_list


