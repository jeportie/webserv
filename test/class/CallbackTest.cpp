/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackTest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:25:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 15:25:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/Callback.hpp"

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

TEST(CallbackTest, Constructor)
{
    int testFd = 42;
    const char* testSource = "test_source";
    
    Callback callback(testCallbackFunction, testFd, NULL, testSource);
    
    EXPECT_EQ(callback.getFd(), testFd);
    EXPECT_FALSE(callback.isCancelled());
    EXPECT_STREQ(callback.getSource(), testSource);
}

TEST(CallbackTest, Execute)
{
    int testFd = 42;
    bool flag = false;
    
    Callback callback(testFlagCallbackFunction, testFd, &flag);
    
    // Execute the callback
    callback.execute();
    
    // Check that the flag was set
    EXPECT_TRUE(flag);
}

TEST(CallbackTest, Cancel)
{
    int testFd = 42;
    bool flag = false;
    
    Callback callback(testFlagCallbackFunction, testFd, &flag);
    
    // Cancel the callback
    callback.cancel();
    
    // Execute the callback (should not set the flag)
    callback.execute();
    
    // Check that the flag was not set
    EXPECT_FALSE(flag);
    EXPECT_TRUE(callback.isCancelled());
}

TEST(CallbackTest, GetCreationTime)
{
    int testFd = 42;
    
    Callback callback(testCallbackFunction, testFd);
    
    // Check that the creation time is reasonable (within the last minute)
    time_t now = time(NULL);
    EXPECT_LE(callback.getCreationTime(), now);
    EXPECT_GE(callback.getCreationTime(), now - 60);
}
