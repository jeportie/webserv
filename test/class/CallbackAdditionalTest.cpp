/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CallbackAdditionalTest.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 00:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/19 00:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/Callback.hpp"

// Test callback function that modifies data
static void dataModifyingCallback(int fd, void* data)
{
    (void) fd;
    int* value = static_cast<int*>(data);
    if (value) {
        *value += 10; // Add 10 to the value
    }
}

// Test for null function pointer handling
TEST(CallbackAdditionalTest, NullFunctionPointerHandling)
{
    int testFd = 42;
    
    // Create a callback with a null function pointer
    // Note: In a real implementation, you might want to prevent NULL function pointers
    // in the constructor, but we're testing the current implementation
    Callback callback(NULL, testFd);
    
    // We don't call execute() since it would cause a segmentation fault
    // Instead, we verify other properties are still correct
    EXPECT_EQ(callback.getFd(), testFd);
    EXPECT_FALSE(callback.isCancelled());
    
    // Cancel the callback
    callback.cancel();
    EXPECT_TRUE(callback.isCancelled());
}

// Test for data integrity
TEST(CallbackAdditionalTest, DataIntegrityTest)
{
    int testFd = 42;
    int testData = 5;
    
    // Create a callback with data
    Callback callback(dataModifyingCallback, testFd, &testData);
    
    // Execute the callback
    callback.execute();
    
    // Check that the data was correctly modified
    EXPECT_EQ(testData, 15); // 5 + 10 = 15
    
    // Execute again to verify consistent behavior
    callback.execute();
    
    // Check that the data was modified again
    EXPECT_EQ(testData, 25); // 15 + 10 = 25
}
