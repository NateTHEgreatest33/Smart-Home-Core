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
from lib.msgAPI  import messageAPI
from lib.results import results
from enum        import Enum
import time

#---------------------------------------------------------------------
#                            Defintions
#---------------------------------------------------------------------
unit_under_test = 0x02         # source identifier for unit under test
num_units 		= 0x03         # number of units in system
#---------------------------------------------------------------------
#                            VARIABLES
#---------------------------------------------------------------------
testIndex = ( 'test_description', 'send_data', 'destination', 'expected_data' )
dTestIndex = ( 'test_description', 'raw_send', 'expected_data')

#---------------------------------------------------------------------
#                      TEST CASE VARIABLES
#---------------------------------------------------------------------
#					  test_description		  		 send_data													  dest  expected data
test_case_group = [ ( "Test 1 byte message",  		[0xFF],															unit_under_test, [0xAA, 0x11] 	),
					( "Test 10 byte message", 		[0x01, 	0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A],  unit_under_test, [0xAA, 0x22 ]  ),
					( "Send 0 byte message",  		[], 															unit_under_test, [0xBB, 0x03] 	),
					( "send to unsupported module", [0xFF],															(num_units+1),   []			    ) ]

#								("description",    			[dest, src,  pad, ver/size, key, DATA... crc],  (True, source, data, valid) )
distructive_test_case_group = [ ("Test CRC Error", 			[unit_under_test, 0x01, 0x00, 0x11, 0x00, 0xFF, 0xFF], 	    (True, unit_under_test, [0xBB, 0x01], True) ),
								("Test Size (Big) Error", 	[unit_under_test, 0x01, 0x00, 0x1F, 0x00, 0xFF, 0xFF], 	    (True, unit_under_test, [0xBB, 0x02], True) ),
								("Test Size (Small) Error", [unit_under_test, 0x01, 0x00, 0x10, 0x00, 0xFF], 			(True, unit_under_test, [0xBB, 0x03], True) ),
								("Test Key Error", 			[unit_under_test, 0x01, 0x00, 0x11, 0xFF, 0xFF, 0xF9 ], 	(True, unit_under_test, [0xBB, 0x04], True) ) ]

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
		self.module = messageAPI( bus=0, chip_select=0, currentModule=0x00, listOfModules=[0x00,0x01,0x02] )
		self.module.InitAPI()


    # ==================================
    # run()
    # ==================================
	def run(self):
		self.log.test_requirement( "SWRD001" )
		self.log.test_requirement( "SWRD002" )

		self.normal_cases()
		self.distructive_cases()



    # ==================================
    # normal_cases()
    # ==================================
	def normal_cases(self):
		for [ test_desc, send_data, dest, expected_rtn_data ] in test_case_group:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( test_desc )

			self.module.TXMessage( message = send_data, destination = dest )
			time.sleep(2)
			return_msg = self.module.RXMessage()

			#------------------------------------------------------------------
			# Because RXMessage() returns an array of [ MsgRx'ed, source, data, 
			# validity ], we need to format the expexted rtn data as such
			#------------------------------------------------------------------
			if expected_rtn_data == []:
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
		for [ desc, raw_tx, rtn_data ] in distructive_test_case_group:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( desc )

			#------------------------------------------------------------------
			# Send raw TX over low-level interfaces
			#------------------------------------------------------------------
			self.module._messageAPI__LoraSendMessage( messageList=raw_tx, messageSize=len( raw_tx ) )
			self.module._messageAPI__LoraSetRxMode()

			time.sleep(1)
			rx_msg = self.module.RXMessage()

			#------------------------------------------------------------------
			# Verify Message
			#------------------------------------------------------------------
			self.log.compare_equal( rx_msg, rtn_data, "Verify return data is as expected" )
		
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

