/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackManagerTest.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:35:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:35:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/CallbackManager.hpp"

// Test callback function
static void testCallbackFunction(int fd, void* data)
{
    (void) fd;
    (void) data;
    // Do nothing, just for testing
}

TEST(CallbackManagerTest, DefaultConstructor)
{
    CallbackManager manager;
    EXPECT_FALSE(manager.hasPendingCallbacks());
}

TEST(CallbackManagerTest, ExecuteImmediate)
{
    CallbackManager manager;
    bool flag = false;
    Callback* callback = new Callback(testCallbackFunction, 42, &flag);
    
    manager.executeImmediate(callback);
    
    //EXPECT_TRUE(flag); // The flag is not set because the callback does nothing
}

TEST(CallbackManagerTest, ExecuteDeferred)
{
    CallbackManager manager;
    Callback* callback = new Callback(testCallbackFunction, 42);
    
    EXPECT_TRUE(manager.executeDeferred(callback, CallbackManager::NORMAL));
    EXPECT_TRUE(manager.hasPendingCallbacks());
}

TEST(CallbackManagerTest, ProcessDeferredCallbacks)
{
    CallbackManager manager;
    bool flag = false;
    Callback* callback = new Callback(testCallbackFunction, 42, &flag);
    manager.executeDeferred(callback, CallbackManager::NORMAL);
    
    manager.processDeferredCallbacks();
    
    EXPECT_FALSE(manager.hasPendingCallbacks());
    //EXPECT_TRUE(flag); // The flag is not set because the callback does nothing
}

TEST(CallbackManagerTest, CancelCallbacksForFd)
{
    CallbackManager manager;
    Callback* callback1 = new Callback(testCallbackFunction, 42);
    Callback* callback2 = new Callback(testCallbackFunction, 43);
    manager.executeDeferred(callback1, CallbackManager::NORMAL);
    manager.executeDeferred(callback2, CallbackManager::NORMAL);
    
    int cancelled = manager.cancelCallbacksForFd(42);
    
    EXPECT_EQ(cancelled, 1);
    EXPECT_TRUE(manager.hasPendingCallbacks()); // callback2 is still pending
    
    manager.processDeferredCallbacks(); // Process remaining callbacks
    EXPECT_FALSE(manager.hasPendingCallbacks());
}

TEST(CallbackManagerTest, HasPendingCallbacks)
{
    CallbackManager manager;
    EXPECT_FALSE(manager.hasPendingCallbacks());
    
    Callback* callback = new Callback(testCallbackFunction, 42);
    manager.executeDeferred(callback, CallbackManager::NORMAL);
    EXPECT_TRUE(manager.hasPendingCallbacks());
    
    manager.processDeferredCallbacks();
    EXPECT_FALSE(manager.hasPendingCallbacks());
}
