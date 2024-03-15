#*********************************************************************
#
#   MODULE NAME:
#       console_api_system_test.py - MessageAPI System Test
#
#   DESCRIPTION:
#       Verify console API functionality
#
#   Copyright 2024 by Nate Lenze
#*********************************************************************

#---------------------------------------------------------------------
#                              IMPORTS
#---------------------------------------------------------------------
from lib.results import results
import serial
import time
import binascii


#---------------------------------------------------------------------
#                            Defintions
#---------------------------------------------------------------------

#---------------------------------------------------------------------
#                      TEST CASE VARIABLES
#---------------------------------------------------------------------
                            #command      expected result
test_case_normal_group = [ ( "help",      "help\nLIST OF COMMANDS:\n - clear\n - print log\n - help" ),
				    	   ( "print log", "print log\nASSERT LOG:\r\n"                                    ),
						   ( "clear",     "" ) ]

								  #command action        send text                          expected result
test_case_unsupported_group = [ ( "unsupported command", "unsupported",                     "unsupported\ncommand not found" ),
                                ( "command too long",    "1234567890123456789012345678901", "123456789012345678901234567890\n1" ) ]

#---------------------------------------------------------------------
#                             CLASSES
#---------------------------------------------------------------------

# ==========================================================
# Test
# ==========================================================
class Test:
    # ==================================
    # Constructor
    # ==================================
	def __init__(self, result_obj ):
		self.log = result_obj
		self.uart_conn = serial.Serial(
								port='/dev/ttyS0'
								baudrate = 115200,
								parity=serial.PARITY_NONE,
								stopbits=serial.STOPBITS_ONE,
								bytesize=serial.EIGHTBITS,
								timeout=1
								)


    # ==================================
    # run()
    # ==================================
	def run(self):
		self.log.test_requirement( "SWRD004" )
		self.run_cases()

    # ==================================
    # run_cases()
    # ==================================
	def run_cases(self):
		#----------------------------------------------------------------------
		# Test Normal Cases
		#----------------------------------------------------------------------
		self.log.test_step( "Test supported commands")
		for [ cmd, expected_rtn ] in test_case_normal_group:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( "Test \'{}\' command is responded to correctly".format( cmd ) )

			self.uart_conn.flushInput()
			hex_str = self.__str_to_hex_list( cmd )

			#------------------------------------------------------------------
			# TX and RX over uart
			#------------------------------------------------------------------
			self.uart_conn.write( hex_str ) 		 #send command
			self.uart_conn.write( hex( ord('\r') ) ) #send return carrige

			time.sleep(0.1)

			rtn_data = self.uart_conn.read( 100 )
			rtn_data = rtn_data.decode( "utf-8" )

			self.log.compare_equal( rtn_data, expected_rtn, "Verify command returns as expected" )


		#----------------------------------------------------------------------
		# Test unsupported commands
		#----------------------------------------------------------------------
		self.log.test_step( "Test un-supported commands")
		for [ action, cmd, expected_rtn ] in test_case_unsupported_group:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( "Test console correctly responses to {}".format( action ) )

			self.uart_conn.flushInput()
			hex_str = self.__str_to_hex_list( cmd )

			#------------------------------------------------------------------
			# TX and RX over uart
			#------------------------------------------------------------------
			self.uart_conn.write( hex_str ) #send command

			# only send return carrige for unsupported command, bc
			# we want to verify too long command self generates return
			if cmd == "unsupported command":
				self.uart_conn.write( hex( ord('\r') ) ) 

			time.sleep(0.1)

			rtn_data = self.uart_conn.read( 100 )
			rtn_data = rtn_data.decode( "utf-8" )

			self.log.compare_equal( rtn_data, expected_rtn, "Verify command returns as expected" )



	# ==================================
    # helper function: str_to_hex()
    # ==================================
	def __str_to_hex_list(self, str):
		hex_list = []
		for i in str:
			hex_list.append( hex( ord( i ) ) )

		return hex_list




#---------------------------------------------------------------------
#                      MAIN FUNCTION
#---------------------------------------------------------------------
def main():
	log = results( __file__ )
	test = Test( log )

	test.run()
#---------------------------------------------------------------------
#                      MAIN TEST LOOP SWRS0001
#---------------------------------------------------------------------
if __name__ == "__main__":
  main()

