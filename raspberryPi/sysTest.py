from test import testingAPI

#Test cases
#					  test_description		  		 send_data													  dest  expected data
test_case_group = [ ( "Test 1 byte message",  		[0xFF],															0x01, [0xAA, 0x11] 	),
					( "Test 10 byte message", 		[0x01, 	0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A],  0x01, [0xAA, 0x22]  ),
					( "Send 0 byte message",  		[], 															0x01, [0xBB, 0x03] 	),
					( "send to unsupported module", [0xFF],															0x05, []			) ]

#								(description,    			[dest, src,  pad, ver/size, key, DATA... crc],  (True, source, data, valid) )
distructive_test_case_group = [ ("Test CRC Error", 			[0x00, 0x01, 0x00, 0x11, 0x00, 0xFF, 0xFF], 	(True, 0x01, [0xBB, 0x01], True) ),
								("Test Size (Big) Error", 	[0x00, 0x01, 0x00, 0x1F, 0x00, 0xFF, 0xFF], 	(True, 0x01, [0xBB, 0x02], True) ),
								("Test Size (Small) Error", [0x00, 0x01, 0x00, 0x10, 0x00, 0xFF], 			(True, 0x01, [0xBB, 0x03], True) ),
								("Test Key Error", 			[0x00, 0x01, 0x00, 0x11, 0xFF, 0xFF, 0x97], 	(True, 0x01, [0xBB, 0x04], True) ) ]


#Test API
Test = testingAPI( bus=0, chip_select=0, currentModule=0x00, listOfModules=[0x00,0x01], testCases=test_case_group, dis_testCases = distructive_test_case_group )
print(bcolors.HEADER + "Normal Test Cases:")
Test.RunTestCases()
print(bcolors.HEADER + "Distructive Test Cases:")
Test.RunDistructiveTesting()