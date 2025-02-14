#*********************************************************************
#
#   MODULE NAME:
#       mailbox_api_system_test.py - Mailbox System Test
#
#   DESCRIPTION:
#       Verify Mailbox API functionality
#
#   Copyright 2025 by Nate Lenze
#*********************************************************************

#---------------------------------------------------------------------
#                              IMPORTS
#---------------------------------------------------------------------
from lib.results import results
from lib.mailbox import Mailbox, modules, special_response, mailbox_idx
from lib.pi_pico import pi_pico

from enum import IntEnum
import time

#---------------------------------------------------------------------
#                            Defintions
#---------------------------------------------------------------------
class global_mbx(IntEnum):
    Float_Tx    = 0
    Float_Rx    = 1
    Int_Tx      = 2
    Int_Rx      = 3
    Bool_Tx     = 4
    Bool_Rx     = 5
    Async_Tx    = 6
    Async_Rx    = 7
    Rnd_5_Tx    = 6
    Rnd_5_Rx    = 7
    Test_Tx     = 8
    Test_Rx     = 9

#---------------------------------------------------------------------
#                      TEST CASE VARIABLES
#---------------------------------------------------------------------
global_mailbox = [
# data,  type,    flag,  dir,  src,                 dest
[ 0.0,   '1',     False, 'TX', modules.RPI_MODULE,  modules.PICO_MODULE ], # float TX message
[ 0.0,   '1',     False, 'RX', modules.PICO_MODULE, modules.RPI_MODULE  ], # float RX message
[ 0,     '1',     False, 'TX', modules.RPI_MODULE,  modules.PICO_MODULE ], # int TX message
[ 0,     '1',     False, 'RX', modules.PICO_MODULE, modules.RPI_MODULE  ], # int RX message
[ False, '1',     False, 'TX', modules.RPI_MODULE,  modules.PICO_MODULE ], # bool TX message
[ False, '1',     False, 'RX', modules.PICO_MODULE, modules.RPI_MODULE  ], # bool RX message
[ 0,     'ASYNC', False, 'TX', modules.RPI_MODULE,  modules.PICO_MODULE ], # Async - int TX message
[ 0,     'ASYNC', False, 'RX', modules.PICO_MODULE, modules.RPI_MODULE  ], # Async - int RX message
[ 0,     '5',     False, 'TX', modules.RPI_MODULE,  modules.PICO_MODULE ], # RND 5 - int TX message
[ 0,     '5',     False, 'RX', modules.PICO_MODULE, modules.RPI_MODULE  ], # RND 5 - int RX message
[ 0,     'ASYNC', False, 'RX', modules.PICO_MODULE, modules.RPI_MODULE  ], # Test Results RX
[ 0,     'ASYNC', False, 'TX', modules.RPI_MODULE,  modules.PICO_MODULE ], # Test Results TX
 ]

test_cases_tx = [
# send index,          Tx Data, Rx Data, round_expected, test string 
[ global_mbx.Float_Tx, 5.5,     5.5,     1,              "Test Sending float data"     ],
[ global_mbx.Int_Tx,   5,       5,       1,              "Test Receiving int data"     ],
[ global_mbx.Bool_Tx,  True,    True,    1,              "Test Sending boolean data"   ],
[ global_mbx.Async_Tx, 10,      10,      1,              "Test Sending async data"     ],
[ global_mbx.Rnd_5_Tx, 15,      15,      5,              "Test Sending rnd 5 data"     ],
]

test_cases_rx = [
# send index,          Tx Data, Rx Data, round_expected, test string 
[ global_mbx.Float_Rx, 20,      5.5,     1,              "Test Receiving float data"   ],
[ global_mbx.Int_Rx,   21,      5,       1,              "Test Receiving int data"     ],
[ global_mbx.Bool_Rx,  22,      True,    1,              "Test Receiving boolean data" ],
[ global_mbx.Async_Rx, 23,      10,      5,              "Test Receiving async data"   ],
[ global_mbx.Rnd_5_Rx, 24,      5,       1,              "Test Receiving rnd 5 data"   ]

]
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
	def __init__(self, result_obj, pico, mailbox ):
		self.log     = result_obj
		self.pico    = pico
		self.mailbox = mailbox

		#load custom mailbox sw onto board
		self.pico.load_software("elfPath")

    # ==================================
    # run()
    # ==================================
	def run(self):
		self.log.test_requirement( "x" )
		self.run_cases()

    # ==================================
    # run_cases()
    # ==================================
	def run_cases(self):
		#----------------------------------------------------------------------
		# Test TX Cases
		#----------------------------------------------------------------------
		self.log.test_step( "Test TX test cases")
		for mbx_index, tx_data, rx_data, within_round, test_case  in test_cases_tx:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( "{} is properly handled".format(test_case) )
			self.mailbox.set_data( mbx_index, tx_data )
			#------------------------------------------------------------------
			# Run Mailbox for rounds required for Rx/Tx
			#------------------------------------------------------------------
			self.__run_mailbox_for( within_round )

			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			actual_data = self.mailbox.mailbox_map[ global_mbx.Test_Rx ][mailbox_idx.DATA]
			self.log.compare_equal( expected=rx_data, actual=actual_data case="Verify data return matches expected" )


		#----------------------------------------------------------------------
		# Test RX Cases
		#----------------------------------------------------------------------
		self.log.test_step( "Test RX test cases")
		for mbx_index, tx_data, rx_data, within_round, test_case  in test_cases_tx:
			#------------------------------------------------------------------
			# Setup Test Case & set Tx Data
			#------------------------------------------------------------------
			self.log.test_step( "{} is properly handled".format(test_case) )
			self.mailbox.set_data( global_mbx.Text_Tx, tx_data )

			#------------------------------------------------------------------
			# Run Mailbox for rounds required for Rx/Tx
			#------------------------------------------------------------------
			self.__run_mailbox_for( within_round )

			#------------------------------------------------------------------
			# Verify Rx Data is as expected
			#------------------------------------------------------------------
			actual_data = self.mailbox.mailbox_map[ mbx_index ][mailbox_idx.DATA]
			self.log.compare_equal( expected=rx_data, actual=actual_data case="Verify data return matches expected" )

		#----------------------------------------------------------------------
		# destructive testing?
		#----------------------------------------------------------------------
		
		#should we test Acks?
		#round updates?

    # ==================================
    # __run_mailbox_for()
    # ==================================
	def __run_mailbox_for( self, rounds ):
		current_round = self.mailbox.round_counter
		#add extra +1 so that RX can transmit a response
		end_round = ( current_round + 1 + rounds ) % 100
		while( current_round != end_round ):
			time.sleep(1)
			self.mailbox.runtime()
			current_round = self.mailbox.round_counter

#---------------------------------------------------------------------
#                      MAIN FUNCTION
#---------------------------------------------------------------------
def main():
	log = results( __file__ )
	Pico = pi_pico( test_mode=False )
	mailbox = Mailbox( msg_conn = Pico.msg_conn, glb_mailbox = global_mailbox )
	test = Test( log, Pico, mailbox )

	test.run()
#---------------------------------------------------------------------
#                      MAIN TEST LOOP SWRS0001
#---------------------------------------------------------------------
if __name__ == "__main__":
  main()

