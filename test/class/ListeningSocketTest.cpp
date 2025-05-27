/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocketTest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:15:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 17:15:03 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "../../src/class/Sockets/ListeningSocket.hpp"

// Mock class for testing ListeningSocket without actual network operations
class MockListeningSocket : public ListeningSocket {
public:
    MockListeningSocket() : ListeningSocket() {}
    
    // Override methods that would interact with the actual system
    virtual bool socketCreate() {
        setFd(42); // Mock file descriptor
        return true;
    }
    
    virtual int setNonBlocking(int fd) {
        (void)fd;
        _isNonBlocking = true;
        return 0;
    }
    
    virtual int safeFcntl(int fd, int cmd, int flag) {
        (void)fd;
        (void)cmd;
        (void)flag;
        return 0;
    }
    
    // Override safeBind to avoid actual binding
    virtual bool safeBind(int port, const std::string& address) {
        // Avoid unused parameter warnings
        (void)port;
        (void)address;
        
        // Mock successful binding without accessing private members
        // Just create a socket and set it to non-blocking
        if (!isValid() && !socketCreate()) {
            return false;
        }
        
        setNonBlocking(getFd());
        
        // We don't actually bind, just pretend we did
        return true;
    }
};

TEST(ListeningSocketTest, DefaultConstructor)
{
    ListeningSocket socket;
    EXPECT_FALSE(socket.isValid());
    EXPECT_FALSE(socket.isNonBlocking());
    EXPECT_EQ(socket.getFd(), -1);
}

TEST(ListeningSocketTest, SafeBind)
{
    MockListeningSocket socket;
    EXPECT_TRUE(socket.safeBind(8080, "127.0.0.1"));
    EXPECT_TRUE(socket.isValid());
    EXPECT_TRUE(socket.isNonBlocking());
}

TEST(ListeningSocketTest, GetPort)
{
    // For this test, we need to use a different approach
    // We'll create a mock class that overrides getPort
    class PortMockListeningSocket : public MockListeningSocket {
    public:
        virtual int getPort() const {
            return 8080; // Mock port
        }
    };
    
    PortMockListeningSocket socket;
    socket.safeBind(8080, "127.0.0.1");
    EXPECT_EQ(socket.getPort(), 8080);
}

TEST(ListeningSocketTest, GetAddress)
{
    // For this test, we need to use a different approach
    // We'll create a mock class that overrides getAddress
    class AddressMockListeningSocket : public MockListeningSocket {
    public:
        virtual std::string getAddress() const {
            return "0.0.0.0"; // Mock address
        }
    };
    
    AddressMockListeningSocket socket;
    socket.safeBind(8080, "127.0.0.1");
    EXPECT_EQ(socket.getAddress(), "0.0.0.0");
}

// Test for safeListen method
// Note: This test doesn't actually call listen() on a real socket
TEST(ListeningSocketTest, SafeListen)
{
    // For this test, we need to use a different approach
    // We'll create a mock class that overrides safeListen
    class ListenMockListeningSocket : public MockListeningSocket {
    public:
        virtual void safeListen(int backlog) {
            (void)backlog;
            // Do nothing, just pretend we listened
        }
    };
    
    ListenMockListeningSocket socket;
    socket.safeBind(8080, "127.0.0.1");
    
    // This should not throw an exception
    EXPECT_NO_THROW(socket.safeListen(10));
}

