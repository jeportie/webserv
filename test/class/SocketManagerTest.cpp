/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketManagerTest.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anastruc <anastruc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 01:28:02 by jeportie          #+#    #+#             */
/*   Updated: 2025/05/15 18:32:50 by anastruc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <thread>
#include <chrono>
#include <dirent.h>  // For DIR, opendir, readdir, closedir
#include <sys/epoll.h>  // For epoll functions
#include <string.h>  // For strerror() and strlen()
#define private public
#define protected public
#include "../src/class/SocketManager.hpp"
#undef private
#undef protected


// #include "../../src/class/SocketManager.hpp"
// #include "../../src/class/Socket.hpp"

// using ::testing::Test;
// using ::testing::Return;
// using ::testing::_;

// // Mock class for testing
// class MockSocketManager : public SocketManager {
// public:
//     // Expose protected methods for testing
//     using SocketManager::SocketManager;
    
//     // Get the server socket file descriptor
//     int getServerSocketFd() const {
//         return getServerSocket();
//     }
    
//     // Get the client socket file descriptor
//     int getClientSocketFd() const {
//         return getClientSocket();
//     }
    
//     // Initialize the server socket
//     void initializeServerSocket(int port, const std::string& address) {
//         // Create a server socket and bind it
//         ServerSocket serverSocket;
//         serverSocket.safeBind(port, address);
//     }
    
//     // Create a new ServerSocket for testing since we can't access the private _serverSocket
//     ServerSocket createTestServerSocket() {
//         ServerSocket serverSocket;
//         return serverSocket;
//     }
// };

// class SocketManagerTest : public Test {
// protected:
//     MockSocketManager socketManager;

//     // Helper method to get a unique port for testing
//     int getUniquePort() {
//         // Use a dynamic port range (49152-65535)
//         static int basePort = 49152;
//         return basePort++;
//     }

//     // Helper method to create a test client socket
//     int createTestClientSocket() {
//         int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//         if (clientSocket < 0) {
//             return -1;
//         }
//         return clientSocket;
//     }

//     void SetUp() override {
//         // Any setup needed before each test
//     }

//     void TearDown() override {
//         // Cleanup after each test
//     }
// };

// // Test constructor and destructor
// TEST_F(SocketManagerTest, Constructor) {
//     SocketManager manager;
//     EXPECT_EQ(manager.getServerSocket(), -1);
//     EXPECT_EQ(manager.getClientSocket(), -1);
// }

// // Test socket creation with invalid parameters
// TEST_F(SocketManagerTest, InvalidSocketCreation) {
//     // Test with invalid socket domain
//     int invalidSocket = socket(999, SOCK_STREAM, 0);
//     EXPECT_EQ(invalidSocket, -1) << "Socket creation with invalid domain should fail";
//     EXPECT_EQ(errno, EAFNOSUPPORT) << "Expected EAFNOSUPPORT error for invalid domain";
    
//     // Test with invalid socket type
//     invalidSocket = socket(AF_INET, 999, 0);
//     EXPECT_EQ(invalidSocket, -1) << "Socket creation with invalid type should fail";
//     EXPECT_EQ(errno, EPROTOTYPE) << "Expected EPROTOTYPE error for invalid socket type";
    
//     // Test socket creation with invalid protocol
//     invalidSocket = socket(AF_INET, SOCK_STREAM, 999);
//     EXPECT_EQ(invalidSocket, -1) << "Socket creation with invalid protocol should fail";
//     EXPECT_EQ(errno, EPROTONOSUPPORT) << "Expected EPROTONOSUPPORT error for invalid protocol";
// }

// // Test server socket binding
// TEST_F(SocketManagerTest, ServerSocketBinding) {
//     int port = getUniquePort();
    
//     // Create a server socket directly (not through SocketManager)
//     ServerSocket serverSocket;
//     bool bindResult = serverSocket.safeBind(port, "127.0.0.1");
    
//     EXPECT_TRUE(bindResult) << "Failed to bind server socket to port " << port;
//     EXPECT_NE(serverSocket.getFd(), -1) << "Server socket file descriptor is invalid";
    
//     // Verify the port and address
//     EXPECT_EQ(serverSocket.getPort(), port);
//     EXPECT_EQ(serverSocket.getAddress(), "127.0.0.1");
// }

// // Test binding to IPv6 address
// TEST_F(SocketManagerTest, IPv6SocketBinding) {
//     int port = getUniquePort();
    
//     // Create a server socket directly (not through SocketManager)
//     ServerSocket serverSocket;
//     bool bindResult = serverSocket.safeBind(port, "::1"); // IPv6 loopback address
    
//     EXPECT_TRUE(bindResult) << "Failed to bind server socket to IPv6 address";
//     EXPECT_NE(serverSocket.getFd(), -1) << "Server socket file descriptor is invalid";
    
//     // Verify the port and address
//     EXPECT_EQ(serverSocket.getPort(), port);
//     EXPECT_EQ(serverSocket.getAddress(), "::1");
// }

// // Test binding to invalid address
// TEST_F(SocketManagerTest, InvalidAddressBinding) {
//     int port = getUniquePort();
    
//     // Create a server socket and try to bind it to an invalid address
//     ServerSocket serverSocket = socketManager.createTestServerSocket();
//     bool bindResult = serverSocket.safeBind(port, "invalid_address");
    
//     EXPECT_FALSE(bindResult) << "Binding to invalid address should fail";
// }

// // Test binding to already in-use port
// TEST_F(SocketManagerTest, PortAlreadyInUse) {
//     int port = getUniquePort();
    
//     // Create and bind the first server socket
//     ServerSocket serverSocket1 = socketManager.createTestServerSocket();
//     bool bindResult1 = serverSocket1.safeBind(port, "127.0.0.1");
//     ASSERT_TRUE(bindResult1) << "Failed to bind first server socket";
    
//     // Create a second socket and try to bind to the same port
//     int secondSocket = socket(AF_INET, SOCK_STREAM, 0);
//     ASSERT_GE(secondSocket, 0) << "Failed to create second socket";
    
//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(port);
//     addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
//     int bindResult2 = bind(secondSocket, (struct sockaddr*)&addr, sizeof(addr));
//     EXPECT_EQ(bindResult2, -1) << "Binding to already in-use port should fail";
//     EXPECT_EQ(errno, EADDRINUSE) << "Expected EADDRINUSE error for port already in use";
    
//     // Clean up
//     close(secondSocket);
// }

// // Test server socket listening
// TEST_F(SocketManagerTest, ServerSocketListening) {
//     int port = getUniquePort();
    
//     // Create and bind a server socket
//     ServerSocket serverSocket = socketManager.createTestServerSocket();
//     bool bindResult = serverSocket.safeBind(port, "127.0.0.1");
//     ASSERT_TRUE(bindResult) << "Failed to bind server socket";
    
//     // Start listening
//     ASSERT_NO_THROW(serverSocket.safeListen(5)) << "Failed to start listening on server socket";
// }

// // Test client connection acceptance
// TEST_F(SocketManagerTest, ClientConnectionAcceptance) {
//     int port = getUniquePort();
    
//     // Create, bind, and listen on a server socket
//     ServerSocket serverSocket = socketManager.createTestServerSocket();
//     bool bindResult = serverSocket.safeBind(port, "127.0.0.1");
//     ASSERT_TRUE(bindResult) << "Failed to bind server socket";
//     ASSERT_NO_THROW(serverSocket.safeListen(5)) << "Failed to start listening";
    
//     // Create an epoll instance for testing
//     int epoll_fd = epoll_create(1);
//     ASSERT_GE(epoll_fd, 0) << "Failed to create epoll instance";
    
//     // Create a client socket and connect to the server in a separate thread
//     std::thread clientThread([port]() {
//         // Give the server time to start listening
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
//         // Create client socket
//         int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//         ASSERT_GE(clientSocket, 0) << "Failed to create client socket";
        
//         // Connect to the server
//         struct sockaddr_in serverAddr;
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_port = htons(port);
//         serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
//         int connectResult = connect(clientSocket, 
//                                    (struct sockaddr*)&serverAddr, 
//                                    sizeof(serverAddr));
//         EXPECT_EQ(connectResult, 0) << "Failed to connect to server: " << strerror(errno);
        
//         // Send a test message
//         const char* testMessage = "Hello, Server!";
//         send(clientSocket, testMessage, strlen(testMessage), 0);
        
//         // Wait a bit before closing
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
//         // Close the client socket
//         close(clientSocket);
//     });
    
//     // Accept the client connection
//     ClientSocket* clientSocket = nullptr;
//     ASSERT_NO_THROW(clientSocket = serverSocket.safeAccept(epoll_fd)) 
//         << "Failed to accept client connection";
    
//     // Verify the client socket
//     ASSERT_NE(clientSocket, nullptr) << "Client socket is null";
//     EXPECT_NE(clientSocket->getFd(), -1) << "Client socket file descriptor is invalid";
//     EXPECT_TRUE(clientSocket->isNonBlocking()) << "Client socket is not in non-blocking mode";
    
//     // Clean up
//     clientThread.join();
//     close(epoll_fd);
//     delete clientSocket;
// }

// // Test non-blocking socket behavior
// TEST_F(SocketManagerTest, NonBlockingSocketBehavior) {
//     int port = getUniquePort();
    
//     // Create, bind, and listen on a server socket
//     ServerSocket serverSocket = socketManager.createTestServerSocket();
//     bool bindResult = serverSocket.safeBind(port, "127.0.0.1");
//     ASSERT_TRUE(bindResult) << "Failed to bind server socket";
//     ASSERT_NO_THROW(serverSocket.safeListen(5)) << "Failed to start listening";
    
//     // Verify that the server socket is in non-blocking mode
//     int flags = fcntl(serverSocket.getFd(), F_GETFL, 0);
//     EXPECT_TRUE(flags & O_NONBLOCK) << "Server socket is not in non-blocking mode";
    
//     // Create an epoll instance for testing
//     int epoll_fd = epoll_create(1);
//     ASSERT_GE(epoll_fd, 0) << "Failed to create epoll instance";
    
//     // Try to accept a connection without any client (should not block)
//     ClientSocket* clientSocket = nullptr;
//     ASSERT_THROW(clientSocket = serverSocket.safeAccept(epoll_fd), std::runtime_error)
//         << "Non-blocking accept should throw when no clients are connecting";
    
//     // Clean up
//     close(epoll_fd);
// }

// // Test multiple client connections
// TEST_F(SocketManagerTest, MultipleClientConnections) {
//     int port = getUniquePort();
//     const int NUM_CLIENTS = 3;
    
//     // Create, bind, and listen on a server socket
//     ServerSocket serverSocket = socketManager.createTestServerSocket();
//     bool bindResult = serverSocket.safeBind(port, "127.0.0.1");
//     ASSERT_TRUE(bindResult) << "Failed to bind server socket";
//     ASSERT_NO_THROW(serverSocket.safeListen(10)) << "Failed to start listening";
    
//     // Create an epoll instance for testing
//     int epoll_fd = epoll_create(1);
//     ASSERT_GE(epoll_fd, 0) << "Failed to create epoll instance";
    
//     // Create multiple client threads
//     std::vector<std::thread> clientThreads;
//     for (int i = 0; i < NUM_CLIENTS; ++i) {
//         clientThreads.push_back(std::thread([port, i]() {
//             // Stagger connections slightly
//             std::this_thread::sleep_for(std::chrono::milliseconds(50 * i));
            
//             // Create client socket
//             int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//             ASSERT_GE(clientSocket, 0) << "Failed to create client socket " << i;
            
//             // Connect to the server
//             struct sockaddr_in serverAddr;
//             serverAddr.sin_family = AF_INET;
//             serverAddr.sin_port = htons(port);
//             serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            
//             int connectResult = connect(clientSocket, 
//                                        (struct sockaddr*)&serverAddr, 
//                                        sizeof(serverAddr));
//             EXPECT_EQ(connectResult, 0) << "Client " << i << " failed to connect: " << strerror(errno);
            
//             // Send a test message
//             std::string testMessage = "Hello from client " + std::to_string(i);
//             send(clientSocket, testMessage.c_str(), testMessage.length(), 0);
            
//             // Wait a bit before closing
//             std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
//             // Close the client socket
//             close(clientSocket);
//         }));
//     }
    
//     // Accept client connections
//     std::vector<ClientSocket*> clientSockets;
//     for (int i = 0; i < NUM_CLIENTS; ++i) {
//         // Wait a bit to ensure the client has connected
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
//         ClientSocket* clientSocket = nullptr;
//         try {
//             clientSocket = serverSocket.safeAccept(epoll_fd);
//             EXPECT_NE(clientSocket, nullptr) << "Failed to accept client " << i;
//             EXPECT_NE(clientSocket->getFd(), -1) << "Client " << i << " socket fd is invalid";
//             clientSockets.push_back(clientSocket);
//         } catch (const std::exception& e) {
//             FAIL() << "Exception while accepting client " << i << ": " << e.what();
//         }
//     }
    
//     // Clean up
//     for (auto& thread : clientThreads) {
//         thread.join();
//     }
//     for (auto* socket : clientSockets) {
//         delete socket;
//     }
//     close(epoll_fd);
// }

// // Test socket cleanup
// TEST_F(SocketManagerTest, SocketCleanup) {
//     int port = getUniquePort();
    
//     // Create, bind, and listen on a server socket
//     ServerSocket serverSocket = socketManager.createTestServerSocket();
//     bool bindResult = serverSocket.safeBind(port, "127.0.0.1");
//     ASSERT_TRUE(bindResult) << "Failed to bind server socket";
//     ASSERT_NO_THROW(serverSocket.safeListen(5)) << "Failed to start listening";
    
//     // Create an epoll instance for testing
//     int epoll_fd = epoll_create(1);
//     ASSERT_GE(epoll_fd, 0) << "Failed to create epoll instance";
    
//     // Create a client socket and connect to the server
//     std::thread clientThread([port]() {
//         // Give the server time to start listening
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
//         // Create client socket
//         int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//         ASSERT_GE(clientSocket, 0) << "Failed to create client socket";
        
//         // Connect to the server
//         struct sockaddr_in serverAddr;
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_port = htons(port);
//         serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
//         int connectResult = connect(clientSocket, 
//                                    (struct sockaddr*)&serverAddr, 
//                                    sizeof(serverAddr));
//         EXPECT_EQ(connectResult, 0) << "Failed to connect to server: " << strerror(errno);
        
//         // Wait a bit before closing
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
//         // Close the client socket
//         close(clientSocket);
//     });
    
//     // Accept the client connection
//     ClientSocket* clientSocket = nullptr;
//     try {
//         clientSocket = serverSocket.safeAccept(epoll_fd);
//         ASSERT_NE(clientSocket, nullptr) << "Failed to accept client connection";
        
//         // Store the file descriptor for later verification
//         int clientFd = clientSocket->getFd();
        
//         // Close the connection
//         clientSocket->closeSocket();
        
//         // Verify that the socket is closed
//         EXPECT_FALSE(clientSocket->isValid()) << "Socket should be invalid after closing";
        
//         // Attempt to use the closed socket (should fail)
//         char buffer[1] = {0};
//         EXPECT_EQ(write(clientFd, buffer, 1), -1);
//         EXPECT_EQ(errno, EBADF) << "Socket was not properly closed";
//     } catch (const std::exception& e) {
//         FAIL() << "Exception while accepting client: " << e.what();
//     }
    
//     // Clean up
//     clientThread.join();
//     delete clientSocket; `
//     close(epoll_fd);
// }

// // Test resource monitoring for file descriptors
// TEST_F(SocketManagerTest, ResourceMonitoring) {
//     int port = getUniquePort();
    
//     // Get the initial count of open file descriptors
//     int initialFdCount = 0;
//     DIR* dir = opendir("/proc/self/fd");
//     if (dir != nullptr) {
//         while (readdir(dir) != nullptr) {
//             initialFdCount++;
//         }
//         closedir(dir);
//     }
//     // Subtract 3 for stdin, stdout, stderr and 1 for the dir we just opened
//     initialFdCount -= 4;
    
//     // Create, bind, and listen on a server socket
//     ServerSocket& serverSocket = socketManager.getServerSocketObj();
//     bool bindResult = serverSocket.safeBind(port, "127.0.0.1");
//     ASSERT_TRUE(bindResult) << "Failed to bind server socket";
//     ASSERT_NO_THROW(serverSocket.safeListen(5)) << "Failed to start listening";
    
//     // Create an epoll instance for testing
//     int epoll_fd = epoll_create(1);
//     ASSERT_GE(epoll_fd, 0) << "Failed to create epoll instance";
    
//     // Create multiple client connections
//     const int NUM_CLIENTS = 5;
//     std::vector<int> clientSockets;
    
//     for (int i = 0; i < NUM_CLIENTS; ++i) {
//         int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//         ASSERT_GE(clientSocket, 0) << "Failed to create client socket " << i;
        
//         struct sockaddr_in serverAddr;
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_port = htons(port);
//         serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        
//         int connectResult = connect(clientSocket, 
//                                    (struct sockaddr*)&serverAddr, 
//                                    sizeof(serverAddr));
//         EXPECT_EQ(connectResult, 0) << "Client " << i << " failed to connect: " << strerror(errno);
        
//         clientSockets.push_back(clientSocket);
//     }
    
//     // Check that file descriptor count has increased
//     int currentFdCount = 0;
//     dir = opendir("/proc/self/fd");
//     if (dir != nullptr) {
//         while (readdir(dir) != nullptr) {
//             currentFdCount++;
//         }
//         closedir(dir);
//     }
//     // Subtract 3 for stdin, stdout, stderr and 1 for the dir we just opened
//     currentFdCount -= 4;
    
//     // We should have at least NUM_CLIENTS + 2 (server socket + epoll) more file descriptors
//     EXPECT_GE(currentFdCount, initialFdCount + NUM_CLIENTS + 2) 
//         << "File descriptor count should increase with open sockets";
    
//     // Close all client sockets
//     for (int clientSocket : clientSockets) {
//         close(clientSocket);
//     }
    
//     // Close the epoll instance
//     close(epoll_fd);
    
//     // Check that file descriptor count has decreased
//     int finalFdCount = 0;
//     dir = opendir("/proc/self/fd");
//     if (dir != nullptr) {
//         while (readdir(dir) != nullptr) {
//             finalFdCount++;
//         }
//         closedir(dir);
//     }
//     // Subtract 3 for stdin, stdout, stderr and 1 for the dir we just opened
//     finalFdCount -= 4;
    
//     // We should be back to approximately the initial count (may be off by 1-2 due to test framework)
//     EXPECT_NEAR(finalFdCount, initialFdCount, 2) 
//         << "File descriptor count should return to initial value after closing sockets";
// }


// test/SocketManagerTest.cpp


// Permet d’accéder aux membres privés/protégés pour les tester

// Fixture qui crée un SocketManager propre à chaque test
class SocketManagerTest : public ::testing::Test {
protected:
    SocketManager manager;
    int sv[2];                  // socketpair pour simuler client<->serveur
    ClientSocket* clientSock;   // wrapper pour fd sv[1]
    int clientFd;

    virtual void SetUp() {
        // création de la paire de sockets
        ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, sv))
            << "socketpair() failed: " << strerror(errno);
        // on considère sv[0] comme "serveur" et sv[1] comme "client"
        clientFd     = sv[1];
        clientSock   = new ClientSocket();
        clientSock->setFd(clientFd);
        clientSock->setHeadersParsed(false);
        clientSock->setContentLength(0);
        clientSock->setChunked(false);
        clientSock->setChunkSize(0);

        manager._clientSockets[clientFd] = clientSock;
    }

    virtual void TearDown() {
        manager.closeConnection(clientFd, /*epoll_fd*/ -1);
        close(sv[0]);
    }
};

