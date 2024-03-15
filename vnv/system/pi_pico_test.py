#*********************************************************************
#
#   MODULE NAME:
#       pi_pico_test.py - Pi Pico Test Script
#
#   DESCRIPTION:
#       Prove message API functionality on the pi pico
#
#   Copyright 2023 by Nate Lenze
#*********************************************************************

#---------------------------------------------------------------------
#                              IMPORTS
#---------------------------------------------------------------------
from vnv.system.lib.msgAPI import messageAPI
from enum                  import Enum
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
#Test cases
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
	def __init__(self, bus, chip_select, currentModule, listOfModules, testCases, dis_testCases, module_under_test):
		#setup local var's
		self.module = messageAPI( bus=bus, chip_select=chip_select, currentModule=currentModule, listOfModules=listOfModules )
		self.module.InitAPI()
		self.TestCases = testCases
		self.DistructiveTestCases = dis_testCases
		self.module.debug_prints = False
		self.module_under_test = module_under_test

    # ==================================
    # RunTestCases()
    # ==================================
	def RunTestCases(self):
		for i in self.TestCases:
			print( bcolors.ENDC + i[ testIndex.index('test_description') ] + ":", end = '')

			self.module.TXMessage( message=i[testIndex.index('send_data')], destination=i[testIndex.index('destination')] )
			time.sleep(2)
			return_tpl = self.module.RXMessage()

			if i[testIndex.index('expected_data')] == []:
				if return_tpl == ( False, 0xFF, [], True ):
					print(bcolors.OKGREEN + " PASSED")
				else:
					print(bcolors.FAIL + " FAILED")
					print(bcolors.WARNING + "Expected: ( False, 0xFF, [], True ).\nReturned: "+ str(return_tpl) )

			else:
				if return_tpl == (True, i[testIndex.index('destination')], i[testIndex.index('expected_data')], True):
					print(bcolors.OKGREEN + " PASSED")
				else:
					print(bcolors.FAIL + " FAILED")
					print(bcolors.WARNING + "Expected: ( True, " + str(i[testIndex.index('destination')]) + ", " + str(i[testIndex.index('expected_data')]) + ", True )\nReturned: " + str(return_tpl) )


    # ==================================
    # RunDistructiveTesting()
    #
    # NOTE: This function is used to 
    # test cases that cannot be properly
    # done through the offical messageAPI
    # ==================================
	def RunDistructiveTesting(self):
		for testcase in self.DistructiveTestCases:
			#send raw data on LoRa

			print( bcolors.ENDC + testcase[ dTestIndex.index('test_description') ] + ":", end = '')
			self.module._messageAPI__LoraSendMessage(messageList=testcase[ dTestIndex.index('raw_send') ], messageSize=len( testcase[ dTestIndex.index('raw_send') ] ) )
			self.module._messageAPI__LoraSetRxMode()
			time.sleep(1)

			#verify return data
			return_tpl = self.module.RXMessage()
			if testcase[ dTestIndex.index('expected_data') ] == return_tpl:
				print(bcolors.OKGREEN + " PASSED")
			else:
				print(bcolors.FAIL + " FAILED")
				print(bcolors.WARNING + "Expected: "+ str(testcase[ dTestIndex.index('expected_data') ]) +"\nReturned: "+ str(return_tpl) )



#---------------------------------------------------------------------
#                      MAIN FUNCTION SWRS0001
#---------------------------------------------------------------------
def main():
    Test = Test( bus=0, chip_select=0, currentModule=0x00, listOfModules=[0x00,0x01,0x02], testCases=test_case_group, dis_testCases = distructive_test_case_group, module_under_test = 0x02 )
    print(bcolors.HEADER + "Normal Test Cases:")
    Test.RunTestCases()
    print(bcolors.HEADER + "Distructive Test Cases:")
    Test.RunDistructiveTesting()

#---------------------------------------------------------------------
#                      MAIN TEST LOOP SWRS0001
#---------------------------------------------------------------------
if __name__ == "__main__":
  main()

