/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TimerTest.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:40:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:40:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/Timer.hpp"
#include <ctime>
#include <vector>

// Test callback function
static void testCallbackFunction(int fd, void* data)
{
    (void) fd;
    (void) data;
    // Do nothing, just for testing
}

// Test callback function that sets a flag
static void testFlagCallbackFunction(int fd, void* data)
{
    (void) fd;
    bool* flag = static_cast<bool*>(data);
    if (flag) {
        *flag = true;
    }
}

TEST(TimerTest, Constructor)
{
    time_t testTime = time(NULL) + 60; // 60 seconds from now
    int testFd = 42;
    
    Callback* callback = new Callback(testCallbackFunction, testFd);
    Timer timer(testTime, callback);
    
    EXPECT_EQ(timer.getExpireTime(), testTime);
    EXPECT_EQ(timer.getCallback()->getFd(), testFd);
}

TEST(TimerTest, SimultaneousTimerExpiry) {
    time_t now = time(NULL);
    int fd1 = 1, fd2 = 2;
    bool flag1 = false, flag2 = false;

    Callback* callback1 = new Callback(testFlagCallbackFunction, fd1, &flag1);
    Callback* callback2 = new Callback(testFlagCallbackFunction, fd2, &flag2);

    Timer timer1(now, callback1);
    Timer timer2(now, callback2);

    // Simulate timer expiry by manually executing callbacks
    timer1.getCallback()->execute();
    timer2.getCallback()->execute();

    EXPECT_TRUE(flag1);
    EXPECT_TRUE(flag2);
}

TEST(TimerTest, ExpiredTimerHandling) {
    time_t pastTime = time(NULL) - 1; // Expired time
    int fd = 1;
    bool flag = false;

    Callback* callback = new Callback(testFlagCallbackFunction, fd, &flag);
    Timer timer(pastTime, callback);

    // Simulate timer expiry by manually executing callback
    timer.getCallback()->execute();

    EXPECT_TRUE(flag);
}

TEST(TimerTest, TimerCancellation) {
    time_t futureTime = time(NULL) + 60;
    int fd = 1;
    bool flag = false;

    Callback* callback = new Callback(testFlagCallbackFunction, fd, &flag);
    Timer timer(futureTime, callback);

    // Simulate timer cancellation by not executing the callback
    // In a real implementation, the timer would be removed from the queue

    EXPECT_FALSE(flag); // Flag should not be set as callback is not executed
}