// 1) readFromClient doit accumuler dans le buffer
TEST_F(SocketManagerTest, ReadFromClientAppendsData) {
    const char* msg = "HelloSocket";
    write(sv[0], msg, strlen(msg));
    close(sv[0]);

    // buffer initial vide
    EXPECT_TRUE(clientSock->getBuffer().empty());
    // appelle readFromClient
    EXPECT_TRUE(manager.readFromClient(clientFd));
    // buffer doit contenir msg
    EXPECT_EQ(clientSock->getBuffer(), std::string(msg));
}

// 2) parseClientHeaders extrait headers et vide le buffer en-têtes
TEST_F(SocketManagerTest, ParseClientHeadersExtractsAndTrims) {
    const char* raw =
        "GET /foo HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Length: 5\r\n"
        "\r\n"
        "ABCDE";  // reste de body
    write(sv[0], raw, strlen(raw));
    close(sv[0]);
    
    manager.readFromClient(clientFd);

    // Avant parsing
    EXPECT_FALSE(clientSock->headersParsed());
    EXPECT_NE(clientSock->getBuffer().find("Content-Length"),
          std::string::npos);

    bool ok = manager.parseClientHeaders(clientSock);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(clientSock->headersParsed());
    EXPECT_EQ(clientSock->getContentLength(), size_t(5));

    // Buffer doit commencer par le début du body
    EXPECT_EQ(clientSock->getBuffer(), std::string("ABCDE"));
}

