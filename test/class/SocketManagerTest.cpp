/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManagerTest.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:02:45 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:03:12 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/SocketManager.hpp"

TEST(SocketManagerTest, DefaultConstructor)
{
    SocketManager manager;
    EXPECT_EQ(manager.getServerSocket(), -1);
    EXPECT_EQ(manager.getClientSocket(), -1);
}

// Note: It's difficult to properly test SocketManager without mocking
// the socket and epoll functions, which is beyond the scope of a simple test.
// The following tests focus on the methods that don't require actual networking.

// Test callback function
static void testCallbackFunction(int fd, void* data)
{
    (void) fd;
    (void) data;
    // Do nothing, just for testing
}

TEST(SocketManagerTest, ExecuteImmediate)
{
    SocketManager manager;
    Callback* callback = new Callback(testCallbackFunction, 42);
    
    // This should not crash
    manager.executeImmediate(callback);
    
    // No need to delete callback, executeImmediate takes ownership
}

TEST(SocketManagerTest, ExecuteDeferred)
{
    SocketManager manager;
    Callback* callback = new Callback(testCallbackFunction, 42);
    
    // This should not crash
    manager.executeDeferred(callback);
    
    // Process the callbacks
    manager.processDeferredCallbacks();
}

TEST(SocketManagerTest, CancelCallbacksForFd)
{
    SocketManager manager;
    Callback* callback1 = new Callback(testCallbackFunction, 42);
    Callback* callback2 = new Callback(testCallbackFunction, 43);
    
    manager.executeDeferred(callback1);
    manager.executeDeferred(callback2);
    
    // Cancel callbacks for fd 42
    int cancelled = manager.cancelCallbacksForFd(42);
    
    // Check that we cancelled 1 callback
    EXPECT_EQ(cancelled, 1);
    
    // Process the callbacks
    manager.processDeferredCallbacks();
}
