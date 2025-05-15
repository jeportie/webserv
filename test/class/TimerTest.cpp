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

// Test callback function
static void testCallbackFunction(int fd, void* data)
{
	(void) fd;
	(void) data;
    // Do nothing, just for testing
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
