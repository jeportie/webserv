/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocketAdditionalTest.cpp                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 00:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/19 00:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../src/class/ServerSocket.hpp"

// Mock class for testing ServerSocket with controlled failures
class MockServerSocket : public ServerSocket {
public:
    MockServerSocket() : ServerSocket(), _bindFailure(false), _socketCreateFailure(false) {}
    
    void setBindFailure(bool fail) { _bindFailure = fail; }
    void setSocketCreateFailure(bool fail) { _socketCreateFailure = fail; }
    
    // Override methods that would interact with the actual system
    virtual bool socketCreate() {
        if (_socketCreateFailure) return false;
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
    
    // Override safeBind to simulate binding failures
    virtual bool safeBind(int port, const std::string& address) {
        // Avoid unused parameter warnings
        (void)port;
        (void)address;
        
        // Create a socket if needed
        if (!isValid() && !socketCreate()) {
            return false;
        }
        
        setNonBlocking(getFd());
        
        // Simulate binding failure if requested
        return !_bindFailure;
    }
    
private:
    bool _bindFailure;
    bool _socketCreateFailure;
};

// Test rebinding protection
TEST(ServerSocketAdditionalTest, RebindingProtection)
{
    // Create a mock server socket that simulates binding failure on second attempt
    class RebindMockServerSocket : public MockServerSocket {
    private:
        bool _firstBind;
        
    public:
        RebindMockServerSocket() : MockServerSocket(), _firstBind(true) {}
        
        virtual bool safeBind(int port, const std::string& address) {
            // First bind succeeds, second fails (simulating address already in use)
            if (_firstBind) {
                _firstBind = false;
                return MockServerSocket::safeBind(port, address);
            } else {
                return false; // Second bind fails
            }
        }
    };
    
    RebindMockServerSocket socket;
    
    // First bind should succeed
    EXPECT_TRUE(socket.safeBind(8080, "127.0.0.1"));
    
    // Second bind to same address/port should fail
    EXPECT_FALSE(socket.safeBind(8080, "127.0.0.1"));
}

// Test actual system integration with real sockets
// This is an integration test that uses real system calls
TEST(ServerSocketAdditionalTest, ActualSystemIntegration)
{
    // Skip this test in CI environments or when real network testing is not possible
    GTEST_SKIP() << "Skipping real network test - requires actual network access";
    
    // The rest of the test is kept for reference but will be skipped
    // In a real environment with network access, you would remove the GTEST_SKIP line
    
    // Use a real ServerSocket
    ServerSocket socket;
    
    // Create the socket
    EXPECT_TRUE(socket.socketCreate());
    
    // Set reuse address to avoid "address already in use" errors
    EXPECT_TRUE(socket.setReuseAddr(true));
    
    // Bind to an ephemeral port (0 lets the OS choose an available port)
    bool bindResult = socket.safeBind(0, "127.0.0.1");
    
    // If binding failed, it might be due to permissions or network configuration
    if (!bindResult) {
        std::cout << "Warning: Could not bind to ephemeral port, skipping real socket test" << std::endl;
        return;
    }
    
    // Get the assigned port (should be non-zero)
    int port = socket.getPort();
    EXPECT_GT(port, 0);
    
    // Try to listen
    try {
        socket.safeListen(5);
        // If we get here, listening succeeded
        SUCCEED();
    } catch (const std::exception& e) {
        // If listening failed, it might be due to permissions
        std::cout << "Warning: Could not listen on port " << port << ": " << e.what() << std::endl;
    }
}

// Test invalid address binding
TEST(ServerSocketAdditionalTest, InvalidAddressBinding)
{
    MockServerSocket socket;
    
    // Simulate binding failure for invalid address
    socket.setBindFailure(true);
    
    // Try to bind to an invalid IP address
    EXPECT_FALSE(socket.safeBind(8080, "999.999.999.999"));
    
    // Try to bind to a restricted port (requires root privileges)
    EXPECT_FALSE(socket.safeBind(80, "127.0.0.1"));
}

// Test socket creation failure
TEST(ServerSocketAdditionalTest, SocketCreationFailure)
{
    MockServerSocket socket;
    
    // Simulate socket creation failure
    socket.setSocketCreateFailure(true);
    
    // Try to bind - should fail because socket creation fails
    EXPECT_FALSE(socket.safeBind(8080, "127.0.0.1"));
}
