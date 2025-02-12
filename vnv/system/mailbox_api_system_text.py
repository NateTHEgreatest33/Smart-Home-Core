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


#---------------------------------------------------------------------
#                            Defintions
#---------------------------------------------------------------------
class global_mbx(IntEnum):
    Float_Tx = 0
    Float_Rx = 1
    Int_Tx   = 2
    Int_Rx   = 3
    Bool_Tx  = 4
    Bool_Rx  = 5
    Async_Tx = 6
    Async_Rx = 7
    Rnd_5_Tx = 6
    Rnd_5_Rx = 7

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
 ]


#---------------------------------------------------------------------
#                             CLASSES
#---------------------------------------------------------------------
test_cases = [
#send index, set data, expect data, round_expected
]
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
		# Test Normal Cases
		#----------------------------------------------------------------------
		self.log.test_step( "Test supported commands")
		for [ x ] in [0,1]:
			#------------------------------------------------------------------
			# Setup Test Case
			#------------------------------------------------------------------
			self.log.test_step( "Test \'{}\' command is responded to correctly".format( x ) )

			self.log.compare_equal( 1, 0, "Verify command returns as expected" )





#---------------------------------------------------------------------
#                      MAIN FUNCTION
#---------------------------------------------------------------------
def main():
	log = results( __file__ )
	Pico = pi_pico( test_mode=False )
	mailbox = Mailbox( msg_conn = Pico.msg_conn, glb_mailbox = [] )
	test = Test( log, Pico, mailbox )

	test.run()
#---------------------------------------------------------------------
#                      MAIN TEST LOOP SWRS0001
#---------------------------------------------------------------------
if __name__ == "__main__":
  main()

