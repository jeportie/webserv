/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketAdditionalTest.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 00:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/19 00:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include "../../src/class/Sockets/Socket.hpp"

// Since Socket is an abstract class, we need a concrete implementation for testing
class TestSocket : public Socket {
public:
    TestSocket() : Socket() {}
    virtual ~TestSocket() {}
    
    // Implement pure virtual methods
    virtual int setNonBlocking(int fd)
    { 
        if (fd < 0) return -1;
        _isNonBlocking = true;
        return 0; 
    }
    
    virtual int safeFcntl(int fd, int cmd, int flag)
    {
        (void) cmd;
        (void) flag;
        if (fd < 0) return -1;
        return 0;
    }
    
    // Override socketCreate to allow failure simulation
    bool socketCreate(bool shouldFail = false)
    {
        if (shouldFail) return false;
        _socketFd = 42; // Mock file descriptor
        return true;
    }
};

// Test double-close protection
TEST(SocketAdditionalTest, DoubleCloseProtection)
{
    TestSocket socket;
    socket.socketCreate();
    EXPECT_TRUE(socket.isValid());
    
    // First close
    socket.closeSocket();
    EXPECT_FALSE(socket.isValid());
    
    // Second close should not crash
    EXPECT_NO_THROW(socket.closeSocket());
    EXPECT_FALSE(socket.isValid());
}

// Test invalid FD handling
TEST(SocketAdditionalTest, InvalidFdHandling)
{
    TestSocket socket;
    
    // Test with invalid FD
    socket.setFd(-1);
    EXPECT_FALSE(socket.isValid());
    
    // setReuseAddr should handle invalid FD gracefully
    EXPECT_FALSE(socket.setReuseAddr());
    
    // setNonBlocking should handle invalid FD gracefully
    EXPECT_EQ(socket.setNonBlocking(socket.getFd()), -1);
}

// Test failure simulation
TEST(SocketAdditionalTest, FailureSimulation)
{
    TestSocket socket;
    
    // Test socketCreate failure
    EXPECT_FALSE(socket.socketCreate(true));
    EXPECT_FALSE(socket.isValid());
    
    // Test safeFcntl failure with invalid FD
    EXPECT_EQ(socket.safeFcntl(-1, 0, 0), -1);
}
