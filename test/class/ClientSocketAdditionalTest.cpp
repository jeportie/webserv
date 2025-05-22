/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocketAdditionalTest.cpp                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jeportie <jeportie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 00:00:00 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/19 00:00:00 by jeportie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../src/class/Sockets/ClientSocket.hpp"

// Enhanced mock class for testing ClientSocket with simulated data reception
class MockClientSocket : public ClientSocket {
public:
    MockClientSocket() : ClientSocket(), _disconnected(false), _dataBuffer("") {}
    
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
    
    // Simulate receiving data
    int simulateReceive(char* buffer, size_t bufferSize) {
        if (_disconnected) {
            return 0; // Simulate EOF (peer disconnection)
        }
        
        size_t bytesToCopy = std::min(bufferSize - 1, _dataBuffer.length());
        if (bytesToCopy == 0) {
            return -1; // No data available
        }
        
        memcpy(buffer, _dataBuffer.c_str(), bytesToCopy);
        buffer[bytesToCopy] = '\0'; // Null-terminate
        
        // Remove the copied data from the buffer
        _dataBuffer = _dataBuffer.substr(bytesToCopy);
        
        return bytesToCopy;
    }
    
    // Add data to the receive buffer
    void addData(const std::string& data) {
        _dataBuffer += data;
    }
    
    // Simulate peer disconnection
    void simulateDisconnect() {
        _disconnected = true;
    }
    
    // Reset the socket state for reuse
    void reset() {
        _disconnected = false;
        _dataBuffer.clear();
    }
    
    // Get the current data buffer (for testing)
    std::string getDataBuffer() const {
        return _dataBuffer;
    }
    
private:
    bool _disconnected;
    std::string _dataBuffer;
};

// Test FD reuse scenario
TEST(ClientSocketAdditionalTest, FdReuseScenario)
{
    MockClientSocket socket;
    socket.socketCreate();
    
    // Set up client address for first connection
    struct sockaddr_in addr1;
    memset(&addr1, 0, sizeof(addr1));
    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(8080);
    addr1.sin_addr.s_addr = inet_addr("192.168.1.1");
    socket.setClientAddr(addr1, sizeof(addr1));
    
    // Add some data for the first connection
    socket.addData("First connection data");
    
    // Verify the data is available
    char buffer[256];
    int bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_GT(bytesRead, 0);
    EXPECT_STREQ(buffer, "First connection data");
    
    // Simulate client disconnect
    socket.simulateDisconnect();
    bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_EQ(bytesRead, 0); // Should indicate EOF
    
    // Reset the socket for reuse (simulating a new connection on the same FD)
    socket.reset();
    
    // Set up client address for second connection
    struct sockaddr_in addr2;
    memset(&addr2, 0, sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_port = htons(9090);
    addr2.sin_addr.s_addr = inet_addr("192.168.1.2");
    socket.setClientAddr(addr2, sizeof(addr2));
    
    // Verify no data leakage from previous connection
    bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_EQ(bytesRead, -1); // No data available
    
    // Add data for the second connection
    socket.addData("Second connection data");
    
    // Verify the new data is available
    bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_GT(bytesRead, 0);
    EXPECT_STREQ(buffer, "Second connection data");
    
    // Verify client address was updated
    EXPECT_EQ(ntohs(socket.getClientAddr().sin_port), 9090);
    EXPECT_EQ(socket.getClientIP(), "192.168.1.2");
}

// Test partial data reception
TEST(ClientSocketAdditionalTest, PartialDataReception)
{
    MockClientSocket socket;
    socket.socketCreate();
    
    // Add data in fragments
    socket.addData("First");
    
    // Read the first fragment
    char buffer[256];
    int bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_GT(bytesRead, 0);
    EXPECT_STREQ(buffer, "First");
    
    // Add more data
    socket.addData(" fragment");
    
    // Read the second fragment
    bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_GT(bytesRead, 0);
    EXPECT_STREQ(buffer, " fragment");
    
    // Add data larger than the buffer
    std::string largeData(300, 'A'); // 300 'A' characters
    socket.addData(largeData);
    
    // Read with a smaller buffer (should only get part of the data)
    char smallBuffer[100];
    bytesRead = socket.simulateReceive(smallBuffer, sizeof(smallBuffer));
    EXPECT_EQ(bytesRead, 99); // 99 bytes + null terminator
    
    // Verify the remaining data is still in the buffer
    EXPECT_EQ(socket.getDataBuffer().length(), 201); // 300 - 99 = 201
}

// Test peer disconnection handling
TEST(ClientSocketAdditionalTest, PeerDisconnectionHandling)
{
    MockClientSocket socket;
    socket.socketCreate();
    
    // Add some data
    socket.addData("Data before disconnect");
    
    // Read the data
    char buffer[256];
    int bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_GT(bytesRead, 0);
    EXPECT_STREQ(buffer, "Data before disconnect");
    
    // Simulate peer disconnection
    socket.simulateDisconnect();
    
    // Try to read after disconnection
    bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_EQ(bytesRead, 0); // Should indicate EOF
    
    // Add more data after disconnection (shouldn't be readable)
    socket.addData("Data after disconnect");
    
    // Try to read again
    bytesRead = socket.simulateReceive(buffer, sizeof(buffer));
    EXPECT_EQ(bytesRead, 0); // Should still indicate EOF
}

// Test integration with real socket operations (skipped in CI environment)
TEST(ClientSocketAdditionalTest, RealSocketIntegration)
{
    // Skip this test in CI environments or when real network testing is not possible
    GTEST_SKIP() << "Skipping real network test - requires actual network access";
    
    // The rest of the test is kept for reference but will be skipped
    // In a real environment with network access, you would remove the GTEST_SKIP line
    
    // Create a real client socket
    ClientSocket socket;
    
    // Create the socket
    EXPECT_TRUE(socket.socketCreate());
    
    // Set non-blocking mode
    EXPECT_EQ(socket.setNonBlocking(socket.getFd()), 0);
    EXPECT_TRUE(socket.isNonBlocking());
    
    // In a real test, you would:
    // 1. Connect to a server
    // 2. Send and receive data
    // 3. Test disconnection handling
    
    // Clean up
    socket.closeSocket();
}
