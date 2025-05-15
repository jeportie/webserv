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

TEST(CallbackTest, Constructor)
{
    int testFd = 42;
    const char* testSource = "test_source";
    
    Callback callback(testCallbackFunction, testFd, NULL, testSource);
    
    EXPECT_EQ(callback.getFd(), testFd);
    EXPECT_FALSE(callback.isCancelled());
    EXPECT_STREQ(callback.getSource(), testSource);
}
