/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueueAdditionalTest.cpp                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 00:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/19 00:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/CallbackQueue.hpp"
#include <vector>

// Global variables to track execution order
static std::vector<int> executionOrder;
static int callbackCounter = 0;

// Test callback function that records execution order
static void orderTrackingCallback(int fd, void* data)
{
    (void)data;
    executionOrder.push_back(fd);
}

// Test callback function that increments a counter
static void counterCallback(int fd, void* data)
{
    (void)fd;
    (void)data;
    callbackCounter++;
}

// Test for cancelled callback execution
TEST(CallbackQueueAdditionalTest, CancelledCallbackExecution)
{
    CallbackQueue queue;
    
    // Create two callbacks, one of which will be cancelled
    Callback* callback1 = new Callback(counterCallback, 1);
    Callback* callback2 = new Callback(counterCallback, 2);
    
    // Add both callbacks to the queue
    queue.addCallback(callback1);
    queue.addCallback(callback2);
    
    // Cancel the second callback
    queue.cancelCallbacksForFd(2);
    
    // Reset the counter
    callbackCounter = 0;
    
    // Process the callbacks
    queue.processCallbacks();
    
    // Only one callback should have executed
    EXPECT_EQ(callbackCounter, 1);
}

// Test for massive queue handling
TEST(CallbackQueueAdditionalTest, MassiveQueueHandling)
{
    CallbackQueue queue;
    const int NUM_CALLBACKS = 500; // Large number but not too large to slow down tests
    
    // Reset the counter
    callbackCounter = 0;
    
    // Add many callbacks to the queue
    for (int i = 0; i < NUM_CALLBACKS; i++)
    {
        Callback* callback = new Callback(counterCallback, i);
        EXPECT_TRUE(queue.addCallback(callback));
    }
    
    // Verify queue size
    EXPECT_EQ(queue.size(), NUM_CALLBACKS);
    
    // Process all callbacks
    queue.processCallbacks();
    
    // Verify all callbacks were executed
    EXPECT_EQ(callbackCounter, NUM_CALLBACKS);
    
    // Verify queue is empty after processing
    EXPECT_TRUE(queue.isEmpty());
}

// Test for queue processing order (FIFO)
TEST(CallbackQueueAdditionalTest, QueueProcessingOrder)
{
    CallbackQueue queue;
    const int NUM_CALLBACKS = 5;
    
    // Clear the execution order tracking
    executionOrder.clear();
    
    // Add callbacks with different fd values to track order
    for (int i = 1; i <= NUM_CALLBACKS; i++)
    {
        Callback* callback = new Callback(orderTrackingCallback, i);
        queue.addCallback(callback);
    }
    
    // Process all callbacks
    queue.processCallbacks();
    
    // Verify callbacks were executed in FIFO order
    ASSERT_EQ(executionOrder.size(), NUM_CALLBACKS);
    for (int i = 0; i < NUM_CALLBACKS; i++)
    {
        EXPECT_EQ(executionOrder[i], i + 1);
    }
}