// 3) parseClientBody attend le corps complet et le place dans HttpRequest
TEST_F(SocketManagerTest, ParseClientBodyFillsRequestBody) {
    // Simule qu'on a déjà parsé headers et mis contentLength + buffer = body+reste
    clientSock->setHeadersParsed(true);
    clientSock->setContentLength(4);
    clientSock->getBuffer() = "WXYZ123";  // 4 octets de body + "123" restant

    HttpRequest req;
    bool ok = manager.parseClientBody(clientSock, req);
    EXPECT_TRUE(ok);
    EXPECT_EQ(req.body, "WXYZ");
}

// 4) cleanupRequest consomme exactement ContentLength octets et reset flags
TEST_F(SocketManagerTest, CleanupRequestTrimsBufferAndResets) {
    clientSock->setContentLength(3);
    clientSock->setHeadersParsed(true);
    // buffer = "123ABC"
    clientSock->getBuffer() = "123ABC";

    manager.cleanupRequest(clientSock);
    // doit avoir supprimé "123"
    EXPECT_EQ(clientSock->getBuffer(), std::string("ABC"));
    EXPECT_FALSE(clientSock->headersParsed());
    EXPECT_EQ(clientSock->getContentLength(), size_t(0));
}


TEST_F(SocketManagerTest, ParseClientBody_ContentLengthIncomplete) {
    // absence de chunked, Content-Length=10, mais buffer contient 5 octets
    clientSock->setChunked(false);
    clientSock->setContentLength(10);
    clientSock->getBuffer() = "12345";

    HttpRequest req;
    bool ok = manager.parseClientBody(clientSock, req);
    EXPECT_FALSE(ok);
    EXPECT_TRUE(req.body.empty());
}

