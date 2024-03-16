
/*********************************************************************
*
*   NAME:
*       test_mode.cpp
*
*   DESCRIPTION:
*       Test mode handles all changes that are needed to adequately
*       test module functionality. This is seperated into a seperate
*       file in order to keep changes isolated from main.cpp
*
*   Copyright 2024 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                              INCLUDES
--------------------------------------------------------------------*/
#include "test_mode.hpp"

/*--------------------------------------------------------------------
                          GLOBAL NAMESPACES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                          LITERAL CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                TYPES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                           MEMORY CONSTANTS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              VARIABLES
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                              EXTERNS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                MACROS
--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
                                EXTERNS
--------------------------------------------------------------------*/
extern bool g_test_mode_enable;

/*--------------------------------------------------------------------
                              PROCEDURES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   PROCEDURE NAME:
*       test_mode_runtime
*
*   DESCRIPTION:
*       handle all test mode changes
*
*********************************************************************/
static void test_mode_runtime
    (
    core::console& c
    )
{

} /* test_mode_runtime() */

/*********************************************************************
*
*   PROCEDURE NAME:
*       test_mode_powerdown
*
*   DESCRIPTION:
*       handle disablement of test mode
*
*********************************************************************/
static void test_mode_powerdown
    (
    core::console& c
    )
{
    
} /*test_mode_powerdown() */


/*********************************************************************
*
*   PROCEDURE NAME:
*       test_mode_runtime
*
*   DESCRIPTION:
*       handle all test mode changes
*
*********************************************************************/
void test_mode
    (
    bool test_mode,             /* flag for if test mode is enabled */
    core::console& c            /* reference to global console      */
    )
{
/*----------------------------------------------------------
Local Variables
----------------------------------------------------------*/
static bool test_mode_prev = false;

/*----------------------------------------------------------
Exit if test mode is not currently enabled and was not enabled
on last run of this function. This is done to allow the power
down function to be run on test mode exit
----------------------------------------------------------*/
if( test_mode == false && test_mode_prev == false )
    return;

/*----------------------------------------------------------
if test mode is currently enabled, run runtime changes
----------------------------------------------------------*/
if( test_mode )
    test_mode_runtime( c );

/*----------------------------------------------------------
If test mode is disabled but was prevously enabled on the
last run of this function, run powerdown function. 
----------------------------------------------------------*/
if( test_mode == false && test_mode_prev == true )
    test_mode_powerdown( c );

/*----------------------------------------------------------
update local copy of test_mode
----------------------------------------------------------*/
test_mode_prev = test_mode;

} /* test_mode() */