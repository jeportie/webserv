/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackManagerAdditionalTest.cpp                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 00:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/19 00:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/CallbackManager.hpp"
#include <vector>
#include <stdexcept>

// Global variables to track execution order
static std::vector<int> executionOrder;

// Test callback function that records execution order
static void orderTrackingCallback(int fd, void* data)
{
    (void)data;
    executionOrder.push_back(fd);
}

// Test callback function that throws an exception
static void exceptionThrowingCallback(int fd, void* data)
{
    (void)fd;
    (void)data;
    throw std::runtime_error("Test exception");
}

// Test callback function that adds another callback to the manager
static void callbackAddingCallback(int fd, void* data)
{
    (void)fd;
    CallbackManager* manager = static_cast<CallbackManager*>(data);
    if (manager)
    {
        // Add a new callback to the manager
        Callback* newCallback = new Callback(orderTrackingCallback, 999);
        manager->executeDeferred(newCallback, CallbackManager::NORMAL);
    }
}

// Test for priority handling
TEST(CallbackManagerAdditionalTest, PriorityHandling)
{
    CallbackManager manager;
    
    // Clear the execution order tracking
    executionOrder.clear();
    
    // Add callbacks with different priorities
    Callback* lowCallback = new Callback(orderTrackingCallback, 3);
    Callback* normalCallback = new Callback(orderTrackingCallback, 2);
    Callback* highCallback = new Callback(orderTrackingCallback, 1);
    
    // Add them in reverse order of priority
    manager.executeDeferred(lowCallback, CallbackManager::LOW);
    manager.executeDeferred(normalCallback, CallbackManager::NORMAL);
    manager.executeDeferred(highCallback, CallbackManager::HIGH);
    
    // Process all callbacks
    manager.processDeferredCallbacks();
    
    // Verify callbacks were executed in priority order (HIGH, NORMAL, LOW)
    ASSERT_EQ(executionOrder.size(), 3);
    EXPECT_EQ(executionOrder[0], 1); // HIGH priority
    EXPECT_EQ(executionOrder[1], 2); // NORMAL priority
    EXPECT_EQ(executionOrder[2], 3); // LOW priority
}

// Test for immediate callback exception handling
TEST(CallbackManagerAdditionalTest, ImmediateCallbackExceptionHandling)
{
    CallbackManager manager;
    
    // Create a callback that throws an exception
    Callback* exceptionCallback = new Callback(exceptionThrowingCallback, 42);
    
    // Executing the callback should not crash the test
    EXPECT_NO_THROW(manager.executeImmediate(exceptionCallback));
    
    // The manager should have handled the exception internally
    // We can't directly test the error logging, but we can verify the program continues
}

// Test for deferred callbacks edge cases (concurrent modification)
TEST(CallbackManagerAdditionalTest, DeferredCallbacksConcurrentModification)
{
    CallbackManager manager;
    
    // Clear the execution order tracking
    executionOrder.clear();
    
    // Add a callback that will add another callback to the manager during execution
    Callback* addingCallback = new Callback(callbackAddingCallback, 42, &manager);
    
    // Add a regular callback
    Callback* regularCallback = new Callback(orderTrackingCallback, 100);
    
    // Add them to the manager
    manager.executeDeferred(addingCallback, CallbackManager::NORMAL);
    manager.executeDeferred(regularCallback, CallbackManager::NORMAL);
    
    // Add a new callback directly to verify it's processed in this batch
    Callback* newCallback = new Callback(orderTrackingCallback, 999);
    manager.executeDeferred(newCallback, CallbackManager::NORMAL);
    
    // Process all callbacks
    manager.processDeferredCallbacks();
    
    // The execution order should include all callbacks
    // Note: The actual behavior depends on the implementation of CallbackManager
    // In this case, it appears that the callback added during processing is also executed
    // in the same batch, which is why we have 3 entries instead of 2
    ASSERT_EQ(executionOrder.size(), 3);
    
    // The order should be:
    // 1. The callback that adds another callback (42) - but this doesn't add to executionOrder
    // 2. The regular callback (100)
    // 3. The directly added callback (999)
    // 4. The callback added during processing (999)
    EXPECT_EQ(executionOrder[0], 100);
    EXPECT_EQ(executionOrder[1], 999);
    EXPECT_EQ(executionOrder[2], 999);
    
    // Manager should now be empty
    EXPECT_FALSE(manager.hasPendingCallbacks());
}
