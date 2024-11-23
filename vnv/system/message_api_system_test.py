#*********************************************************************
#
#   MODULE NAME:
#       message_api_system_test.py - MessageAPI System Test
#
#   DESCRIPTION:
#       Verify message API functionality
#
#   Copyright 2024 by Nate Lenze
#*********************************************************************

#---------------------------------------------------------------------
#                              IMPORTS
#---------------------------------------------------------------------
# from lib.msgAPI  import messageAPI
from lib.pi_pico import pi_pico
from lib.results import results
import time

#---------------------------------------------------------------------
#                            Defintions
#---------------------------------------------------------------------
unit_under_test = 0x02         # source identifier for unit under test
num_units 		= 0x03         # number of units in system

#---------------------------------------------------------------------
#                      TEST CASE VARIABLES
#---------------------------------------------------------------------
#					  test_description		  		 		 send_data													        dest             expected data
test_case_group = [ ( "Test 1 byte message",  				 [0xFF],															unit_under_test, [0xAA, 0x11] 	),
					( "Test 10 byte message", 				 [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A],      unit_under_test, [0xAA, 0x22 ]  ),
					( "Send 0 byte message",  				 [], 																unit_under_test, [0xBB, 0x03] 	),
					( "send to unsupported module", 		 [0xFF],															(num_units+1),   []			    ) ]

#								("description",    		      	   gen CRC, [dest, src,  pad, ver/size, key, DATA... crc],                (Rx'd, source, data, valid) )
distructive_test_case_group = [ ("Test CRC Error", 			       False,   [unit_under_test, 0x01, 0x00, 0x21, 0x00, 0xFF, 0xFF], 	      (True, unit_under_test, [0xBB, 0x01], True) ),
								("Test Size (Big) Error", 	       False,   [unit_under_test, 0x01, 0x00, 0x2F, 0x00, 0xFF, 0xFF], 	      (True, unit_under_test, [0xBB, 0x06], True) ),
								("Test Size (Small) Error",        True,    [unit_under_test, 0x01, 0x00, 0x20, 0x00            ], 	      (True, unit_under_test, [0xBB, 0x03], True) ),
								("Test Key Error", 			       True,    [unit_under_test, 0x01, 0x00, 0x21, 0xFF, 0xFF      ], 	      (True, unit_under_test, [0xBB, 0x04], True) ) ]

#                         test step description        sleep-flag, send data            destination      expected data
multi_msg_test_group = [ ("Test multi-message: setup", False,      [0xCC, 0x00, 0x00 ], unit_under_test, [(unit_under_test, [0xAA, 0x44 ], True)]                   												     	   ),
						 ("Test multi-message: msg 1", False,      [0xCC, 0x01, 0x00 ], unit_under_test, []                                         																		   ),
						 ("Test multi-message: msg 2", False,      [0xCC, 0x02, 0x00 ], unit_under_test, []                                         																		   ),
						 ("Test multi-message: msg 3", True,       [0xCC, 0x03, 0x00 ], unit_under_test, [(unit_under_test, [0xAA, 0x01], True), (unit_under_test, [0xAA, 0x02], True), (unit_under_test, [0xAA, 0x03], True)] ) ]
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
		self.pico = pi_pico( test_mode = True )


    # ==================================
    # run()
    # ==================================
	def run(self):
		self.log.test_requirement( "SWRD001" )
		self.log.test_requirement( "SWRD002" )

		self.normal_cases()
		self.distructive_cases()
		self.multi_msg_cases()

    # ==================================
    # normal_cases()
    # ==================================
	def normal_cases(self):
		for [ test_desc, send_data, dest, expected_rtn_data ] in test_case_group:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( test_desc )

			self.pico.msg_conn.TXMessage( message = send_data, destination = dest )
			time.sleep(2)
			return_msg = self.pico.msg_conn.RX_Single()

			#------------------------------------------------------------------
			# Because RXMessage() returns an array of [ Msg Rx'ed, source, data, 
			# validity ], we need to format the expexted rtn data as such
			#------------------------------------------------------------------
			if expected_rtn_data == []:
				#format for no message recieved is as follows
				rtn_list = ( False, 0xFF, [], True )
			else:
				rtn_list =  (True, dest, expected_rtn_data, True)

			#------------------------------------------------------------------
			# Verify Message
			#------------------------------------------------------------------
			self.log.compare_equal( rtn_list, return_msg, "Verify return data is as expected" )
		
    # ==================================
    # distructive_cases()
    # ==================================
	def distructive_cases(self):
		for [ desc, crc_gen, raw_tx, rtn_data ] in distructive_test_case_group:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( desc )
			time.sleep(.5)

			#------------------------------------------------------------------
			# If we want to auto-generate CRC, generate and append
			#------------------------------------------------------------------
			if( crc_gen ):
				raw_tx.append( self.pico.msg_conn._messageAPI__updateCRC( raw_tx ) )

			#------------------------------------------------------------------
			# Send raw TX over low-level interfaces
			#------------------------------------------------------------------
			self.pico.msg_conn._messageAPI__LoraSendMessage( messageList=raw_tx, messageSize=len( raw_tx ) )
			self.pico.msg_conn._messageAPI__LoraSetRxMode()

			time.sleep(1)
			rx_msg = self.pico.msg_conn.RX_Single()

			#------------------------------------------------------------------
			# Verify Message
			#------------------------------------------------------------------
			self.log.compare_equal( rx_msg, rtn_data, "Verify return data is as expected" )
		

    # ==================================
    # multi_msg_cases()
    # ==================================
	def multi_msg_cases(self):
		self.log.test_step( "Testing multi-message support")
		for [desc, sleep, send_data, dest, expected_rtn_data ] in multi_msg_test_group:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( desc )
			time.sleep(.5)

			self.pico.msg_conn.TXMessage( message = send_data, destination = dest )
			
			#------------------------------------------------------------------
			# extra sleep if flag is set
			#------------------------------------------------------------------
			if sleep:
				time.sleep(5)

			#------------------------------------------------------------------
			# Wait for return data
			#------------------------------------------------------------------
			time.sleep(1)
			actual_return = self.pico.msg_conn.RX_Multi()

			#------------------------------------------------------------------
			# Verify Message
			#------------------------------------------------------------------
			if( expected_rtn_data == [] ):
				self.log.compare_equal( actual_return, None, "Verify no data was returned (as expected)" )
			elif( actual_return == None ):
				self.log.compare_equal( True, False, "Data was expected but was not sent" )
			else:
				#parse data
				num_rx, data_rx = actual_return

				#skip compare and add debug prints if value length is not as expected
				if( len(data_rx) != len(expected_rtn_data) ):
					self.log.compare_equal( True, False, "Data returned was missing messages" )
					for src, data, validity in data_rx:
						print( "src: {}, valdity: {}, data, {}, ".format(src, validity, data ) )
				else:
					for msg in range(num_rx):
						actual_src, actual_data, actual_valid = data_rx[msg]
						expected_src, expected_data, expected_valid = expected_rtn_data[msg]
					
						self.log.compare_equal( expected=expected_src,   actual=actual_src,   case="Verify return data (source) is as expected" )
						self.log.compare_equal( expected=expected_data,  actual=actual_data,  case="Verify return data (data) is as expected" )
						self.log.compare_equal( expected=expected_valid, actual=actual_valid, case="Verify return data (validity) is as expected" )
				


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