TEST_F(SocketManagerTest, ParseClientBody_ChunkedSingleChunk) {
    // un seul chunk de 4 octets: "Test"
    const char* chunked =
        "4\r\n"
        "Test\r\n"
        "0\r\n"
        "\r\n";
    clientSock->setChunked(true);
    clientSock->setChunkSize(0);
    clientSock->getBuffer() = chunked;

    HttpRequest req;
    bool ok = manager.parseClientBody(clientSock, req);
    EXPECT_TRUE(ok);
    EXPECT_EQ(req.body, "Test");
    EXPECT_TRUE(clientSock->getBuffer().empty());
}

TEST_F(SocketManagerTest, ParseClientBody_ChunkedMultipleChunks) {
    // deux chunks: "Ab" + "Cde"
    const char* chunked =
        "2\r\n"
        "Ab\r\n"
        "3\r\n"
        "Cde\r\n"
        "0\r\n"
        "\r\n";
    clientSock->setChunked(true);
    clientSock->setChunkSize(0);
    clientSock->getBuffer() = chunked;

    HttpRequest req;
    bool ok = manager.parseClientBody(clientSock, req);
    EXPECT_TRUE(ok);
    EXPECT_EQ(req.body, "AbCde");
    EXPECT_TRUE(clientSock->getBuffer().empty());
}

TEST_F(SocketManagerTest, ParseClientBody_ChunkedIncompleteHeader) {
    // header chunker tronqué
    clientSock->setChunked(true);
    clientSock->setChunkSize(0);
    clientSock->getBuffer() = "A";  // pas de \r\n

    HttpRequest req;
    bool ok = manager.parseClientBody(clientSock, req);
    EXPECT_FALSE(ok);
    EXPECT_TRUE(req.body.empty());
}

TEST_F(SocketManagerTest, ParseClientBody_ChunkedIncompleteData) {
    // taille 4, mais seulement 2 octets + CRLF partiel
    clientSock->setChunked(true);
    clientSock->setChunkSize(0);
    clientSock->getBuffer() =
        "4\r\n"
        "XY";  // pas assez de data ni CRLF

    HttpRequest req;
    bool ok = manager.parseClientBody(clientSock, req);
    EXPECT_FALSE(ok);
    EXPECT_TRUE(req.body.empty());
}


