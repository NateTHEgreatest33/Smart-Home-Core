#*********************************************************************
#
#   MODULE NAME:
#       results.py - results handler class
#
#   DESCRIPTION:
#       Provides results functionality
#
#   Copyright 2023 by Nate Lenze
#*********************************************************************

#---------------------------------------------------------------------
#                              IMPORTS
#---------------------------------------------------------------------
import os
import git
from datetime import datetime


#---------------------------------------------------------------------
#                          HELPER CLASSES
#---------------------------------------------------------------------
class consoleColor:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

#---------------------------------------------------------------------
#                             CLASSES
#---------------------------------------------------------------------
class results:
    # ================================
    # local variables
    # ================================
    __text_replacement = [ ( "\r",               "\\r" ),
                           ( "\n",               "\\n" ),
                           ( "\x1b[2J\x1b[0;0H", "[clear screen]" ) ]

    # ================================
    # results constructor
    # ================================
    def __init__(self, fut ):
        self.file_under_test = fut
        self.date_time       = datetime.now()
        self.checksum        = ( git.Repo(search_parent_directories=True) ).head.object.hexsha
        self.test_case_list  = []
        self.req_list        = []

    # ================================
    # results deconstructor
    # ================================
    def __del__(self):
        self.__publish_results()

    # ================================
    # requirement
    # ================================
    def test_requirement( self, req ):
        self.req_list.append( req )
        print( consoleColor.HEADER + "Req Tested > "+ req )
    # ================================
    # test step
    # ================================
    def test_step( self, step ):
         self.__global_compare( True, "step", 0, 0, step )
    # ================================
    # compare equal
    # ================================
    def compare_equal( self, expected, actual, case ):
        cmp = expected == actual
        self.__global_compare( cmp, "==", expected, actual, case )

    # ================================
    # compare not equal 
    # ================================ 
    def compare_not_equal( self, expected, actual, case ):
        cmp = expected != actual
        self.__global_compare( cmp, "!=", expected, actual, case )

    # ================================
    # compare less than
    # ================================ 
    def compare_less_than( self, expected, actual, case ):
        cmp = expected < actual
        self.__global_compare( cmp, "<", expected, actual, case )

    # ================================
    # compare more than
    # ================================
    def compare_more_than( self, expected, actual, case ):
        cmp = expected > actual
        self.__global_compare( cmp, ">", expected, actual, case )

    # ================================
    # global compare function
    # ================================
    def __global_compare( self, result, cmp_type, x, y, case ):
        self.test_case_list.append( [result, cmp_type, x, y, case ] )
        self.__console( result, cmp_type, x, y, case )
    
    def __console( self, result, cmp_type, x, y, case ):
        if cmp_type == "step":
            print( consoleColor.HEADER + case )
            return
        print( consoleColor.ENDC + case + ": " + self.__text_cleanser(x) + " " + cmp_type + " " + self.__text_cleanser(y) )
        color = consoleColor.OKGREEN if result is True else consoleColor.FAIL
        print( color + str(result) )

    # ================================
    # text cleanser
    # ================================
    def __text_cleanser( self, item ):
        text = str(item)
        
        for orginal, replace in self.__text_replacement:
            text = text.replace( orginal, replace ) 
	
        return text


    # ================================
    # publish results
    # ================================
    def __publish_results( self ):
        # ============================
        # initailize global pass/fail
        # to true to start test
        # ============================
        pass_fail = True

        # ============================
        # format & open results file
        # ============================
        results_file = self.file_under_test[:-3] + "_results.txt"
        f = open(results_file, "w")

        # ============================
        # add general header info
        # ============================
        f.writelines( "file under test: {}\n".format(os.path.basename(self.file_under_test)) )
        f.writelines( "current test:    {}\n".format(os.path.basename(results_file) ) )
        f.writelines( "FUT sha:         {}\n".format(self.checksum ) )
        f.writelines( "date:            {}\n".format(self.date_time.ctime()) )

        f.writelines( "\nRequirements Tested: \n" )
        for req in self.req_list:
            f.writelines( req + "\n" )

        # ============================
        # run through test-cases
        # ============================
        f.writelines( "\nTest Cases: \n" )
        for [result, cmp_type, x, y, case ] in self.test_case_list:
            # =========================
            # Special handling for step's
            # =========================
            if cmp_type == "step":
                f.writelines("> " + case + "\n")
                continue

            # =========================
            # normal handling
            # =========================
            pass_fail = pass_fail and result
            test_case_str = case + ": " + self.__text_cleanser(x) + " " + cmp_type + " " + self.__text_cleanser(y) + "\nRESULT: " + str( result ) + "\n"
            f.writelines(test_case_str )

        # ============================
        # Print overal result to file
        # ============================
        f.writelines( "\n\nOverall Result:  {}\n".format( str(pass_fail) ) )

        # ============================
        # Print overal result to console
        # ============================
        print( consoleColor.ENDC + "\nOverall Result: ")
        color = consoleColor.OKGREEN if pass_fail is True else consoleColor.FAIL
        print( color + str(pass_fail) )

        # ============================
        # close file
        # ============================
        f.close()
