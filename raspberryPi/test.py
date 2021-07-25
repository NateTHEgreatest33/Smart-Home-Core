from messageAPI import messageAPI
from enum import Enum
import time

testIndex = ( 'test_description', 'send_data', 'destination', 'expected_data' )
dTestIndex = ( 'test_description', 'raw_send', 'expected_data')

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

class testingAPI:
	def __init__(self, bus, chip_select, currentModule, listOfModules, testCases, dis_testCases):
		
		#setup local var's
		self.module = messageAPI( bus=bus, chip_select=chip_select, currentModule=currentModule, listOfModules=listOfModules )
		self.module.InitAPI()
		self.TestCases = testCases
		self.DistructiveTestCases = dis_testCases
		self.module.debug_prints = False

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


	#for test cases that cannot use the messageAPI (errors)
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