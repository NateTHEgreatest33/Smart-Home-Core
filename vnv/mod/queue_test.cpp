/*********************************************************************
*
*   HEADER:
*       queue_test.cpp
*
*   DESCRIPTION:
*       This file contains the unit tests for the utl::queue class.
*
*   Copyright 2025 Nate Lenze
*
*********************************************************************/

/*--------------------------------------------------------------------
                              INCLUDES
--------------------------------------------------------------------*/
#include "gtest/gtest.h"
#include "lib/util/queue.hpp"

/*--------------------------------------------------------------------
                               CLASSES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   CLASS NAME:
*       QueueTest
*
*   DESCRIPTION:
*       Test fixture for the utl::queue class.
*
*********************************************************************/
class QueueTest : public ::testing::Test 
{
protected:
    utl::queue<5, int> q;
};

/*--------------------------------------------------------------------
                              TEST CASES
--------------------------------------------------------------------*/

/*********************************************************************
*
*   TEST NAME:
*       QueueTest.IsEmpty
*
*   DESCRIPTION:
*       Tests the is_empty() method.
*
*********************************************************************/
TEST_F(QueueTest, IsEmpty) 
{
    EXPECT_TRUE(q.is_empty());
    q.push(1);
    EXPECT_FALSE(q.is_empty());
}

/*********************************************************************
*
*   TEST NAME:
*       QueueTest.IsFull
*
*   DESCRIPTION:
*       Tests the is_full() method.
*
*********************************************************************/
TEST_F(QueueTest, IsFull) 
{
    EXPECT_FALSE(q.is_full());
    for (int i = 0; i < 5; ++i) 
    {
        q.push(i);
    }
    EXPECT_TRUE(q.is_full());
}

/*********************************************************************
*
*   TEST NAME:
*       QueueTest.PushAndFront
*
*   DESCRIPTION:
*       Tests the push() and front() methods.
*
*********************************************************************/
TEST_F(QueueTest, PushAndFront) 
{
    q.push(10);
    EXPECT_EQ(q.front(), 10);
    q.push(20);
    EXPECT_EQ(q.front(), 10); // Front should still be 10
}

/*********************************************************************
*
*   TEST NAME:
*       QueueTest.Pop
*
*   DESCRIPTION:
*       Tests the pop() method.
*
*********************************************************************/
TEST_F(QueueTest, Pop) 
{
    q.push(1);
    q.push(2);
    q.pop();
    EXPECT_EQ(q.front(), 2);
}

/*********************************************************************
*
*   TEST NAME:
*       QueueTest.Size
*
*   DESCRIPTION:
*       Tests the size() method.
*
*********************************************************************/
TEST_F(QueueTest, Size) 
{
    EXPECT_EQ(q.size(), 0);
    for (int i = 0; i < 3; ++i) 
    {
        q.push(i);
    }
    EXPECT_EQ(q.size(), 3);
}

/*********************************************************************
*
*   TEST NAME:
*       QueueTest.Empty
*
*   DESCRIPTION:
*       Tests the empty() method.
*
*********************************************************************/
TEST_F(QueueTest, Empty) 
{
    q.push(1);
    q.push(2);
    q.empty();
    EXPECT_TRUE(q.is_empty());
    EXPECT_EQ(q.size(), 0);
}

/*********************************************************************
*
*   TEST NAME:
*       QueueTest.Rollover
*
*   DESCRIPTION:
*       Tests the queue's rollover behavior.
*
*********************************************************************/
TEST_F(QueueTest, Rollover) 
{
    for (int i = 0; i < 5; ++i) 
    {
        q.push(i);
    }
    EXPECT_TRUE(q.is_full());
    q.pop();
    q.pop();
    // At this point, front is at index 2, tail is at index 0
    q.push(5);
    q.push(6);
    EXPECT_EQ(q.front(), 2);
    q.pop();
    EXPECT_EQ(q.front(), 3);
    q.pop();
    EXPECT_EQ(q.front(), 4);
    q.pop();
    EXPECT_EQ(q.front(), 5);
    q.pop();
    EXPECT_EQ(q.front(), 6);
}