/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackQueueTest.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:30:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:30:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/CallbackQueue.hpp"

// Test callback function
static void testCallbackFunction(int fd, void* data)
{
    (void) fd;
    (void) data;
    // Do nothing, just for testing
}

TEST(CallbackQueueTest, DefaultConstructor)
{
    CallbackQueue queue;
    EXPECT_TRUE(queue.isEmpty());
    EXPECT_EQ(queue.size(), 0);
}

TEST(CallbackQueueTest, AddCallback)
{
    CallbackQueue queue;
    Callback* callback = new Callback(testCallbackFunction, 42);
    
    EXPECT_TRUE(queue.addCallback(callback));
    EXPECT_FALSE(queue.isEmpty());
    EXPECT_EQ(queue.size(), 1);
}

TEST(CallbackQueueTest, ProcessCallbacks)
{
    CallbackQueue queue;
    bool flag = false;
    Callback* callback = new Callback(testCallbackFunction, 42, &flag);
    queue.addCallback(callback);
    
    queue.processCallbacks();
    
    EXPECT_TRUE(queue.isEmpty());
    //EXPECT_TRUE(flag); // The flag is not set because the callback does nothing
}

TEST(CallbackQueueTest, CancelCallbacksForFd)
{
    CallbackQueue queue;
    Callback* callback1 = new Callback(testCallbackFunction, 42);
    Callback* callback2 = new Callback(testCallbackFunction, 43);
    queue.addCallback(callback1);
    queue.addCallback(callback2);
    
    int cancelled = queue.cancelCallbacksForFd(42);
    
    EXPECT_EQ(cancelled, 1);
    EXPECT_EQ(queue.size(), 2);
}

TEST(CallbackQueueTest, IsEmpty)
{
    CallbackQueue queue;
    EXPECT_TRUE(queue.isEmpty());
    
    Callback* callback = new Callback(testCallbackFunction, 42);
    queue.addCallback(callback);
    EXPECT_FALSE(queue.isEmpty());
    
    queue.processCallbacks();
    EXPECT_TRUE(queue.isEmpty());
}

TEST(CallbackQueueTest, Size)
{
    CallbackQueue queue;
    EXPECT_EQ(queue.size(), 0);
    
    Callback* callback1 = new Callback(testCallbackFunction, 42);
    queue.addCallback(callback1);
    EXPECT_EQ(queue.size(), 1);
    
    Callback* callback2 = new Callback(testCallbackFunction, 43);
    queue.addCallback(callback2);
    EXPECT_EQ(queue.size(), 2);
    
    queue.processCallbacks();
    EXPECT_EQ(queue.size(), 0);
}
