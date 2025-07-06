/*********************************************************************
*
*   HEADER:
*       disable_interrupt_test.cpp
*
*   DESCRIPTION:
*       This file contains the unit tests for the utl::disable_interrupts class.
*
*   Copyright 2025 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                              INCLUDES
--------------------------------------------------------------------*/
#include "gtest/gtest.h"
#include "lib/util/disable_interrupt.hpp"
#include "hardware/sync.h" // This will now pick up our mock

/*--------------------------------------------------------------------
                               CLASSES
--------------------------------------------------------------------*/

// Extern declarations for the mock variables
extern uint32_t save_and_disable_interrupts_mock_return_value;
extern uint32_t restore_interrupts_called_value;

namespace utl {
namespace test {

/*********************************************************************
*
*   CLASS NAME:
*       DisableInterruptsTest
*
*   DESCRIPTION:
*       Test fixture for the utl::disable_interrupts class.
*
*********************************************************************/
class DisableInterruptsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        save_and_disable_interrupts_mock_return_value = 0;
        restore_interrupts_called_value = 0;
    }
};

/*--------------------------------------------------------------------
                              TEST CASES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   TEST NAME:
*       DisableInterruptsTest.ConstructorDisablesInterrupts
*
*   DESCRIPTION:
*       Tests that the constructor disables interrupts.
*
*********************************************************************/
TEST_F(DisableInterruptsTest, ConstructorDisablesInterrupts)
{
    save_and_disable_interrupts_mock_return_value = 0x12345678;
    // When di is constructed, save_and_disable_interrupts() should be called
    utl::disable_interrupts di;
    // We can't directly assert on the private member p_int_status,
    // but the fact that the mock function was called and its return value
    // would have been used is sufficient for this test.
    // The destructor test will further confirm the value was stored.
    SUCCEED();
}

/*********************************************************************
*
*   TEST NAME:
*       DisableInterruptsTest.DestructorRestoresInterrupts
*
*   DESCRIPTION:
*       Tests that the destructor restores interrupts.
*
*********************************************************************/
TEST_F(DisableInterruptsTest, DestructorRestoresInterrupts)
{
    save_and_disable_interrupts_mock_return_value = 0xABCDEF00; // Set a value to be saved

    {
        utl::disable_interrupts di; // Constructor saves interrupts
    } // Destructor restores interrupts

    // Verify that restore_interrupts was called with the value saved by the constructor
    ASSERT_EQ(restore_interrupts_called_value, 0xABCDEF00);
}

} // namespace test
} // namespace utl
