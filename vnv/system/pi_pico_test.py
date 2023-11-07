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
from msgAPI import messageAPI
from enum import Enum
import time

#---------------------------------------------------------------------
#                            VARIABLES
#---------------------------------------------------------------------
testIndex = ( 'test_description', 'send_data', 'destination', 'expected_data' )
dTestIndex = ( 'test_description', 'raw_send', 'expected_data')


#---------------------------------------------------------------------
#                      TEST CASE VARIABLES
#---------------------------------------------------------------------
#					  test_description		  	    send_data													    dest  expected data
test_case_group = [ ( "Test 1 byte message",  		[0xFF],															0x02, [0xAA, 0x11] 	),
					( "Test 10 byte message", 		[0x01, 	0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A],  0x02, [0xAA, 0x22 ] ),
					( "Send 0 byte message",  		[], 															0x02, [0xBB, 0x03] 	),
					( "send to unsupported module", [0xFF],															0x05, []			) ]

#								("description",    			[dest, src,  pad, ver/size, key, DATA... crc], (True, source, data, valid) )
distructive_test_case_group = [ ("Test CRC Error", 			[0x00, 0x01, 0x00, 0x11, 0x00, 0xFF, 0xFF], 	(True, 0x01, [0xBB, 0x01], True) ),
								("Test Size (Big) Error", 	[0x00, 0x01, 0x00, 0x1F, 0x00, 0xFF, 0xFF], 	(True, 0x01, [0xBB, 0x02], True) ),
								("Test Size (Small) Error", [0x00, 0x01, 0x00, 0x10, 0x00, 0xFF], 			(True, 0x01, [0xBB, 0x03], True) ),
								("Test Key Error", 			[0x00, 0x01, 0x00, 0x11, 0xFF, 0xFF, 0x97], 	(True, 0x01, [0xBB, 0x04], True) ) ]


#---------------------------------------------------------------------
#                             CLASSES
#---------------------------------------------------------------------

# ==========================================================
# Terminal Colors
# ==========================================================
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

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
#                      MAIN FUNCTION
#---------------------------------------------------------------------
def main():
    Test = Test( bus=0, chip_select=0, currentModule=0x00, listOfModules=[0x00,0x01,0x02], testCases=test_case_group, dis_testCases = distructive_test_case_group, module_under_test = 0x02 )
    print(bcolors.HEADER + "Normal Test Cases:")
    Test.RunTestCases()
    print(bcolors.HEADER + "Distructive Test Cases:")
    Test.RunDistructiveTesting()

#---------------------------------------------------------------------
#                      MAIN TEST LOOP
#---------------------------------------------------------------------
if __name__ == "__main__":
  main()

